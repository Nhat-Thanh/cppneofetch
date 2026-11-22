#ifndef CPPNEOFETCH_CPACKAGEMANINFO_HPP
#define CPPNEOFETCH_CPACKAGEMANINFO_HPP

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Represents a detected package manager and its installed package count.
 */
typedef struct {
    std::string str_Name;
    uint32_t ui32_Count;
} pkg_manager_t;

/**
 * @brief A class to detect installed package managers and count installed packages.
 */
class CPackageManInfo {
public:
    /**
     * @brief Construct CPackageManInfo and detect installed package managers.
     */
    CPackageManInfo();

    /**
     * @brief Get all detected package managers and their counts.
     *
     * @return Const reference to the vector of detected package managers
     */
    const std::vector<pkg_manager_t>& fkrvec_GetManagers() const noexcept {
        return mvec_Managers;
    }

    /**
     * @brief Get the primary package manager name (or "unknown" if none detected).
     *
     * @return Package manager name string
     */
    std::string fstr_GetName() const;

    /**
     * @brief Get the sum of installed packages across all detected package managers.
     *
     * @return Total installed package count
     */
    uint32_t fui32_CountPkgs() const noexcept;

private:
    std::vector<pkg_manager_t> mvec_Managers;
};

#endif // CPPNEOFETCH_CPACKAGEMANINFO_HPP
