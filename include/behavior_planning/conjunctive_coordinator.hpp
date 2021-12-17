#pragma once

#include <memory>
#include <optional>

#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"
#include "exceptions.hpp"


namespace behavior_planning {

/*!
 * \brief The ConjunctiveCoordinator class combines all sub-commands by conjunction, using the operator&()
 */
template <typename CommandT, typename SubCommandT>
class ConjunctiveCoordinator : public Arbitrator<CommandT, SubCommandT> {
public:
    using ArbitratorBase = Arbitrator<CommandT, SubCommandT>;

    using Ptr = std::shared_ptr<ConjunctiveCoordinator>;
    using ConstPtr = std::shared_ptr<const ConjunctiveCoordinator>;

    struct Option : public ArbitratorBase::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NO_FLAGS = 0b0 };

        Option(const typename Behavior<SubCommandT>::Ptr& behavior, const FlagsT& flags)
                : ArbitratorBase::Option(behavior, flags) {
        }

        /*!
         * \brief Writes a string representation of the behavior option and its current state to the output stream.
         *
         * \param output        Output stream to write into, will be returned also
         * \param time          Expected execution time point of this behaviors command
         * \param option_index  Position index of this option within behaviorOptions_
         * \param prefix        A string that should be prepended to each line that is written to the output stream
         * \param suffix        A string that should be appended to each line that is written to the output stream
         * \return              The same given input stream (signature similar to std::ostream& operator<<())
         *
         * \see Arbitrator::to_stream()
         */
        virtual std::ostream& to_stream(std::ostream& output,
                                        const Time& time,
                                        const int& option_index,
                                        const std::string& prefix = "",
                                        const std::string& suffix = "") const {
            output << "- ";
            ArbitratorBase::Option::to_stream(output, time, option_index, prefix, suffix);
            return output;
        }
    };

    ConjunctiveCoordinator(const std::string& name = "ConjunctiveCoordinator") : ArbitratorBase(name) {
    }

    void addOption(const typename Behavior<SubCommandT>::Ptr& behavior, const typename Option::Flags& flags) {
        for (auto option : this->behaviorOptions_) {
            if (option->behavior_ == behavior) {
                throw MultipleReferencesToSameInstanceError(
                    "Coordinators cannot hold the same behavior instantiation multiple times!");
            }
        }

        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

    CommandT getCommand(const Time& time) override {
        SubCommandT subcommand_conjunction;

        for (auto& option_base : this->behaviorOptions_) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(option_base);
            subcommand_conjunction &= option->behavior_->getCommand(time);
        }

        return CommandT(subcommand_conjunction);
    }

    bool checkInvocationCondition(const Time& time) const override {
        if (this->behaviorOptions_.size() == 0) {
            return false;
        }
        for (auto& option : this->behaviorOptions_) {
            if (!option->behavior_->checkInvocationCondition(time)) {
                return false;
            }
        }
        return true;
    }
    bool checkCommitmentCondition(const Time& time) const override {
        if (!isActive_) {
            return false;
        }
        for (auto& option : this->behaviorOptions_) {
            if (option->behavior_->checkCommitmentCondition(time)) {
                return true;
            }
        }
        return false;
    }

    virtual void gainControl(const Time& time) override {
        for (auto& option : this->behaviorOptions_) {
            option->behavior_->gainControl(time);
        }
        isActive_ = true;
    }

    virtual void loseControl(const Time& time) override {
        isActive_ = false;
        for (auto& option : this->behaviorOptions_) {
            option->behavior_->loseControl(time);
        }
    }

    /*!
     * \brief Writes a string representation of the Coordinator object with its current state to the output stream.
     *
     * \param output    Output stream to write into, will be returned also
     * \param time      Expected execution time point of this behaviors command
     * \param prefix    A string that should be prepended to each line that is written to the output stream
     * \param suffix    A string that should be appended to each line that is written to the output stream
     * \return          The same given input stream (signature similar to std::ostream& operator<<())
     *
     * \see Behavior::to_stream()
     */
    virtual std::ostream& to_stream(std::ostream& output,
                                    const Time& time,
                                    const std::string& prefix = "",
                                    const std::string& suffix = "") const override {
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

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      Yaml representation of this behavior
     */
    virtual YAML::Node toYaml(const Time& time) const override {
        YAML::Node node = ArbitratorBase::toYaml(time);
        node["type"] = "ConjunctiveCoordinator";
        return node;
    }

protected:
    virtual typename ArbitratorBase::Option::Ptr findBestOption(const Time& time) const override {
        return nullptr;
    }

    bool isActive_{false};
};
} // namespace behavior_planning
