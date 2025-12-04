"""
CFD Combustion Engine Post-Processing Suite
"""

__version__ = "1.0.0"

from .data_reader import SimulationDataReader
from .visualization import ContourPlotter
from .spark_analysis import SparkKernelAnalyzer
from .analysis import AnalysisTools

__all__ = [
    'SimulationDataReader',
    'ContourPlotter',
    'SparkKernelAnalyzer',
    'AnalysisTools'
]
