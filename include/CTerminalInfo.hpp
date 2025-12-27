#ifndef _CTERMINALINFO_HPP_
#define _CTERMINALINFO_HPP_

#include <string>

/**
 * @brief A class to get running terminal application information
 */
class CTerminalInfo {
public:
    CTerminalInfo();
    CTerminalInfo(const CTerminalInfo&) = default;
    CTerminalInfo& operator=(const CTerminalInfo&) = default;
    CTerminalInfo(CTerminalInfo&&) = default;
    CTerminalInfo& operator=(CTerminalInfo&&) = default;
    virtual ~CTerminalInfo() = default;

    /**
     * @brief Get running termial application name
     *
     * @param None
     *
     * @return A std::string - The terminal application name
     */
    std::string fstr_GetName();

private:
    std::string mstr_Name;
};

#endif
