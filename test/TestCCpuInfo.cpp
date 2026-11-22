#include "TestCommon.hpp"
#include "CCpuInfo.hpp"

#include <iostream>

void fv_RunTestCpuInfo() {
    // 1. Normal case
    {
        const CCpuInfo cpu;
        TEST_CHECK_EQ(0U, cpu.fui32_GetCpuId());
        TEST_CHECK(!cpu.fstr_GetName().empty());
        TEST_CHECK_EQ(std::string::npos, cpu.fstr_GetName().find("(R)"));
        TEST_CHECK_EQ(std::string::npos, cpu.fstr_GetName().find("(TM)"));
        TEST_CHECK_EQ(std::string::npos, cpu.fstr_GetName().find('@'));

        TEST_CHECK(0U < cpu.fui32_GetLogicalCores());
        TEST_CHECK(0U < cpu.fui32_GetPhysicalCores());
        TEST_CHECK(cpu.fui32_GetLogicalCores() >= cpu.fui32_GetPhysicalCores());

        if (cpu.foptui64_GetMaxFreqKhz()) {
            TEST_CHECK(0ULL < *cpu.foptui64_GetMaxFreqKhz());
        }
        if (cpu.foptui64_GetMinFreqKhz()) {
            TEST_CHECK(0ULL < *cpu.foptui64_GetMinFreqKhz());
        }
        if (cpu.foptui64_GetMaxFreqKhz() && cpu.foptui64_GetMinFreqKhz()) {
            TEST_CHECK(*cpu.foptui64_GetMaxFreqKhz() >= *cpu.foptui64_GetMinFreqKhz());
        }
        if (cpu.foptui32_GetCacheKb()) {
            TEST_CHECK(0U < *cpu.foptui32_GetCacheKb());
        }
    }

    // 2. Corner/edge case: nonexistent physical CPU package ID must not crash
    {
        const CCpuInfo cpuNonexistent(9999U);
        TEST_CHECK_EQ(9999U, cpuNonexistent.fui32_GetCpuId());
        TEST_CHECK(cpuNonexistent.fstr_GetName().empty());
        TEST_CHECK_EQ(0U, cpuNonexistent.fui32_GetLogicalCores());
        TEST_CHECK_EQ(0U, cpuNonexistent.fui32_GetPhysicalCores());
        TEST_CHECK_EQ(0U, *cpuNonexistent.foptui32_GetCacheKb());
        TEST_CHECK_EQ(0UL, *cpuNonexistent.foptui64_GetMinFreqKhz());
        TEST_CHECK_EQ(0UL, *cpuNonexistent.foptui64_GetMaxFreqKhz());
    }

    std::cout << "[PASS] TestCCpuInfo\n";
}
