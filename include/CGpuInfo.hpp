#ifndef CPPNEOFETCH_CGPUINFO_HPP
#define CPPNEOFETCH_CGPUINFO_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents a detected PCI GPU/display controller and its properties.
 */
typedef struct {
    std::string str_BusAddress;
    std::string str_Name;
    std::optional<uint32_t> optui32_MaxFreqMhz;
} gpu_device_t;

/**
 * @brief A class that detects PCI display controllers and provides GPU information.
 */
class CGpuInfo {
public:
    /**
     * @brief Construct CGpuInfo and scan sysfs PCI devices for display controllers.
     */
    CGpuInfo();

    /**
     * @brief Get all detected GPU devices sorted by PCI bus address.
     *
     * @return Const reference to the vector of detected gpu_device_t records
     */
    const std::vector<gpu_device_t>& fkrvec_GetDevices() const noexcept {
        return mvec_Devices;
    }

private:
    /**
     * @brief Scan /sys/bus/pci/devices for display controllers and resolve names/clocks.
     */
    void fv_DetectDevices();

    std::vector<gpu_device_t> mvec_Devices;
};

#endif // CPPNEOFETCH_CGPUINFO_HPP
