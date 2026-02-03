"""News feed ingestion pipeline package."""

from .config import PipelineConfig
from .pipeline import NewsIngestPipeline

__all__ = ["PipelineConfig", "NewsIngestPipeline"]
