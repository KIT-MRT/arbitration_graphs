import sys
import os

build_directory = "/home/blinky/arbitration_graphs/build/"
sys.path.append(build_directory)

import time
import arbitration_graphs_py as ag

now = time.time()

dummy_behavior = ag.DummyBehavior(True, False, "TestBehavior")

print("Invocation: " + str(dummy_behavior.check_invocation_condition(now)))
print("Commitment: " + str(dummy_behavior.check_commitment_condition(now)))
print("Command: " + str(dummy_behavior.get_command(now)))
