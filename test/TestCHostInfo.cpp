#include "TestCommon.hpp"
#include "CHostInfo.hpp"

#include <iostream>

void fv_RunTestHostInfo() {
    // Reused CHostInfo instance within this independent suite
    const CHostInfo host;

    // 1. Normal case & reference stability
    {
        const std::string& krstr_Host = host.fstr_GetName();
        const std::string& krstr_User = host.fstr_GetUser();

        TEST_CHECK(!krstr_Host.empty());
        TEST_CHECK(!krstr_User.empty());
        TEST_CHECK_EQ(std::string::npos, krstr_Host.find('\n'));
        TEST_CHECK_EQ(std::string::npos, krstr_User.find('\n'));

        TEST_CHECK(&krstr_Host == &host.fstr_GetName());
        TEST_CHECK(&krstr_User == &host.fstr_GetUser());
    }

    // 2. Corner/edge case: unset and empty USER env var still resolves user via getpwuid_r
    {
        CScopedEnvVar envUser("USER");

        envUser.fv_Unset();
        const CHostInfo hostUnsetUser;
        TEST_CHECK_EQ(host.fstr_GetName(), hostUnsetUser.fstr_GetName());
        TEST_CHECK(!hostUnsetUser.fstr_GetUser().empty());

        envUser.fv_Set("");
        const CHostInfo hostEmptyUser;
        TEST_CHECK_EQ(hostUnsetUser.fstr_GetUser(), hostEmptyUser.fstr_GetUser());
    }

    std::cout << "[PASS] TestCHostInfo\n";
}
