#pragma once

#include <iomanip>

#include "../random_arbitrator.hpp"


namespace arbitration_graphs {


//////////////////////////////////////
//    RandomArbitrator::Option    //
//////////////////////////////////////

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
std::ostream& RandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option::to_stream(
    std::ostream& output,
    const Time& time,
    const int& option_index,
    const std::string& prefix,
    const std::string& suffix) const {

    output << std::fixed << std::setprecision(3) << "- (weight: " << weight_ << ") ";
    ArbitratorBase::Option::to_stream(output, time, option_index, prefix, suffix);
    return output;
}


//////////////////////////////////////
//        RandomArbitrator        //
//////////////////////////////////////

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
YAML::Node RandomArbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::toYaml(const Time& time) const {
    YAML::Node node = ArbitratorBase::toYaml(time);
    node["type"] = "RandomArbitrator";
    return node;
}

} // namespace arbitration_graphs
