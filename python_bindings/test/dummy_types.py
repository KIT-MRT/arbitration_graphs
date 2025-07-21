from typing_extensions import override

from arbitration_graphs import Behavior
from arbitration_graphs.typing import Time
from arbitration_graphs.verification import Result, Verifier


class PrintStrings:
    strike_through_on: str = "×××\010\010\010\033[9m"
    strike_through_off: str = "\033[29m\033[8m×××\033[28m"

    invocation_true: str = "\033[32mINVOCATION\033[39m "
    invocation_false: str = "\033[31mInvocation\033[39m "
    commitment_true: str = "\033[32mCOMMITMENT\033[39m "
    commitment_false: str = "\033[31mCommitment\033[39m "


class DummyEnvironmentModel:
    def __init__(self):
        self.access_counter: int = 0

    # A mock getter function that simulates accessing the environment model.
    def get_observation(self) -> None:
        self.access_counter += 1


class DummyCommand(str):
    pass


class DummyBehavior(Behavior):
    def __init__(
        self, invocation: bool, commitment: bool, name: str = "dummy_behavior"
    ):
        super().__init__(name)
        self.invocation_condition: bool = invocation
        self.commitment_condition: bool = commitment
        self.lose_control_counter: int = 0

    @override
    def get_command(
        self, time: Time, environment_model: DummyEnvironmentModel
    ) -> DummyCommand:
        # While computing a command, we can read the environment model to get the required context. In this mock
        # implementation, we merely simulate this access to assure that this interaction is possible. You will find
        # analogous calls to the environment model in the other methods of this class.
        environment_model.get_observation()
        return DummyCommand(self.name())

    @override
    def check_invocation_condition(
        self, time: Time, environment_model: DummyEnvironmentModel
    ) -> bool:
        environment_model.get_observation()
        return self.invocation_condition

    @override
    def check_commitment_condition(
        self, time: Time, environment_model: DummyEnvironmentModel
    ) -> bool:
        environment_model.get_observation()
        return self.commitment_condition

    @override
    def gain_control(
        self, time: Time, environment_model: DummyEnvironmentModel
    ) -> None:
        environment_model.get_observation()

    @override
    def lose_control(
        self, time: Time, environment_model: DummyEnvironmentModel
    ) -> None:
        environment_model.get_observation()
        self.lose_control_counter += 1


class BrokenDummyBehavior(DummyBehavior):
    def __init__(
        self,
        invocation: bool,
        commitment: bool,
        name: str = "broken_dummy_behavior",
        num_get_commands_until_raise: int = 0,
    ):
        super().__init__(invocation, commitment, name)
        self.num_get_commands_until_raise: int = num_get_commands_until_raise
        self.get_command_counter: int = 0

    @override
    def get_command(
        self, time: Time, environment_model: DummyEnvironmentModel
    ) -> DummyCommand:
        if self.get_command_counter >= self.num_get_commands_until_raise:
            raise Exception("BrokenDummyBehavior::getCommand() is broken")
        environment_model.get_observation()

        self.get_command_counter += 1
        return DummyCommand(self.name())


class DummyResult(Result):
    def __init__(self, is_ok: bool = True):
        Result.__init__(self)
        self._is_ok: bool = is_ok

    @override
    def is_ok(self) -> bool:
        return self._is_ok

    @override
    def __str__(self):
        return "is okay" if self.is_ok() else "is not okay"


class DummyVerifier(Verifier):
    def __init__(self, wrong: str = "", reject_all: bool = False):
        Verifier.__init__(self)
        self.wrong: str = wrong
        self.reject_all: bool = reject_all

    @override
    def analyze(
        self,
        time: Time,
        environment_model: DummyEnvironmentModel,
        command: DummyCommand,
    ) -> DummyResult:
        if self.reject_all or command == self.wrong:
            return DummyResult(False)

        return DummyResult(True)
