# pyright: reportAny=false,reportExplicitAny=false

import typing

from typing_extensions import override

from arbitration_graphs import verification
from arbitration_graphs.typing import (
    ArbitratorYamlNode,
    Command,
    EnvironmentModel,
    Time,
)

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
            @override
            def __eq__(self, other: typing.Any) -> bool: ...
            def __getstate__(self) -> int: ...
            @override
            def __hash__(self) -> int: ...
            def __index__(self) -> int: ...
            def __init__(self, value: typing.SupportsInt) -> None: ...
            def __int__(self) -> int: ...
            @override
            def __ne__(self, other: typing.Any) -> bool: ...
            @override
            def __repr__(self) -> str: ...
            def __setstate__(self, state: typing.SupportsInt) -> None: ...
            @override
            def __str__(self) -> str: ...
            @property
            def name(self) -> str: ...
            @property
            def value(self) -> int: ...

        def __init__(self, behavior: Behavior, flags: typing.SupportsInt) -> None: ...
        def behavior(self) -> Behavior: ...
        def has_flags(self, flags_to_check: typing.SupportsInt) -> bool: ...
        def verification_result(self, time: Time) -> verification.Result | None: ...

    @override
    def __repr__(self) -> str: ...
    def add_option(self, behavior: Behavior, flags: typing.SupportsInt) -> None: ...
    @override
    def check_commitment_condition(
        self, time: Time, environment_model: EnvironmentModel
    ) -> bool: ...
    @override
    def check_invocation_condition(
        self, time: Time, environment_model: EnvironmentModel
    ) -> bool: ...
    @override
    def gain_control(self, time: Time, environment_model: EnvironmentModel) -> None: ...
    @override
    def lose_control(self, time: Time, environment_model: EnvironmentModel) -> None: ...
    def options(self) -> list[Option]: ...

class Behavior:
    def __init__(self, name: str = "Behavior") -> None: ...
    @override
    def __repr__(self) -> str: ...
    def check_commitment_condition(
        self, time: Time, environment_model: EnvironmentModel
    ) -> bool: ...
    def check_invocation_condition(
        self, time: Time, environment_model: EnvironmentModel
    ) -> bool: ...
    def gain_control(self, time: Time, environment_model: EnvironmentModel) -> None: ...
    def get_command(
        self, time: Time, environment_model: EnvironmentModel
    ) -> Command: ...
    def lose_control(self, time: Time, environment_model: EnvironmentModel) -> None: ...
    def name(self) -> str: ...
    def to_string(
        self,
        time: Time,
        environment_model: EnvironmentModel,
        prefix: str = "",
        suffix: str = "",
    ) -> str: ...
    def to_yaml(
        self, time: Time, environment_model: EnvironmentModel
    ) -> ArbitratorYamlNode: ...

class CostArbitrator(Arbitrator):
    class Option(Arbitrator.Option):
        # The nested "Flags" class intentionally differs between Arbitrator subtypes.
        # Each Arbitrator subclass defines its own Option.Flags type even though pyright
        # expects the child class to override the Flags enum of the parent class.
        # We therefore suppress the pyright warning.
        class Flags:  # pyright: ignore[reportIncompatibleVariableOverride]
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
            @override
            def __eq__(self, other: typing.Any) -> bool: ...
            def __getstate__(self) -> int: ...
            @override
            def __hash__(self) -> int: ...
            def __index__(self) -> int: ...
            def __init__(self, value: typing.SupportsInt) -> None: ...
            def __int__(self) -> int: ...
            @override
            def __ne__(self, other: typing.Any) -> bool: ...
            @override
            def __repr__(self) -> str: ...
            def __setstate__(self, state: typing.SupportsInt) -> None: ...
            @override
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
    @override
    def __repr__(self) -> str: ...
    @typing.overload
    def add_option(
        self,
        behavior: Behavior,
        flags: typing.SupportsInt,
    ) -> None:
        """
        Inherited from abstract Arbitrator. This overload is for compatibility with the base class.
        Use the overload passing a CostEstimator instead.
        """
        ...
    @typing.overload
    def add_option(
        self,
        behavior: Behavior,
        flags: typing.SupportsInt,
        cost_estimator: CostEstimator,
    ) -> None: ...
    @override
    def to_yaml(
        self, time: Time, environment_model: EnvironmentModel
    ) -> ArbitratorYamlNode: ...

class CostEstimator:
    def __init__(self) -> None: ...
    def estimate_cost(
        self,
        time: Time,
        environment_model: EnvironmentModel,
        command: Command,
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
        # The nested "Flags" class intentionally differs between Arbitrator subtypes.
        # Each Arbitrator subclass defines its own Option.Flags type even though pyright
        # expects the child class to override the Flags enum of the parent class.
        # We therefore suppress the pyright warning.
        class Flags:  # pyright: ignore[reportIncompatibleVariableOverride]
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
            @override
            def __eq__(self, other: typing.Any) -> bool: ...
            def __getstate__(self) -> int: ...
            @override
            def __hash__(self) -> int: ...
            def __index__(self) -> int: ...
            def __init__(self, value: typing.SupportsInt) -> None: ...
            def __int__(self) -> int: ...
            @override
            def __ne__(self, other: typing.Any) -> bool: ...
            @override
            def __repr__(self) -> str: ...
            def __setstate__(self, state: typing.SupportsInt) -> None: ...
            @override
            def __str__(self) -> str: ...
            @property
            def name(self) -> str: ...
            @property
            def value(self) -> int: ...

        def __init__(self, behavior: Behavior, flags: typing.SupportsInt) -> None: ...

    def __init__(
        self, name: str = "PriorityArbitrator", verifier: verification.Verifier = ...
    ) -> None: ...
    @override
    def __repr__(self) -> str: ...
    @override
    def add_option(self, behavior: Behavior, flags: typing.SupportsInt) -> None: ...
    @override
    def to_yaml(
        self, time: Time, environment_model: EnvironmentModel
    ) -> ArbitratorYamlNode: ...

class RandomArbitrator(Arbitrator):
    class Option(Arbitrator.Option):
        # The nested "Flags" class intentionally differs between Arbitrator subtypes.
        # Each Arbitrator subclass defines its own Option.Flags type even though pyright
        # expects the child class to override the Flags enum of the parent class.
        # We therefore suppress the pyright warning.
        class Flags:  # pyright: ignore[reportIncompatibleVariableOverride]
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
            @override
            def __eq__(self, other: typing.Any) -> bool: ...
            def __getstate__(self) -> int: ...
            @override
            def __hash__(self) -> int: ...
            def __index__(self) -> int: ...
            def __init__(self, value: typing.SupportsInt) -> None: ...
            def __int__(self) -> int: ...
            @override
            def __ne__(self, other: typing.Any) -> bool: ...
            @override
            def __repr__(self) -> str: ...
            def __setstate__(self, state: typing.SupportsInt) -> None: ...
            @override
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
    @override
    def __repr__(self) -> str: ...
    @override
    def add_option(
        self,
        behavior: Behavior,
        flags: typing.SupportsInt,
        weight: typing.SupportsFloat = 1,
    ) -> None: ...
    @override
    def to_yaml(
        self, time: Time, environment_model: EnvironmentModel
    ) -> ArbitratorYamlNode: ...

class VerificationError(Exception):
    pass

__version__: str
