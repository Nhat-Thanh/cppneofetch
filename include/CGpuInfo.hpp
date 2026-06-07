#ifndef _CGPUINFO_HPP_
#define _CGPUINFO_HPP_
#include <string>
#include <vector>
#include <stdint.h>

/**
 * @brief A utility class that manages and provides access to GPU information.
 */
class CGpuInfo
{
private:
    std::vector<std::string> mvecstr_GpuNames;
    std::vector<std::string> mvecstr_PciBusAddresses;
    std::vector<uint32_t> mvecui32_MaxGpuFreqMhz;

    void fv_FindPciBusAddress();

    bool fb_GetDecimalValue(const std::string& kstr_Path, uint32_t& rui32_Out);

    /**
     * Reads a PCI device ID or Vendor ID from hex file.
     * 
     * @param path Path to the hex value file (e.g., "/sys/bus/pci/devices/xxx/vendor")
     * 
     * @return true if read successful, false otherwise
     */
    bool fb_GetHexValue(const std::string& kstr_Path, uint32_t& rui32_Out);

  public:
    CGpuInfo();
    CGpuInfo(const CGpuInfo&) = default;
    CGpuInfo& operator=(const CGpuInfo&) = default;
    CGpuInfo(CGpuInfo&&) = default;
    CGpuInfo& operator=(CGpuInfo&&) = default;
    virtual ~CGpuInfo() = default;

    /**
     * @brief Retrieves a list containing all detected GPU names from the system.
     * 
     * @param None
     * 
     * @return A std::vector<std::string> - contains strings representing each GPU's name, returns empty vector if no GPUs found
     */
    const std::vector<std::string>& fvecstr_GetNames();

    /**
     * @brief Retrieves the maximum integrated GPU clock speed from stored data.
     * 
     * @param None
     * 
     * @return A float - represents the maximum integrated GPU clock frequency in MHz, returns zero if no valid data available
     */
    const std::vector<uint32_t>& fvecui32_GetMaxGpuFreqMhz();
};

#endif
