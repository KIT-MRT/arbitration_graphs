from arbitration_graphs import Behavior
from arbitration_graphs.verification import Result, Verifier


class PrintStrings:
    strike_through_on = "×××\010\010\010\033[9m"
    strike_through_off = "\033[29m\033[8m×××\033[28m"

    invocation_true = "\033[32mINVOCATION\033[39m "
    invocation_false = "\033[31mInvocation\033[39m "
    commitment_true = "\033[32mCOMMITMENT\033[39m "
    commitment_false = "\033[31mCommitment\033[39m "


class DummyEnvironmentModel:
    def __init__(self):
        self.access_counter = 0

    # A mock getter function that simulates accessing the environment model.
    def get_observation(self):
        self.access_counter += 1


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

    def get_command(self, time, environment_model):
        # While computing a command, we can read the environment model to get the required context. In this mock
        # implementation, we merely simulate this access to assure that this interaction is possible. You will find
        # analogous calls to the environment model in the other methods of this class.
        environment_model.get_observation()
        return self.name

    def check_invocation_condition(self, time, environment_model) -> bool:
        environment_model.get_observation()
        return self.invocation_condition

    def check_commitment_condition(self, time, environment_model) -> bool:
        environment_model.get_observation()
        return self.commitment_condition

    def gain_control(self, time, environment_model):
        environment_model.get_observation()

    def lose_control(self, time, environment_model):
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
        self.num_get_commands_until_raise = num_get_commands_until_raise
        self.get_command_counter = 0

    def get_command(self, time, environment_model):
        if self.get_command_counter >= self.num_get_commands_until_raise:
            raise Exception("BrokenDummyBehavior::getCommand() is broken")
        environment_model.get_observation()

        self.get_command_counter += 1
        return self.name


class DummyResult(Result):
    def __init__(self, is_ok=True):
        Result.__init__(self)
        self._is_ok = is_ok

    def is_ok(self):
        return self._is_ok

    def __str__(self):
        return "is okay" if self.is_ok() else "is not okay"


class DummyVerifier(Verifier):
    def __init__(self, wrong="", reject_all=False):
        Verifier.__init__(self)
        self.wrong = wrong
        self.reject_all = reject_all

    def analyze(self, time, environment_model, command):
        if self.reject_all or command == self.wrong:
            return DummyResult(False)

        return DummyResult(True)
