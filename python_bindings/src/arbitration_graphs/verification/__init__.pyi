from __future__ import annotations
import datetime

__all__ = ["PlaceboVerifier", "Result", "SimpleResult", "Verifier"]

class PlaceboVerifier(Verifier):
    def __init__(self) -> None: ...

class Result:
    def __init__(self) -> None: ...
    def is_ok(self) -> bool: ...

class SimpleResult(Result):
    def __init__(self, is_ok: bool) -> None: ...
    def is_ok(self) -> bool: ...

class Verifier:
    def __init__(self) -> None: ...
    def analyze(
        self,
        time: datetime.timedelta,
        environment_model: EnvironmentModelWrapper,
        command: CommandWrapper,
    ) -> Result: ...
