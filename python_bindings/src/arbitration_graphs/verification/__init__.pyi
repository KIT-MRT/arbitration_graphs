# pyright: reportAny=false,reportExplicitAny=false

from typing_extensions import override

from arbitration_graphs.typing import Command, EnvironmentModel, Time

__all__ = ["PlaceboVerifier", "Result", "SimpleResult", "Verifier"]

class PlaceboVerifier(Verifier):
    def __init__(self) -> None: ...

class Result:
    def __init__(self) -> None: ...
    def is_ok(self) -> bool: ...

class SimpleResult(Result):
    def __init__(self, is_ok: bool) -> None: ...
    @override
    def is_ok(self) -> bool: ...

class Verifier:
    def __init__(self) -> None: ...
    def analyze(
        self,
        time: Time,
        environment_model: EnvironmentModel,
        command: Command,
    ) -> Result: ...
