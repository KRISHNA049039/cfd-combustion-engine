# News Feed Ingestion Pipeline

This package provides a Kafka consumer that ingests news feed messages, stores the raw payloads in Amazon S3, and catalogs metadata in DynamoDB for downstream analysis.

For a primer on the underlying services and pipeline terminology, see [News Pipeline Concepts](../../docs/NEWS_PIPELINE_CONCEPTS.md).

## Architecture

1. **Kafka**: News feed messages are consumed from a configured topic.
2. **S3**: Raw payloads are written to `s3://<bucket>/<prefix>/YYYY/MM/DD/<message_id>.json`.
3. **DynamoDB**: Each message is cataloged with offsets, S3 location, and timestamps.

## Configuration

Configuration can be provided via environment variables or CLI flags.

| Setting | Env var | CLI flag | Default |
| --- | --- | --- | --- |
| Kafka bootstrap servers | `KAFKA_BOOTSTRAP_SERVERS` | `--bootstrap-servers` | `localhost:9092` |
| Kafka topic | `KAFKA_TOPIC` | `--topic` | `news` |
| Kafka group ID | `KAFKA_GROUP_ID` | `--group-id` | `news-ingest` |
| S3 bucket | `S3_BUCKET` | `--s3-bucket` | *(required)* |
| S3 prefix | `S3_PREFIX` | `--s3-prefix` | `news` |
| DynamoDB table | `DYNAMODB_TABLE` | `--dynamodb-table` | *(required)* |
| AWS region | `AWS_REGION` | `--region` | `us-east-1` |
| Max messages | `MAX_MESSAGES` | `--max-messages` | *(none)* |

## Required AWS Resources

- **S3 bucket** for raw payload storage.
- **DynamoDB table** with primary key `message_id` (string).

## IAM Permissions

The pipeline needs permission to:

- `s3:PutObject` on the configured bucket/prefix.
- `dynamodb:PutItem` on the configured table.
- `kafka:DescribeCluster`/`kafka:GetBootstrapBrokers` if using MSK IAM auth (not configured in this sample).

## Run the pipeline

```bash
export S3_BUCKET="my-news-bucket"
export DYNAMODB_TABLE="news-catalog"
python -m python.news_pipeline.pipeline \
  --bootstrap-servers "localhost:9092" \
  --topic "news" \
  --max-messages 100
```

## Notes

- The pipeline attempts to parse payloads as JSON and will use `published_at` or `event_time` if present.
- If the payload is not JSON, it will still be stored as `application/octet-stream`.
