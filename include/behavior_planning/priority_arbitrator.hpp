#pragma once

#include <memory>
#include <optional>

#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"


namespace behavior_planning {

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
class PriorityArbitrator : public Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using ArbitratorBase = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    using Ptr = std::shared_ptr<PriorityArbitrator>;
    using ConstPtr = std::shared_ptr<const PriorityArbitrator>;

    struct Option : public ArbitratorBase::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };

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
                                        const std::string& suffix = "") const;
    };

    PriorityArbitrator(const std::string& name = "PriorityArbitrator", const VerifierT& verifier = VerifierT())
            : ArbitratorBase(name, verifier){};

    void addOption(const typename Behavior<SubCommandT>::Ptr& behavior, const typename Option::FlagsT& flags) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      Yaml representation of this behavior
     */
    virtual YAML::Node toYaml(const Time& time) const override;

protected:
    /*!
     * @brief   Sort behavior options by priority
     *
     * @return  Behavior options sorted by priority
     */
    typename ArbitratorBase::Options sortOptionsByGivenPolicy(const typename ArbitratorBase::Options& options,
                                                              const Time& time) const override {
        // Options are already sorted by priority in behaviorOptions_ and thus in options (which keeps the order)
        return options;
    }
};
} // namespace behavior_planning

#include "internal/priority_arbitrator_io.hpp"