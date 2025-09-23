# pyright: reportExplicitAny=false

from __future__ import annotations

import datetime
from typing import Any, Literal, TypedDict, Union

from typing_extensions import TypeAlias

# Both Command and EnvironmentModel can be Any as they will only be wrapped into the
# corresponding wrapper and "passed through" the arbitration graph. Adding this type alias
# does provide a little more semantic context for the hover info of functions such as get_command.
Command: TypeAlias = Any
EnvironmentModel: TypeAlias = Any

# We allow Time to be a float as well to support the use of time.time()
Time: TypeAlias = Union[datetime.timedelta, float]


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
