#pragma once

#include <memory>
#include <random>

#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"

namespace arbitration_graphs {

template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
class RandomArbitrator : public Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using ArbitratorBase = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    using Ptr = std::shared_ptr<RandomArbitrator>;
    using ConstPtr = std::shared_ptr<const RandomArbitrator>;

    struct Option : public ArbitratorBase::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1, FALLBACK = 0b10 };

        Option(const typename Behavior<SubCommandT>::Ptr& behavior, const FlagsT& flags, const double& weight)
                : ArbitratorBase::Option(behavior, flags), weight_(weight) {
        }

        /*!
         * \brief Writes a string representation of the behavior option and its
         * current state to the output stream.
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

        //! The option has a chance of weight_ divided by the sum of all options' weights to be selected.
        double weight_;
    };
    using Options = std::vector<typename Option::Ptr>;

    RandomArbitrator(const std::string& name = "RandomArbitrator", const VerifierT& verifier = VerifierT())
            : ArbitratorBase(name, verifier){};

    void addOption(const typename Behavior<SubCommandT>::Ptr& behavior,
                   const typename Option::FlagsT& flags,
                   const double& weight = 1) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags, weight);
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
     * \brief   Sort behavior options randomly considering their respective weights
     *
     * \return  Behavior options sorted randomly considering their respective weights
     */
    typename ArbitratorBase::Options sortOptionsByGivenPolicy(const typename ArbitratorBase::Options& options,
                                                              const Time& time) const override {
        typename ArbitratorBase::Options shuffledOptions;
        shuffledOptions.reserve(options.size());

        std::vector<double> weights;
        weights.reserve(options.size());
        for (const auto& optionBase : options) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(optionBase);
            weights.push_back(option->weight_);
        }

        std::random_device randomDevice;
        std::mt19937 randomGenerator(randomDevice());

        // A weighted shuffle is equivalent to a weighted sampling of options without replacement.
        while (!weights.empty()) {
            std::discrete_distribution<int> discreteDistribution(weights.begin(), weights.end());
            auto index = discreteDistribution(randomGenerator);

            shuffledOptions.push_back(options.at(index));
            weights.erase(weights.begin() + index);
        }

        return shuffledOptions;
    }

}; // namespace arbitration_graphs
} // namespace arbitration_graphs

#include "internal/random_arbitrator_io.hpp"
