#pragma once

#include <iomanip>

#include "../random_arbitrator.hpp"


namespace arbitration_graphs {


//////////////////////////////////////
//    RandomArbitrator::Option    //
//////////////////////////////////////

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
std::ostream& RandomArbitrator<EnvironmentModelT, CommandT, SubCommandT>::Option::to_stream(
    std::ostream& output,
    const Time& time,
    const EnvironmentModelT& environmentModel,
    const int& optionIndex,
    const std::string& prefix,
    const std::string& suffix) const {
    output << std::fixed << std::setprecision(3) << "- (weight: " << weight_ << ") ";
    ArbitratorBase::Option::to_stream(output, time, environmentModel, optionIndex, prefix, suffix);
    return output;
}


//////////////////////////////////////
//        RandomArbitrator        //
//////////////////////////////////////

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
YAML::Node RandomArbitrator<EnvironmentModelT, CommandT, SubCommandT>::toYaml(
    const Time& time, const EnvironmentModelT& environmentModel) const {
    YAML::Node node = ArbitratorBase::toYaml(time, environmentModel);
    node["type"] = "RandomArbitrator";
    return node;
}

} // namespace arbitration_graphs
