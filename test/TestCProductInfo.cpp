#include "TestCommon.hpp"
#include "CProductInfo.hpp"

#include <array>
#include <iostream>

void fv_RunTestProductInfo() {
    const CProductInfo product;

    // 1. Exercise all getters on reused CProductInfo instance
    const std::string& krstr_Name    = product.fstr_GetName();
    const std::string& krstr_Family  = product.fstr_GetFamily();
    const std::string& krstr_Sku     = product.fstr_GetSku();
    const std::string& krstr_Vendor  = product.fstr_GetVendor();
    const std::string& krstr_Version = product.fstr_GetVersion();

    // 2. Verify reused references remain identical
    TEST_CHECK(&krstr_Name == &product.fstr_GetName());
    TEST_CHECK(&krstr_Family == &product.fstr_GetFamily());
    TEST_CHECK(&krstr_Sku == &product.fstr_GetSku());
    TEST_CHECK(&krstr_Vendor == &product.fstr_GetVendor());
    TEST_CHECK(&krstr_Version == &product.fstr_GetVersion());

    // 3. Corner/edge checks: no raw newlines/NULs or OEM placeholder strings leaked through
    constexpr std::array<std::string_view, 9> ska_Placeholders = {
        "To Be Filled By O.E.M.",
        "To be filled by O.E.M.",
        "Default string",
        "System Product Name",
        "System Version",
        "None",
        "Not Specified",
        "Not Available",
        "Undefined"
    };

    const std::array<const std::string*, 5> ka_Fields = {
        &krstr_Name, &krstr_Family, &krstr_Sku, &krstr_Vendor, &krstr_Version
    };
    for (const std::string* kp_Field : ka_Fields) {
        TEST_CHECK_EQ(std::string::npos, kp_Field->find('\n'));
        TEST_CHECK_EQ(std::string::npos, kp_Field->find('\r'));
        TEST_CHECK_EQ(std::string::npos, kp_Field->find('\0'));
        for (std::string_view sv_Ph : ska_Placeholders) {
            TEST_CHECK(sv_Ph != *kp_Field);
        }
    }

    std::cout << "[PASS] TestCProductInfo\n";
}
