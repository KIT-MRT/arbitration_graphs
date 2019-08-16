#pragma once

#include <iostream>
#include <memory>
#include <sstream>


namespace behavior_planning {

template <typename CommandT>
class Behavior {
public:
    using Ptr = std::shared_ptr<Behavior>;

    Behavior(const std::string& name = "Behavior") : name_{name} {
    }

    virtual CommandT getCommand() = 0;
    virtual bool checkInvocationCondition() const {
        return false;
    }
    virtual bool checkCommitmentCondition() const {
        return false;
    }
    virtual void gainControl() {
    }
    virtual void loseControl() {
    }

    /*!
     * \brief Returns a string representation of the behavior object with its current state using to_stream()
     *
     * \param prefix    A string that should be prepended to each line this function writes
     * \param suffix    A string that should be appended to each line this function writes
     * \return          String representation of this behavior
     *
     * \see to_stream()
     */
    virtual std::string to_str(const std::string& prefix = "", const std::string& suffix = "") const {
        std::stringstream ss;
        to_stream(ss, prefix, suffix);
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
     * \param prefix    A string that should be prepended to each line that is written to the output stream
     * \param suffix    A string that should be appended to each line that is written to the output stream
     * \return          The same given input stream (signature similar to std::ostream& operator<<())
     */
    virtual std::ostream& to_stream(std::ostream& output,
                                    const std::string& prefix = "",
                                    const std::string& suffix = "") const {
        if (checkInvocationCondition()) {
            output << "\033[32mINVOCATION\033[0m ";
        } else {
            output << "\033[31mInvocation\033[0m ";
        }
        if (checkCommitmentCondition()) {
            output << "\033[32mCOMMITMENT\033[0m ";
        } else {
            output << "\033[31mCommitment\033[0m ";
        }

        output << name_;
        return output;
    }

    const std::string name_;
};


/*!
 * \brief Writes a string representation of a given behavior object with its current state to the given output stream.
 *
 * \example std::cout << my_behavior_object << std::endl;
 *
 * \param output    Output stream to write into, will be returned also
 * \param behavior  This behavior objects state will be written into the output stream
 * \return          The same given input stream
 *
 * \see Behavior::to_stream()
 */
template <typename CommandT>
std::ostream& operator<<(std::ostream& output, const Behavior<CommandT>& behavior) {
    behavior.to_stream(output);
    return output;
}
} // namespace behavior_planning
