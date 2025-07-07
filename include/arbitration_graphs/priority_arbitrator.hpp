#pragma once

#include <memory>

#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"


namespace arbitration_graphs {

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT = CommandT>
class PriorityArbitrator : public Arbitrator<EnvironmentModelT, CommandT, SubCommandT> {
public:
    using ArbitratorBase = Arbitrator<EnvironmentModelT, CommandT, SubCommandT>;

    using Ptr = std::shared_ptr<PriorityArbitrator>;
    using ConstPtr = std::shared_ptr<const PriorityArbitrator>;

    using PlaceboVerifierT = verification::PlaceboVerifier<EnvironmentModelT, SubCommandT>;
    using VerifierT = verification::Verifier<EnvironmentModelT, SubCommandT>;

    class Option : public ArbitratorBase::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NoFlags = 0b0, Interruptable = 0b1, Fallback = 0b10 };

        Option(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior, const FlagsT& flags)
                : ArbitratorBase::Option(behavior, flags) {
        }

        /*!
         * \brief Writes a string representation of the behavior option and its current state to the output stream.
         *
         * \param output            Output stream to write into, will be returned also
         * \param time              Expected execution time point of this behaviors command
         * \param environmentModel  A read-only object containing the current state of the environment
         * \param optionIndex       Position index of this option within behaviorOptions_
         * \param prefix            A string that should be prepended to each line that is written to the output stream
         * \param suffix            A string that should be appended to each line that is written to the output stream
         * \return                  The same given input stream (signature similar to std::ostream& operator<<())
         *
         * \see Arbitrator::to_stream()
         */
        std::ostream& to_stream(std::ostream& output,
                                const Time& time,
                                const EnvironmentModelT& environmentModel,
                                const int& optionIndex,
                                const std::string& prefix = "",
                                const std::string& suffix = "") const override;
    };

    explicit PriorityArbitrator(const std::string& name = "PriorityArbitrator",
                                typename VerifierT::Ptr verifier = std::make_shared<PlaceboVerifierT>())
            : ArbitratorBase(name, verifier) {};

    void addOption(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior,
                   const typename Option::FlagsT& flags) override {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \param environmentModel  A read-only object containing the current state of the environment
     * \return      Yaml representation of this behavior
     */
    YAML::Node toYaml(const Time& time, const EnvironmentModelT& environmentModel) const override;

protected:
    /*!
     * @brief   Sort behavior options by priority
     *
     * @return  Behavior options sorted by priority
     */
    typename ArbitratorBase::Options sortOptionsByGivenPolicy(
        const typename ArbitratorBase::Options& options,
        const Time& /*time*/,
        const EnvironmentModelT& /*environmentModel*/) const override {
        // Options are already sorted by priority in behaviorOptions_ and thus in options (which keeps the order)
        return options;
    }
};
} // namespace arbitration_graphs

#include "internal/priority_arbitrator_io.hpp" // IWYU pragma: keep
