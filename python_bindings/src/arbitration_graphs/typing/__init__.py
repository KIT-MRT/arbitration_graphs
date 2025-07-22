# pyright: reportExplicitAny=false

from __future__ import annotations

import datetime
from typing import Any, Literal, TypeAlias, TypedDict

EnvironmentModel: TypeAlias = Any
Command: TypeAlias = Any
Time: TypeAlias = datetime.timedelta | float


# =========================================================
# Types related to YAML representation of arbitration graph
# =========================================================

VerificationResult = Literal["passed", "failed"]


class BehaviorYamlNode(TypedDict):
    type: str
    name: str
    invocationCondition: bool
    commitmentCondition: bool


class ArbitratorYamlNode(BehaviorYamlNode):
    options: list[OptionYamlNode]
    activeBehavior: int | None


class OptionYamlNode(TypedDict):
    type: Literal["Option"]
    behavior: BehaviorYamlNode | ArbitratorYamlNode
    flags: list[str] | None
    verificationResult: VerificationResult | None
    cost: float | None
