#pragma once

#include "../conjunctive_coordinator.hpp"


namespace arbitration_graphs {

//////////////////////////////////////////
//    ConjunctiveCoordinator::Option    //
//////////////////////////////////////////

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
std::ostream& ConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option::to_stream(
    std::ostream& output,
    const Time& time,
    const int& option_index,
    const std::string& prefix,
    const std::string& suffix) const {

    output << "- ";
    ArbitratorBase::Option::to_stream(output, time, option_index, prefix, suffix);
    return output;
}


//////////////////////////////////////////
//        ConjunctiveCoordinator        //
//////////////////////////////////////////

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
std::ostream& ConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>::to_stream(
    std::ostream& output, const Time& time, const std::string& prefix, const std::string& suffix) const {

    Behavior<CommandT>::to_stream(output, time, prefix, suffix);

    for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
        typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

        if (isActive_) {
            output << suffix << std::endl << prefix << " -> ";
        } else {
            output << suffix << std::endl << prefix << "    ";
        }
        option->to_stream(output, time, i, "    " + prefix, suffix);
    }
    return output;
}

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
YAML::Node ConjunctiveCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>::toYaml(
    const Time& time) const {

    YAML::Node node = ArbitratorBase::toYaml(time);
    node["type"] = "ConjunctiveCoordinator";
    return node;
}

} // namespace arbitration_graphs