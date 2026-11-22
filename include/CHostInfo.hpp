#ifndef CPPNEOFETCH_CHOSTINFO_HPP
#define CPPNEOFETCH_CHOSTINFO_HPP

#include <string>

/**
 * @brief A class to get host name and current user name.
 */
class CHostInfo {
public:
    /**
     * @brief Construct CHostInfo and eagerly resolve host and user names.
     */
    CHostInfo();

    /**
     * @brief Get the system host name.
     *
     * @return Const reference to the host name string
     */
    const std::string& fstr_GetName() const noexcept { return mstr_Name; }

    /**
     * @brief Get the current user name.
     *
     * @return Const reference to the user name string
     */
    const std::string& fstr_GetUser() const noexcept { return mstr_User; }

private:
    std::string mstr_Name;
    std::string mstr_User;
};

#endif // CPPNEOFETCH_CHOSTINFO_HPP
