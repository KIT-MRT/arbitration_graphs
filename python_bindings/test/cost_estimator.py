from collections.abc import Mapping
from typing import final

from typing_extensions import override

from arbitration_graphs import CostEstimator
from arbitration_graphs.typing import Time

from .dummy_types import DummyCommand, DummyEnvironmentModel


@final
class CostEstimatorFromCostMap(CostEstimator):
    def __init__(self, cost_map: Mapping[str, float], activation_costs: float = 0):
        super().__init__()
        self.cost_map = cost_map
        self.activation_costs = activation_costs

    @override
    def estimate_cost(
        self,
        time: Time,
        environment_model: DummyEnvironmentModel,
        command: DummyCommand,
        is_active: bool,
    ) -> float:
        # In an actual application, you might want to get data from the environment model here to help you determine
        # the cost of a command. In this mock implementation, we simply simulate this access to show that we could do
        # so.
        environment_model.get_observation()
        if is_active:
            return self.cost_map[command] / (1 + self.activation_costs)

        return (self.cost_map[command] + self.activation_costs) / (
            1 + self.activation_costs
        )
