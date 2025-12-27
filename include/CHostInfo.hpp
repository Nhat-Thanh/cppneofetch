#ifndef _CHOSTINFO_HPP_
#define _CHOSTINFO_HPP_

#include <string>

/**
 * @brief A class to get host information
 */
class CHostInfo {
public:
    CHostInfo();
    CHostInfo(const CHostInfo&) = default;
    CHostInfo& operator=(const CHostInfo&) = default;
    CHostInfo(CHostInfo&&) = default;
    CHostInfo& operator=(CHostInfo&&) = default;
    virtual ~CHostInfo() = default;

    /**
     * @brief Get the host name
     *
     * @param None
     *
     * @return A std::string - The host name
     */
    std::string fstr_GetName();

    /**
     * @brief Get the user name
     *
     * @param None
     *
     * @return A std::string - The user name
     */
    std::string fstr_GetUser();

private:
    std::string mstr_Name;
    std::string mstr_User;
};

#endif
