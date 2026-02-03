"""Configuration for the news ingestion pipeline."""

from dataclasses import dataclass
import os


@dataclass(frozen=True)
class PipelineConfig:
    kafka_bootstrap_servers: str
    kafka_topic: str
    kafka_group_id: str
    s3_bucket: str
    s3_prefix: str
    dynamodb_table: str
    aws_region: str
    max_messages: int | None = None
    poll_timeout_seconds: float = 1.0

    @staticmethod
    def from_env() -> "PipelineConfig":
        return PipelineConfig(
            kafka_bootstrap_servers=os.environ.get("KAFKA_BOOTSTRAP_SERVERS", "localhost:9092"),
            kafka_topic=os.environ.get("KAFKA_TOPIC", "news"),
            kafka_group_id=os.environ.get("KAFKA_GROUP_ID", "news-ingest"),
            s3_bucket=os.environ.get("S3_BUCKET", ""),
            s3_prefix=os.environ.get("S3_PREFIX", "news"),
            dynamodb_table=os.environ.get("DYNAMODB_TABLE", ""),
            aws_region=os.environ.get("AWS_REGION", "us-east-1"),
            max_messages=int(os.environ["MAX_MESSAGES"]) if os.environ.get("MAX_MESSAGES") else None,
            poll_timeout_seconds=float(os.environ.get("POLL_TIMEOUT_SECONDS", "1.0")),
        )
