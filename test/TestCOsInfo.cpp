#include "TestCommon.hpp"
#include "COsInfo.hpp"

#include <iostream>

void fv_RunTestOsInfo() {
    // Reused COsInfo instance within this independent suite
    const COsInfo os;

    // 1. Normal case & reference stability
    {
        TEST_CHECK(!os.fstr_GetName().empty());
        TEST_CHECK(!os.fstr_GetKernel().empty());
        TEST_CHECK_EQ(std::string::npos, os.fstr_GetName().find('"'));
        TEST_CHECK_EQ(std::string::npos, os.fstr_GetKernel().find('\n'));
        (void)os.fui64_GetUptime();

        TEST_CHECK(&os.fstr_GetName() == &os.fstr_GetName());
        TEST_CHECK(&os.fstr_GetId() == &os.fstr_GetId());
        TEST_CHECK(&os.fstr_GetKernel() == &os.fstr_GetKernel());
        TEST_CHECK(&os.fstr_GetShell() == &os.fstr_GetShell());
    }

    // 2. Corner/edge cases for SHELL resolution (restored automatically by CScopedEnvVar)
    {
        CScopedEnvVar envShell("SHELL");

        envShell.fv_Set("/usr/bin/zsh");
        const COsInfo osZsh;
        TEST_CHECK_EQ("zsh", osZsh.fstr_GetShell());

        envShell.fv_Set("fish");
        const COsInfo osBare;
        TEST_CHECK_EQ("fish", osBare.fstr_GetShell());

        // Trailing slash produces empty basename -> falls back to /proc/<ppid>/comm
        envShell.fv_Set("/usr/bin/");
        const COsInfo osTrailingSlash;
        TEST_CHECK(!osTrailingSlash.fstr_GetShell().empty());

        envShell.fv_Unset();
        const COsInfo osUnset;
        TEST_CHECK(!osUnset.fstr_GetShell().empty());
    }

    std::cout << "[PASS] TestCOsInfo\n";
}
