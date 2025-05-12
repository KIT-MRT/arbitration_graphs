from arbitration_graphs_py import Behavior
from arbitration_graphs_py import CommandWrapper


class PrintStrings:
    strike_through_on = "×××\010\010\010\033[9m"
    strike_through_off = "\033[29m\033[8m×××\033[28m"

    invocation_true = "\033[32mINVOCATION\033[39m "
    invocation_false = "\033[31mInvocation\033[39m "
    commitment_true = "\033[32mCOMMITMENT\033[39m "
    commitment_false = "\033[31mCommitment\033[39m "


class DummyCommandPy(str):
    pass


class DummyBehaviorPy(Behavior):
    def __init__(
        self, invocation: bool, commitment: bool, name: str = "dummy_behavior"
    ):
        super().__init__(name)
        self.invocation_condition = invocation
        self.commitment_condition = commitment
        self.lose_control_counter = 0

    def get_command(self, time):
        return CommandWrapper(self.name)

    def check_invocation_condition(self, time) -> bool:
        return self.invocation_condition

    def check_commitment_condition(self, time) -> bool:
        return self.commitment_condition

    def lose_control(self, time):
        self.lose_control_counter += 1
