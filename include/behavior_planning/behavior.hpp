#pragma once

#include <iostream>
#include <memory>
#include <sstream>

#include <yaml-cpp/yaml.h>

#include "types.hpp"


namespace behavior_planning {


/*!
 * \brief The Behavior class is the most abstract representation of a behavior.
 *
 * Derive from this class to implement simple and atomic behaviors. Multiple behaviors can be combined to more complex
 * behaviors using arbitrators, building up a behavior hierarchy, \see arbitrator.hpp
 *
 * The command itself is a template argument, so you can use this behavior planning library to realize chessplay,
 * trading or automated driving.
 *
 * In general, a behavior is not aware if it is active or not, but only if it is applicable in the current situation.
 * In such a way, the complexity of behavior planning is tackled on twofold:
 *  -   The simple, thus manageble behaviors (usually leaf nodes of a behavior hierarchy) carry all the information
 *      and logic needed to decide if such an action is currently applicable.
 *  -   The arbitrators (non-leaf nodes of the behavior hierarchy) combine multiple behaviors together
 *      using a defined policy to select between these subbehaviors.
 *      An arbitrator itself has no particular information about the nature of its subbehaviors,
 *      only about their commitment/invocation conditions and optionally other abstract (quality) measures
 *      (e.g. priority, expected reward, etc.).
 */
template <typename CommandT>
class Behavior {
public:
    using Ptr = std::shared_ptr<Behavior>;
    using ConstPtr = std::shared_ptr<const Behavior>;

    Behavior(const std::string& name = "Behavior") : name_{name} {
    }

    /*!
     * \brief Generates a command that realizes this behavior.
     *
     * \attention   Make sure to only call getCommand(), if either the invocation or the commitment condition is true!
     *              Otherwise the behavior is undefined or an exception may be thrown.
     * \param time  Expected execution time point of this behaviors command
     * \return      A command that can be executed to realize this behavior.
     */
    virtual CommandT getCommand(const Time& time) = 0;

    /*!
     * \brief   true if the behavior can be activated in the current state of the environment model
     *          and would generate reasonable commands
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      true if this behavior can be activated
     */
    virtual bool checkInvocationCondition(const Time& time) const {
        return false;
    }

    /*!
     * \brief true if the behavior would generate reasonable commands in the current state of the environment model.
     *
     * Usually tells a superior behavior/arbitrator that this can be continued.
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      true if this behavior can be continued
     */
    virtual bool checkCommitmentCondition(const Time& time) const {
        return false;
    }

    /*!
     * \brief Informs the behavior that it will become active.
     *
     * Will be called only once before the behavior is active, so should be used only for preparations.
     * If the behavior is currently not executable, the invocation condition should be false.
     * After gainControl(), getCommand() can be called multiple times, as long as the commitment condition is true.
     *
     * If possible, the behavior should not be aware if it is active or not. Neither invocation/commitment conditions,
     * nor the command itself depend on that.
     *
     * \param time  Expected execution time point of this behaviors command
     */
    virtual void gainControl(const Time& time) {
    }

    /*!
     * \brief Informs the behavior that it will become inactive.
     *
     * Will be called only once before the behavior becomes inactive again, so should be used only for cleanups.
     * After loseControl(), getCommand() will not be called without prior gainControl().
     *
     * \param time  Expected execution time point of next behaviors command
     */
    virtual void loseControl(const Time& time) {
    }

    /*!
     * \brief Returns a string representation of the behavior object with its current state using to_stream()
     *
     * \param time      Expected execution time point of this behaviors command
     * \param prefix    A string that should be prepended to each line this function writes
     * \param suffix    A string that should be appended to each line this function writes
     * \return          String representation of this behavior
     *
     * \see to_stream()
     */
    virtual std::string to_str(const Time& time, const std::string& prefix = "", const std::string& suffix = "") const {
        std::stringstream ss;
        to_stream(ss, time, prefix, suffix);
        return ss.str();
    }

    /*!
     * \brief Writes a string representation of the behavior object with its current state to the given output stream.
     *
     * Uses capitalization and ANSI color codes to highlight states (true -> upper-case green, false -> lower-case red)
     *
     * E.g. "INVOCATION commitment my_behavior_object" for a behavior with
     * - name == "my_behavior_object"
     * - checkInvocationCondition() == true
     * - checkCommitmentCondition() == false
     *
     * When overriding this function, make sure to write prefix and suffix only if you print newline characters.
     * This should make sure that the pre-/suffix is really a pre-/suffix of the full line, not somewhere inbetween.
     *
     * \example This function will be called by the output stream \see operator<<()
     *          So the easiest usage is through an output stream, e.g.:
     *          std::cout << my_behavior_object << std::endl;
     *
     * \param output    Output stream to write into, will be returned also
     * \param time      Expected execution time point of this behaviors command
     * \param prefix    A string that should be prepended to each line that is written to the output stream
     * \param suffix    A string that should be appended to each line that is written to the output stream
     * \return          The same given input stream (signature similar to std::ostream& operator<<())
     */
    virtual std::ostream& to_stream(std::ostream& output,
                                    const Time& time,
                                    const std::string& prefix = "",
                                    const std::string& suffix = "") const {
        if (checkInvocationCondition(time)) {
            output << "\033[32mINVOCATION\033[39m ";
        } else {
            output << "\033[31mInvocation\033[39m ";
        }
        if (checkCommitmentCondition(time)) {
            output << "\033[32mCOMMITMENT\033[39m ";
        } else {
            output << "\033[31mCommitment\033[39m ";
        }

        output << name_;
        return output;
    }

    /*!
     * \brief Returns a yaml representation of the behavior object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      Yaml representation of this behavior
     */
    virtual YAML::Node toYaml(const Time& time) const {
        YAML::Node node;
        node["type"] = "Behavior";
        node["name"] = name_;
        node["invocationCondition"] = checkInvocationCondition(time);
        node["commitmentCondition"] = checkCommitmentCondition(time);
        return node;
    }

    const std::string name_;
};
} // namespace behavior_planning
