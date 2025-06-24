import sys
from .arbitration_graphs_py import *

_verification = getattr(arbitration_graphs_py, "verification")
sys.modules["arbitration_graphs.verification"] = _verification
