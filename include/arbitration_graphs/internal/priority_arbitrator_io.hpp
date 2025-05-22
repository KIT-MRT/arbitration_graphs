#pragma once

#include "../priority_arbitrator.hpp"


namespace arbitration_graphs {


//////////////////////////////////////
//    PriorityArbitrator::Option    //
//////////////////////////////////////

template <typename CommandT, typename SubCommandT>
std::ostream& PriorityArbitrator<CommandT, SubCommandT>::Option::to_stream(std::ostream& output,
                                                                           const Time& time,
                                                                           const int& option_index,
                                                                           const std::string& prefix,
                                                                           const std::string& suffix) const {

    output << option_index + 1 << ". ";
    ArbitratorBase::Option::to_stream(output, time, option_index, prefix, suffix);
    return output;
}


//////////////////////////////////////
//        PriorityArbitrator        //
//////////////////////////////////////

template <typename CommandT, typename SubCommandT>
YAML::Node PriorityArbitrator<CommandT, SubCommandT>::toYaml(const Time& time) const {
    YAML::Node node = ArbitratorBase::toYaml(time);
    node["type"] = "PriorityArbitrator";
    return node;
}

} // namespace arbitration_graphs
