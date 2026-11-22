#ifndef CPPNEOFETCH_COSINFO_HPP
#define CPPNEOFETCH_COSINFO_HPP

#include <cstdint>
#include <string>

/**
 * @brief A class to get operating system, kernel, shell, and uptime information.
 */
class COsInfo {
public:
    /**
     * @brief Construct COsInfo and eagerly load OS, kernel, shell, and uptime details.
     */
    COsInfo();

    /**
     * @brief Get the operating system display name (PRETTY_NAME or NAME).
     *
     * @return Const reference to the OS name string
     */
    const std::string& fstr_GetName() const noexcept { return mstr_Name; }

    /**
     * @brief Get the operating system identifier (ID from os-release).
     *
     * @return Const reference to the OS ID string
     */
    const std::string& fstr_GetId() const noexcept { return mstr_Id; }

    /**
     * @brief Get the Linux kernel release string.
     *
     * @return Const reference to the kernel release string
     */
    const std::string& fstr_GetKernel() const noexcept { return mstr_Kernel; }

    /**
     * @brief Get the user's shell name.
     *
     * @return Const reference to the shell name string
     */
    const std::string& fstr_GetShell() const noexcept { return mstr_Shell; }

    /**
     * @brief Get the system uptime in seconds.
     *
     * @return System uptime in seconds
     */
    uint64_t fui64_GetUptime() const noexcept { return mui64_Uptime; }

private:
    /**
     * @brief Read /etc/os-release (or /usr/lib/os-release) for OS name and ID.
     */
    void fv_ReadOsReleaseFs();

    /**
     * @brief Query uname(2) to retrieve the kernel release string.
     */
    void fv_ReadKernel();

    /**
     * @brief Detect the active shell from $SHELL or parent process comm.
     */
    void fv_ReadShellFs();

    /**
     * @brief Query sysinfo(2) to retrieve system uptime in seconds.
     */
    void fv_ReadUptime();

    std::string mstr_Name;
    std::string mstr_Id;
    std::string mstr_Kernel;
    std::string mstr_Shell;
    uint64_t mui64_Uptime{0ULL};
};

#endif // CPPNEOFETCH_COSINFO_HPP
