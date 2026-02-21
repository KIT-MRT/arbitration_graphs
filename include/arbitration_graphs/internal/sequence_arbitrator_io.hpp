#pragma once

#include "../sequence_arbitrator.hpp"


namespace arbitration_graphs {


///////////////////////////////////////
//    SequenceArbitrator::Option    //
///////////////////////////////////////

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
std::ostream& SequenceArbitrator<EnvironmentModelT, CommandT, SubCommandT>::Option::toStream(
    std::ostream& output,
    const Time& time,
    const EnvironmentModelT& environmentModel,
    const int& optionIndex,
    const std::string& prefix,
    const std::string& suffix) const {
    output << optionIndex + 1 << ". ";
    ArbitratorBase::Option::toStream(output, time, environmentModel, optionIndex, prefix, suffix);
    return output;
}


///////////////////////////////////////
//        SequenceArbitrator        //
///////////////////////////////////////

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
std::ostream& SequenceArbitrator<EnvironmentModelT, CommandT, SubCommandT>::toStream(
    std::ostream& output,
    const Time& time,
    const EnvironmentModelT& environmentModel,
    const std::string& prefix,
    const std::string& suffix) const {

    Behavior<EnvironmentModelT, CommandT>::toStream(output, time, environmentModel, prefix, suffix);

    for (int i = 0; i < static_cast<int>(this->options().size()); ++i) {
        const typename ArbitratorBase::Option::ConstPtr option = this->options().at(i);
        const bool isCurrent = sequenceStarted_ && currentBehaviorActivated_ && (i == currentIndex_);

        if (isCurrent) {
            output << suffix << '\n' << prefix << " -> ";
        } else {
            output << suffix << '\n' << prefix << "    ";
        }
        option->toStream(output, time, environmentModel, i, "    " + prefix, suffix);
    }
    return output;
}

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
YAML::Node SequenceArbitrator<EnvironmentModelT, CommandT, SubCommandT>::toYaml(
    const Time& time, const EnvironmentModelT& environmentModel) const {
    YAML::Node node = Behavior<EnvironmentModelT, CommandT>::toYaml(time, environmentModel);

    node["type"] = "SequenceArbitrator";
    for (const typename ArbitratorBase::Option::ConstPtr& option : this->options()) {
        node["options"].push_back(option->toYaml(time, environmentModel));
    }
    if (sequenceStarted_ && currentBehaviorActivated_) {
        node["activeBehavior"] = currentIndex_;
    }

    return node;
}

} // namespace arbitration_graphs
