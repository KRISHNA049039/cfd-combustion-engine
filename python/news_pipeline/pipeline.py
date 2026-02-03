"""Kafka-to-S3 ingestion pipeline with DynamoDB cataloging."""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
from typing import Any

import boto3
from kafka import KafkaConsumer

from .config import PipelineConfig


class NewsIngestPipeline:
    def __init__(self, config: PipelineConfig) -> None:
        self._config = config
        self._s3_client = boto3.client("s3", region_name=config.aws_region)
        self._ddb_client = boto3.client("dynamodb", region_name=config.aws_region)
        self._consumer = KafkaConsumer(
            config.kafka_topic,
            bootstrap_servers=[server.strip() for server in config.kafka_bootstrap_servers.split(",")],
            group_id=config.kafka_group_id,
            enable_auto_commit=True,
            auto_offset_reset="earliest",
        )

    def run(self) -> None:
        processed = 0
        for message in self._consumer:
            self._process_message(message)
            processed += 1
            if self._config.max_messages is not None and processed >= self._config.max_messages:
                break

    def _process_message(self, message: Any) -> None:
        payload = message.value or b""
        key = message.key or b""
        payload_hash = hashlib.sha256(payload).hexdigest()
        message_id = self._build_message_id(key, payload_hash)
        metadata = self._extract_metadata(payload)

        s3_key = self._build_s3_key(message_id, metadata)
        self._s3_client.put_object(
            Bucket=self._config.s3_bucket,
            Key=s3_key,
            Body=payload,
            ContentType=metadata.get("content_type", "application/json"),
        )

        self._ddb_client.put_item(
            TableName=self._config.dynamodb_table,
            Item={
                "message_id": {"S": message_id},
                "topic": {"S": message.topic},
                "partition": {"N": str(message.partition)},
                "offset": {"N": str(message.offset)},
                "s3_bucket": {"S": self._config.s3_bucket},
                "s3_key": {"S": s3_key},
                "payload_hash": {"S": payload_hash},
                "ingested_at": {"S": metadata["ingested_at"]},
                "event_time": {"S": metadata["event_time"]},
            },
        )

    def _build_message_id(self, key: bytes, payload_hash: str) -> str:
        if key:
            return hashlib.sha256(key).hexdigest()
        return payload_hash

    def _extract_metadata(self, payload: bytes) -> dict[str, str]:
        now = dt.datetime.now(tz=dt.timezone.utc)
        ingested_at = now.isoformat()
        event_time = ingested_at
        content_type = "application/json"

        if payload:
            try:
                parsed = json.loads(payload.decode("utf-8"))
                if isinstance(parsed, dict):
                    event_time_value = parsed.get("published_at") or parsed.get("event_time")
                    if isinstance(event_time_value, str) and event_time_value:
                        event_time = event_time_value
            except (json.JSONDecodeError, UnicodeDecodeError):
                content_type = "application/octet-stream"

        return {
            "ingested_at": ingested_at,
            "event_time": event_time,
            "content_type": content_type,
        }

    def _build_s3_key(self, message_id: str, metadata: dict[str, str]) -> str:
        event_time = metadata["event_time"]
        try:
            parsed_time = dt.datetime.fromisoformat(event_time.replace("Z", "+00:00"))
        except ValueError:
            parsed_time = dt.datetime.now(tz=dt.timezone.utc)
        date_prefix = parsed_time.strftime("%Y/%m/%d")
        return f"{self._config.s3_prefix}/{date_prefix}/{message_id}.json"


def _build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="News feed ingestion pipeline")
    parser.add_argument("--bootstrap-servers", default=None)
    parser.add_argument("--topic", default=None)
    parser.add_argument("--group-id", default=None)
    parser.add_argument("--s3-bucket", default=None)
    parser.add_argument("--s3-prefix", default=None)
    parser.add_argument("--dynamodb-table", default=None)
    parser.add_argument("--region", default=None)
    parser.add_argument("--max-messages", type=int, default=None)
    return parser


def _apply_overrides(config: PipelineConfig, args: argparse.Namespace) -> PipelineConfig:
    return PipelineConfig(
        kafka_bootstrap_servers=args.bootstrap_servers or config.kafka_bootstrap_servers,
        kafka_topic=args.topic or config.kafka_topic,
        kafka_group_id=args.group_id or config.kafka_group_id,
        s3_bucket=args.s3_bucket or config.s3_bucket,
        s3_prefix=args.s3_prefix or config.s3_prefix,
        dynamodb_table=args.dynamodb_table or config.dynamodb_table,
        aws_region=args.region or config.aws_region,
        max_messages=args.max_messages if args.max_messages is not None else config.max_messages,
        poll_timeout_seconds=config.poll_timeout_seconds,
    )


def main() -> None:
    parser = _build_arg_parser()
    args = parser.parse_args()
    config = _apply_overrides(PipelineConfig.from_env(), args)

    if not config.s3_bucket:
        raise ValueError("S3 bucket is required. Set S3_BUCKET or --s3-bucket.")
    if not config.dynamodb_table:
        raise ValueError("DynamoDB table is required. Set DYNAMODB_TABLE or --dynamodb-table.")

    pipeline = NewsIngestPipeline(config)
    pipeline.run()


if __name__ == "__main__":
    main()
