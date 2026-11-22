#include "TestCommon.hpp"
#include "CPackageManInfo.hpp"

#include <iostream>

namespace {

uint32_t sfui32_FindManagerCount(const CPackageManInfo& kr_Pkg, const std::string& krstr_Name) {
    for (const pkg_manager_t& kr_Entry : kr_Pkg.fkrvec_GetManagers()) {
        if (krstr_Name == kr_Entry.str_Name) {
            return kr_Entry.ui32_Count;
        }
    }
    return 0U;
}

} // namespace

void fv_RunTestPackageManInfo() {
    // 1. Normal case
    {
        const CPackageManInfo pkgMan;
        const auto& krvec_Managers = pkgMan.fkrvec_GetManagers();
        uint32_t ui32_Sum = 0U;
        for (const pkg_manager_t& kr_Entry : krvec_Managers) {
            TEST_CHECK(!kr_Entry.str_Name.empty());
            TEST_CHECK(0U < kr_Entry.ui32_Count);
            ui32_Sum += kr_Entry.ui32_Count;
        }
        TEST_CHECK_EQ(ui32_Sum, pkgMan.fui32_CountPkgs());

        if (krvec_Managers.empty()) {
            TEST_CHECK_EQ("unknown", pkgMan.fstr_GetName());
        } else {
            TEST_CHECK_EQ(krvec_Managers.front().str_Name, pkgMan.fstr_GetName());
        }
    }

    // 2. Corner/edge case: Flatpak user directory counting & regular file exclusion
    {
        CScopedEnvVar envHome("HOME");
        CScopedTempDir tempHome("pkg_flatpak_");

        // Baseline with empty HOME (no user flatpaks)
        envHome.fv_Set(tempHome.fkr_Path().c_str());
        const CPackageManInfo pkgBaseline;
        const uint32_t ui32_BaseFlatpak = sfui32_FindManagerCount(pkgBaseline, "flatpak");

        // Create 2 app subdirectories + 1 regular file (must be ignored by countSubdirs)
        std::filesystem::create_directories(tempHome.fkr_Path() / ".local/share/flatpak/app/org.test.AppOne");
        std::filesystem::create_directories(tempHome.fkr_Path() / ".local/share/flatpak/app/org.test.AppTwo");
        tempHome.fstr_WriteFile(".local/share/flatpak/app/not_an_app.txt", "regular file");

        const CPackageManInfo pkgWithFlatpak;
        const uint32_t ui32_NewFlatpak = sfui32_FindManagerCount(pkgWithFlatpak, "flatpak");
        TEST_CHECK_EQ(ui32_BaseFlatpak + 2U, ui32_NewFlatpak);

        // Edge case: unset HOME must not crash
        envHome.fv_Unset();
        const CPackageManInfo pkgNoHome;
        TEST_CHECK_EQ(ui32_BaseFlatpak, sfui32_FindManagerCount(pkgNoHome, "flatpak"));
    }

    std::cout << "[PASS] TestCPackageManInfo\n";
}
