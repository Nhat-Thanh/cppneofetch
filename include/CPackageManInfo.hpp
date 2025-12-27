#ifndef _CPACKAGEMANINFO_HPP_
#define _CPACKAGEMANINFO_HPP_

#include <string>
#include <cstdint>

/**
 * @brief A class to get package manager information
 */
class CPackageManInfo {
public:
    CPackageManInfo();
    CPackageManInfo(const CPackageManInfo&) = default;
    CPackageManInfo& operator=(const CPackageManInfo&) = default;
    CPackageManInfo(CPackageManInfo&&) = default;
    CPackageManInfo& operator=(CPackageManInfo&&) = default;
    virtual ~CPackageManInfo() = default;


    /**
     * @brief Get package manager name
     *
     * @param None
     *
     * @return A std::string - The package manager name
     */
    std::string fstr_GetName();

    /**
     * @brief Get the count of installed packages
     *
     * @param None
     *
     * @return An uint32_t number - The count of installed packages
     */
    uint32_t fui32_CountPkgs();

private:
    std::string mstr_Name;
    uint32_t (*mpfui32_PkgCounter)();
};

#endif
