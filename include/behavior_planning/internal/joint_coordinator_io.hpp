#pragma once

#include "../joint_coordinator.hpp"


namespace behavior_planning {

////////////////////////////////////
//    JointCoordinator::Option    //
////////////////////////////////////

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
std::ostream& JointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Option::to_stream(
    std::ostream& output,
    const Time& time,
    const int& option_index,
    const std::string& prefix,
    const std::string& suffix) const {

    output << "- ";
    ArbitratorBase::Option::to_stream(output, time, option_index, prefix, suffix);
    return output;
}

////////////////////////////////////
//        JointCoordinator        //
////////////////////////////////////

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
std::ostream& JointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>::to_stream(
    std::ostream& output, const Time& time, const std::string& prefix, const std::string& suffix) const {

    Behavior<CommandT>::to_stream(output, time, prefix, suffix);

    for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
        typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

        /// \todo Put arrow only in front of active options, not to all if JointCoordinator is active
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
YAML::Node JointCoordinator<CommandT, SubCommandT, VerifierT, VerificationResultT>::toYaml(const Time& time) const {
    YAML::Node node = ArbitratorBase::toYaml(time);
    node["type"] = "JointCoordinator";
    return node;
}

} // namespace behavior_planning