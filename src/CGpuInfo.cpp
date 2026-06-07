#include "PciIds.hpp"
#include "CStringHelper.hpp"
#include "CFileReader.hpp"
#include "Definitions.hpp"
#include "CGpuInfo.hpp"
#include <dirent.h>     // opendir(), readdir() - Linux specific
#include <fcntl.h>      // R_OK for access() check  
#include <unistd.h>     // access()
#include <cstdio>       // FILE
#include <cstdlib>      // strtol()
#include <algorithm>
#include <iostream>

CGpuInfo::CGpuInfo():
    mvecstr_GpuNames({}),
    mvecstr_PciBusAddresses({}),
    mvecui32_MaxGpuFreqMhz({})
{
}

void CGpuInfo::fv_FindPciBusAddress() {
    if (!mvecstr_PciBusAddresses.empty()) {
        return;
    }
    DIR* dirp = opendir(PATH_SYS_PCI_DEV_BUS);
    if (dirp != nullptr) {
        struct dirent* entry;
        uint32_t ui32_Class;
        std::string str_FilePath;
        while ((entry = readdir(dirp)) != nullptr) {
            // Directory names like "0000:01:00.0" represent PCI devices
            std::string devName(entry->d_name);
            if (devName.find(':') == std::string::npos) {
                continue;     // Skip non-PCI entries
            }
            CStringHelper::fv_Format(str_FilePath, "%s/%s/class", PATH_SYS_PCI_DEV_BUS, devName.c_str());
            if (!fb_GetHexValue(str_FilePath, ui32_Class)) {
                continue;
            }
            if ((ui32_Class >> 16) != PCI_CLASS_ID_DISPLAY_CTRL) {
                continue;
            }
            mvecstr_PciBusAddresses.push_back(devName);
        }
    }
    closedir(dirp);
}

bool CGpuInfo::fb_GetDecimalValue(const std::string& kstr_Path, uint32_t& rui32_Out) {
    std::string str_Line;
    if (EXIT_SUCCESS != CFileReader::fi64_GetLine_nth(str_Line, kstr_Path)) {
        return false;
    }
    rui32_Out = static_cast<uint32_t>(strtoul(str_Line.c_str(), nullptr, 10));
    return true;
}

bool CGpuInfo::fb_GetHexValue(const std::string& kstr_Path, uint32_t& rui32_Out) {
    std::string str_Line;
    if (EXIT_SUCCESS != CFileReader::fi64_GetLine_nth(str_Line, kstr_Path)) {
        return false;
    }

    // if (0 != access(kstr_Path.c_str(), R_OK)) {
    //     return false;
    // }
    // FILE* fp = fopen(kstr_Path.c_str(), "r");
    // if (fp) {
    //     const int ki32_HexSize = 10;
    //     char ac_Buffer[ki32_HexSize + 1];
    //     int i32_ReadSize = fread(ac_Buffer, 1, ki32_HexSize, fp);
    //     fclose(fp);
    //     ac_Buffer[i32_ReadSize] = '\0';
    //     rui32_Out = static_cast<uint32_t>(strtol(ac_Buffer, nullptr, 16));
    // }
    rui32_Out = static_cast<uint32_t>(strtoul(str_Line.c_str(), nullptr, 16));
    return true;
}

const std::vector<std::string>& CGpuInfo::fvecstr_GetNames() {
    if (mvecstr_GpuNames.empty()) {
        fv_FindPciBusAddress();
        std::vector<uint16_t> vec16_VendorIds;
        std::vector<uint16_t> vec16_DeviceIds;
        std::string str_FilePath;
        uint32_t ui32_VendorId, ui32_DeviceId;
        for (const std::string& krstr_BusAddress : mvecstr_PciBusAddresses) {
            CStringHelper::fv_Format(str_FilePath, "%s/%s/vendor", PATH_SYS_PCI_DEV_BUS, krstr_BusAddress.c_str());
            if (!fb_GetHexValue(str_FilePath, ui32_VendorId)) {
                continue;
            }
            CStringHelper::fv_Format(str_FilePath, "%s/%s/device", PATH_SYS_PCI_DEV_BUS, krstr_BusAddress.c_str());
            if (!fb_GetHexValue(str_FilePath, ui32_DeviceId)) {
                continue;
            }
            vec16_VendorIds.push_back(static_cast<uint16_t>(ui32_VendorId));
            vec16_DeviceIds.push_back(static_cast<uint16_t>(ui32_DeviceId));
        }
        for (size_t ui64_Idx = 0; ui64_Idx < vec16_VendorIds.size(); ++ui64_Idx) {
            const auto iter_PciVendor = kMap_PciVendors.find(vec16_VendorIds[ui64_Idx]);
            if (kMap_PciVendors.cend() == iter_PciVendor) {
                continue;
            }

            const auto* pMap_PciDevs = iter_PciVendor->second.kpMap_PciDevs;
            const auto iter_PciDev = pMap_PciDevs->find(vec16_DeviceIds[ui64_Idx]);
            if (pMap_PciDevs->cend() == iter_PciDev) {
                continue;
            }

            const pci_vendor_t& kr_PciVendor = iter_PciVendor->second;
            const std::string kstr_PciDevName = iter_PciDev->second;
            std::string str_GpuName;
            std::string str_ShortenName = CStringHelper::fstr_GetSubstringBetween(kstr_PciDevName, '[', ']');
            if (!str_ShortenName.empty()) {
                CStringHelper::fv_Format(str_GpuName, "%s %s", kr_PciVendor.kpc_Name, str_ShortenName.c_str());
            } else {
                CStringHelper::fv_Format(str_GpuName, "%s %s", kr_PciVendor.kpc_Name, kstr_PciDevName.c_str());
            }

            mvecstr_GpuNames.push_back(str_GpuName);
        }
    }

    return mvecstr_GpuNames;
}

const std::vector<uint32_t>& CGpuInfo::fvecui32_GetMaxGpuFreqMhz() {
    if (mvecui32_MaxGpuFreqMhz.empty()) {
        fv_FindPciBusAddress();
        const uint32_t ui32_GpuCount = mvecstr_PciBusAddresses.size();
        mvecui32_MaxGpuFreqMhz.assign(ui32_GpuCount, UINT32_INIT);
        std::string str_FilePath;
        uint32_t ui32_MaxFreqMhz;
        for (uint32_t ui32_Idx = 0; ui32_Idx < ui32_GpuCount; ++ui32_Idx) {
            ui32_MaxFreqMhz = UINT32_INIT;
            for (uint32_t ui32_CardNo = 0; ui32_CardNo < ui32_GpuCount; ++ui32_CardNo) {
                CStringHelper::fv_Format(str_FilePath,
                                        "%s/%s/drm/card%d/gt_max_freq_mhz",
                                        PATH_SYS_PCI_DEV_BUS,
                                        mvecstr_PciBusAddresses[ui32_Idx].c_str(),
                                        ui32_CardNo);
                if (fb_GetDecimalValue(str_FilePath, ui32_MaxFreqMhz)) {
                    std::cout << "GPU " << ui32_Idx + 1 << ": Max freq = " << ui32_MaxFreqMhz << " MHz" << std::endl;
                    mvecui32_MaxGpuFreqMhz[ui32_Idx] = ui32_MaxFreqMhz;
                    break;
                }
            }
            if (UINT32_INIT != ui32_MaxFreqMhz) {
                break;
            }
        }
    }
    return mvecui32_MaxGpuFreqMhz;
}
