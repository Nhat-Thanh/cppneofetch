#ifndef _COSINFO_HPP_
#define _COSINFO_HPP_

#include <string>
#include <cstdint>

/**
 * @brief A class to get operating system information
 */
class COsInfo {
public:
    COsInfo();
    COsInfo(const COsInfo&) = default;
    COsInfo& operator=(const COsInfo&) = default;
    COsInfo(COsInfo&&) = default;
    COsInfo& operator=(COsInfo&&) = default;
    virtual ~COsInfo() = default;


    /**
     * @brief Get the OS name
     *
     * @param None
     *
     * @return A std::string - OS name
     */
    std::string fstr_GetName() const;

    /**
     * @brief Get kernel version of OS
     *
     * @param None
     *
     * @return A std::string - kernel version
     */
    std::string fstr_GetKernel() const;

    /**
     * @brief Get name of running shell (The application runs this program)
     *
     * @param None
     *
     * @return A std::string - shell name
     */
    std::string fstr_GetShell() const;

    /**
     * @brief Get uptime of system in seconds
     *
     * @param None
     *
     * @return An uint64_t number - uptime in seconds
     */
    uint64_t fui64_GetUptime() const;

private:
    /**
     * @brief Read os-release filesystem and get OS name
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadOsReleaseFs() const;

    /**
     * @brief Read /proc filesystem to get name of running shell process
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadShellFs() const;

    mutable std::string mstr_Name;
    mutable std::string mstr_Kernel;
    mutable std::string mstr_Shell;
    mutable uint64_t mui64_Uptime;
};

#endif
