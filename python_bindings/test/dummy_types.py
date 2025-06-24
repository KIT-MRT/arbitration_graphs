from arbitration_graphs import Behavior
from arbitration_graphs.verification import AbstractResult, AbstractVerifier


class PrintStrings:
    strike_through_on = "×××\010\010\010\033[9m"
    strike_through_off = "\033[29m\033[8m×××\033[28m"

    invocation_true = "\033[32mINVOCATION\033[39m "
    invocation_false = "\033[31mInvocation\033[39m "
    commitment_true = "\033[32mCOMMITMENT\033[39m "
    commitment_false = "\033[31mCommitment\033[39m "


class DummyCommand(str):
    pass


class DummyBehavior(Behavior):
    def __init__(
        self, invocation: bool, commitment: bool, name: str = "dummy_behavior"
    ):
        super().__init__(name)
        self.invocation_condition = invocation
        self.commitment_condition = commitment
        self.lose_control_counter = 0

    def get_command(self, time):
        return self.name

    def check_invocation_condition(self, time) -> bool:
        return self.invocation_condition

    def check_commitment_condition(self, time) -> bool:
        return self.commitment_condition

    def lose_control(self, time):
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
        self.num_get_commands_until_raise = num_get_commands_until_raise
        self.get_command_counter = 0

    def get_command(self, time):
        if self.get_command_counter >= self.num_get_commands_until_raise:
            raise Exception("BrokenDummyBehavior::getCommand() is broken")

        self.get_command_counter += 1
        return self.name


class DummyResult(AbstractResult):
    def __init__(self, is_ok=True):
        self._is_ok = is_ok

    def is_ok(self):
        return self._is_ok

    def __str__(self):
        return "is okay" if self.is_ok() else "is not okay"


class DummyVerifier(AbstractVerifier):
    def __init__(self, wrong="", reject_all=False):
        super().__init__()
        self.wrong = wrong
        self.reject_all = reject_all

    def analyze(self, time, command):
        if self.reject_all or command == self.wrong:
            return DummyResult(False)

        return DummyResult(True)
