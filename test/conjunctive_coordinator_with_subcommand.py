import os
import time
import unittest

import arbitration_graphs_py_with_subcommand as ag_subcommand


class TestConjunctiveCoordinatorWithSubCommand(unittest.TestCase):
    def test_sub_command_type_differs_from_command_type(self):
        now = time.time()

        # We cannot mix arbitration_graphs_py_with_subcommand and arbitration_graphs_py since they bind some of the same types.
        # To not create a another DummyBehavior implementation, we'll just use the pybind types for this test.
        test_behavior_a = ag_subcommand.testing_types.DummyBehavior(False, False, "A")
        test_behavior_b1 = ag_subcommand.testing_types.DummyBehavior(True, False, "B")
        test_behavior_c = ag_subcommand.testing_types.DummyBehavior(True, True, "C")
        test_behavior_b2 = ag_subcommand.testing_types.DummyBehavior(True, False, "B")

        test_conjunctive_coordinator = ag_subcommand.ConjunctiveCoordinator()

        test_conjunctive_coordinator.add_option(
            test_behavior_a,
            ag_subcommand.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        test_conjunctive_coordinator.add_option(
            test_behavior_b1,
            ag_subcommand.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        test_conjunctive_coordinator.add_option(
            test_behavior_c,
            ag_subcommand.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )
        test_conjunctive_coordinator.add_option(
            test_behavior_b2,
            ag_subcommand.ConjunctiveCoordinator.Option.Flags.NO_FLAGS,
        )

        test_behavior_a.invocation_condition = True
        test_conjunctive_coordinator.gain_control(now)

        expected = "ABCB"
        self.assertEqual(len(expected), test_conjunctive_coordinator.get_command(now))


if __name__ == "__main__":
    header = "Running " + os.path.basename(__file__)

    print("=" * len(header))
    print(header)
    print("=" * len(header) + "\n")
    unittest.main(exit=False)
    print("=" * len(header) + "\n")
