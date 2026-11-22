#include "TestCommon.hpp"
#include "CGpuInfo.hpp"
#include "PciIds.hpp"

#include <iostream>

void fv_RunTestGpuInfo() {
    // 1. Normal case
    {
        const CGpuInfo gpu;
        const auto& krvec_Devices = gpu.fkrvec_GetDevices();
        for (std::size_t ui64_Idx = 0U; ui64_Idx < krvec_Devices.size(); ++ui64_Idx) {
            const gpu_device_t& kr_Dev = krvec_Devices[ui64_Idx];
            TEST_CHECK(!kr_Dev.str_BusAddress.empty());
            TEST_CHECK(std::string::npos != kr_Dev.str_BusAddress.find(':'));
            TEST_CHECK(!kr_Dev.str_Name.empty());
            if (kr_Dev.optui32_MaxFreqMhz) {
                TEST_CHECK(0U < *kr_Dev.optui32_MaxFreqMhz);
            }
            if (0U < ui64_Idx) {
                TEST_CHECK(kr_Dev.str_BusAddress >= krvec_Devices[ui64_Idx - 1U].str_BusAddress);
            }
        }
    }

    // 2. Generated PciIds binary search tests (normal, corner, boundary/miss cases)
    {
        std::string_view sv_Vendor;
        std::string_view sv_Device;

        TEST_CHECK(fb_LookupPciDevice(0x10de, 0x1e04, sv_Vendor, sv_Device));
        TEST_CHECK_EQ("NVIDIA", sv_Vendor);
        TEST_CHECK(!sv_Device.empty());

        TEST_CHECK(fb_LookupPciDevice(0x8086, 0x3e92, sv_Vendor, sv_Device));
        TEST_CHECK_EQ("Intel", sv_Vendor);

        TEST_CHECK(fb_LookupPciDevice(0x1002, 0x1114, sv_Vendor, sv_Device));
        TEST_CHECK_EQ("AMD/ATI", sv_Vendor);

        // Known vendor with nonexistent device ID
        TEST_CHECK(!fb_LookupPciDevice(0x8086, 0xFFFF, sv_Vendor, sv_Device));

        // Boundary and nonexistent vendor IDs
        TEST_CHECK(!fb_LookupPciDevice(0x0000, 0x0000, sv_Vendor, sv_Device));
        TEST_CHECK(!fb_LookupPciDevice(0xFFFF, 0xFFFF, sv_Vendor, sv_Device));
    }

    std::cout << "[PASS] TestCGpuInfo\n";
}
