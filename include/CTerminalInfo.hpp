#ifndef CPPNEOFETCH_CTERMINALINFO_HPP
#define CPPNEOFETCH_CTERMINALINFO_HPP

#include <string>

/**
 * @brief A class to detect the running terminal emulator or TTY name.
 */
class CTerminalInfo {
public:
    /**
     * @brief Construct CTerminalInfo and detect the active terminal application.
     */
    CTerminalInfo();

    /**
     * @brief Get the detected terminal application or TTY name.
     *
     * @return Const reference to the terminal name string
     */
    const std::string& fstr_GetName() const noexcept { return mstr_Name; }

private:
    std::string mstr_Name;
};

#endif // CPPNEOFETCH_CTERMINALINFO_HPP
