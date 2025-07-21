from __future__ import annotations
import datetime
import typing
from . import verification

__all__ = [
    "ApplicableOptionFailedVerificationError",
    "Arbitrator",
    "Behavior",
    "CostArbitrator",
    "CostEstimator",
    "GetCommandCalledWithoutGainControlError",
    "InvalidArgumentsError",
    "InvocationConditionIsFalseError",
    "MultipleReferencesToSameInstanceError",
    "NoApplicableOptionPassedVerificationError",
    "PriorityArbitrator",
    "RandomArbitrator",
    "VerificationError",
    "verification",
]

class ApplicableOptionFailedVerificationError(Exception):
    pass

class Arbitrator(Behavior):
    class Option:
        class Flags:
            """
            Members:

              NO_FLAGS

              INTERRUPTABLE

              FALLBACK
            """

            FALLBACK: typing.ClassVar[
                Arbitrator.Option.Flags
            ]  # value = <Flags.FALLBACK: 2>
            INTERRUPTABLE: typing.ClassVar[
                Arbitrator.Option.Flags
            ]  # value = <Flags.INTERRUPTABLE: 1>
            NO_FLAGS: typing.ClassVar[
                Arbitrator.Option.Flags
            ]  # value = <Flags.NO_FLAGS: 0>
            __members__: typing.ClassVar[
                dict[str, Arbitrator.Option.Flags]
            ]  # value = {'NO_FLAGS': <Flags.NO_FLAGS: 0>, 'INTERRUPTABLE': <Flags.INTERRUPTABLE: 1>, 'FALLBACK': <Flags.FALLBACK: 2>}
            def __eq__(self, other: typing.Any) -> bool: ...
            def __getstate__(self) -> int: ...
            def __hash__(self) -> int: ...
            def __index__(self) -> int: ...
            def __init__(self, value: typing.SupportsInt) -> None: ...
            def __int__(self) -> int: ...
            def __ne__(self, other: typing.Any) -> bool: ...
            def __repr__(self) -> str: ...
            def __setstate__(self, state: typing.SupportsInt) -> None: ...
            def __str__(self) -> str: ...
            @property
            def name(self) -> str: ...
            @property
            def value(self) -> int: ...

        def __init__(self, behavior: Behavior, flags: typing.SupportsInt) -> None: ...
        def behavior(self) -> Behavior: ...
        def has_flags(self, flags_to_check: typing.SupportsInt) -> bool: ...
        def verification_result(
            self, time: datetime.timedelta
        ) -> arbitration_graphs.arbitration_graphs_py.verification.Result | None: ...

    def __repr__(self) -> str: ...
    def add_option(self, behavior: Behavior, flags: typing.SupportsInt) -> None: ...
    def check_commitment_condition(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> bool: ...
    def check_invocation_condition(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> bool: ...
    def gain_control(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> None: ...
    def lose_control(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> None: ...
    def options(self) -> list[..., ..., ...]: ...

class Behavior:
    def __init__(self, name: str = "Behavior") -> None: ...
    def __repr__(self) -> str: ...
    def check_commitment_condition(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> bool: ...
    def check_invocation_condition(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> bool: ...
    def gain_control(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> None: ...
    def get_command(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> typing.Any: ...
    def lose_control(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> None: ...
    def name(self) -> str: ...
    def to_string(
        self,
        time: datetime.timedelta,
        environment_model: EnvironmentModelWrapper,
        prefix: str = "",
        suffix: str = "",
    ) -> str: ...
    def to_yaml(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> typing.Any: ...

class CostArbitrator(Arbitrator):
    class Option(Arbitrator.Option):
        class Flags:
            """
            Members:

              NO_FLAGS

              INTERRUPTABLE

              FALLBACK
            """

            FALLBACK: typing.ClassVar[
                CostArbitrator.Option.Flags
            ]  # value = <Flags.FALLBACK: 2>
            INTERRUPTABLE: typing.ClassVar[
                CostArbitrator.Option.Flags
            ]  # value = <Flags.INTERRUPTABLE: 1>
            NO_FLAGS: typing.ClassVar[
                CostArbitrator.Option.Flags
            ]  # value = <Flags.NO_FLAGS: 0>
            __members__: typing.ClassVar[
                dict[str, CostArbitrator.Option.Flags]
            ]  # value = {'NO_FLAGS': <Flags.NO_FLAGS: 0>, 'INTERRUPTABLE': <Flags.INTERRUPTABLE: 1>, 'FALLBACK': <Flags.FALLBACK: 2>}
            def __eq__(self, other: typing.Any) -> bool: ...
            def __getstate__(self) -> int: ...
            def __hash__(self) -> int: ...
            def __index__(self) -> int: ...
            def __init__(self, value: typing.SupportsInt) -> None: ...
            def __int__(self) -> int: ...
            def __ne__(self, other: typing.Any) -> bool: ...
            def __repr__(self) -> str: ...
            def __setstate__(self, state: typing.SupportsInt) -> None: ...
            def __str__(self) -> str: ...
            @property
            def name(self) -> str: ...
            @property
            def value(self) -> int: ...

        def __init__(
            self,
            behavior: Behavior,
            flags: typing.SupportsInt,
            cost_estimator: CostEstimator,
        ) -> None: ...

    def __init__(
        self, name: str = "CostArbitrator", verifier: verification.Verifier = ...
    ) -> None: ...
    def __repr__(self) -> str: ...
    def add_option(
        self,
        behavior: Behavior,
        flags: typing.SupportsInt,
        cost_estimator: CostEstimator,
    ) -> None: ...
    def to_yaml(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> typing.Any: ...

class CostEstimator:
    def __init__(self) -> None: ...
    def estimate_cost(
        self,
        time: datetime.timedelta,
        environment_model: EnvironmentModelWrapper,
        command: CommandWrapper,
        is_active: bool,
    ) -> float: ...

class GetCommandCalledWithoutGainControlError(Exception):
    pass

class InvalidArgumentsError(Exception):
    pass

class InvocationConditionIsFalseError(Exception):
    pass

class MultipleReferencesToSameInstanceError(Exception):
    pass

class NoApplicableOptionPassedVerificationError(Exception):
    pass

class PriorityArbitrator(Arbitrator):
    class Option(Arbitrator.Option):
        class Flags:
            """
            Members:

              NO_FLAGS

              INTERRUPTABLE

              FALLBACK
            """

            FALLBACK: typing.ClassVar[
                PriorityArbitrator.Option.Flags
            ]  # value = <Flags.FALLBACK: 2>
            INTERRUPTABLE: typing.ClassVar[
                PriorityArbitrator.Option.Flags
            ]  # value = <Flags.INTERRUPTABLE: 1>
            NO_FLAGS: typing.ClassVar[
                PriorityArbitrator.Option.Flags
            ]  # value = <Flags.NO_FLAGS: 0>
            __members__: typing.ClassVar[
                dict[str, PriorityArbitrator.Option.Flags]
            ]  # value = {'NO_FLAGS': <Flags.NO_FLAGS: 0>, 'INTERRUPTABLE': <Flags.INTERRUPTABLE: 1>, 'FALLBACK': <Flags.FALLBACK: 2>}
            def __eq__(self, other: typing.Any) -> bool: ...
            def __getstate__(self) -> int: ...
            def __hash__(self) -> int: ...
            def __index__(self) -> int: ...
            def __init__(self, value: typing.SupportsInt) -> None: ...
            def __int__(self) -> int: ...
            def __ne__(self, other: typing.Any) -> bool: ...
            def __repr__(self) -> str: ...
            def __setstate__(self, state: typing.SupportsInt) -> None: ...
            def __str__(self) -> str: ...
            @property
            def name(self) -> str: ...
            @property
            def value(self) -> int: ...

        def __init__(self, behavior: Behavior, flags: typing.SupportsInt) -> None: ...

    def __init__(
        self, name: str = "PriorityArbitrator", verifier: verification.Verifier = ...
    ) -> None: ...
    def __repr__(self) -> str: ...
    def add_option(self, behavior: Behavior, flags: typing.SupportsInt) -> None: ...
    def to_yaml(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> typing.Any: ...

class RandomArbitrator(Arbitrator):
    class Option(Arbitrator.Option):
        class Flags:
            """
            Members:

              NO_FLAGS

              INTERRUPTABLE

              FALLBACK
            """

            FALLBACK: typing.ClassVar[
                RandomArbitrator.Option.Flags
            ]  # value = <Flags.FALLBACK: 2>
            INTERRUPTABLE: typing.ClassVar[
                RandomArbitrator.Option.Flags
            ]  # value = <Flags.INTERRUPTABLE: 1>
            NO_FLAGS: typing.ClassVar[
                RandomArbitrator.Option.Flags
            ]  # value = <Flags.NO_FLAGS: 0>
            __members__: typing.ClassVar[
                dict[str, RandomArbitrator.Option.Flags]
            ]  # value = {'NO_FLAGS': <Flags.NO_FLAGS: 0>, 'INTERRUPTABLE': <Flags.INTERRUPTABLE: 1>, 'FALLBACK': <Flags.FALLBACK: 2>}
            def __eq__(self, other: typing.Any) -> bool: ...
            def __getstate__(self) -> int: ...
            def __hash__(self) -> int: ...
            def __index__(self) -> int: ...
            def __init__(self, value: typing.SupportsInt) -> None: ...
            def __int__(self) -> int: ...
            def __ne__(self, other: typing.Any) -> bool: ...
            def __repr__(self) -> str: ...
            def __setstate__(self, state: typing.SupportsInt) -> None: ...
            def __str__(self) -> str: ...
            @property
            def name(self) -> str: ...
            @property
            def value(self) -> int: ...

        def __init__(
            self,
            behavior: Behavior,
            flags: typing.SupportsInt,
            weight: typing.SupportsFloat,
        ) -> None: ...
        def weight(self) -> float: ...

    def __init__(
        self, name: str = "RandomArbitrator", verifier: verification.Verifier = ...
    ) -> None: ...
    def __repr__(self) -> str: ...
    def add_option(
        self,
        behavior: Behavior,
        flags: typing.SupportsInt,
        weight: typing.SupportsFloat = 1,
    ) -> None: ...
    def to_yaml(
        self, time: datetime.timedelta, environment_model: EnvironmentModelWrapper
    ) -> typing.Any: ...

class VerificationError(Exception):
    pass

__version__: str = '"0.8.0"'
