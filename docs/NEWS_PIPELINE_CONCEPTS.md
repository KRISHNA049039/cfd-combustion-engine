# News Pipeline Concepts

This document describes the core concepts used by the news ingestion pipeline located in `python/news_pipeline`.

## Kafka Concepts

- **Topic**: A named stream of records. The pipeline subscribes to a single topic and consumes new records in order per partition.
- **Partition**: An ordered, immutable sequence of records within a topic. Kafka guarantees ordering within each partition. The pipeline stores the partition and offset to support traceability.
- **Offset**: A monotonically increasing integer identifying a record within a partition. Offsets allow replay or auditing of ingested data.
- **Consumer Group**: A group of consumers that share the work of reading a topic. The pipeline uses a configurable group ID to manage offsets.

## S3 Concepts

- **Bucket**: A top-level container for objects in S3. The pipeline requires a bucket to store raw payloads.
- **Object Key**: The full path of an object within a bucket. The pipeline uses a structured key layout: `s3://<bucket>/<prefix>/YYYY/MM/DD/<message_id>.json`.
- **Prefix**: A logical folder-like path component used for grouping objects. The pipeline uses a configurable prefix to separate news data from other data.

## DynamoDB Concepts

- **Table**: A collection of items identified by a primary key. The pipeline writes a catalog entry for each message.
- **Primary Key**: The unique identifier for each item. The pipeline expects a table with a partition key named `message_id` (string).
- **Item**: A collection of attributes. Each item stores Kafka offsets, S3 location, payload hash, and timestamps.

## Pipeline-Specific Concepts

- **Message ID**: A deterministic identifier derived from the Kafka message key (preferred) or payload hash. This ID is used in both the S3 key and DynamoDB primary key.
- **Payload Hash**: A SHA-256 hash of the message payload. Stored in DynamoDB for integrity checks and deduplication.
- **Event Time**: The time an event occurred, read from `published_at` or `event_time` in the JSON payload when available. Used to place objects into date-partitioned prefixes.
- **Ingested At**: The timestamp when the pipeline processed the message. Stored in DynamoDB for traceability.
