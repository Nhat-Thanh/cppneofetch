#include "CProductInfo.hpp"
#include "helpers/FileReader.hpp"
#include "helpers/StringHandler.hpp"

#include <array>

namespace {

constexpr const char* kpc_DmiProductName      = "/sys/devices/virtual/dmi/id/product_name";
constexpr const char* kpc_DmiProductFamily    = "/sys/devices/virtual/dmi/id/product_family";
constexpr const char* kpc_DmiProductSku       = "/sys/devices/virtual/dmi/id/product_sku";
constexpr const char* kpc_DmiSysVendor        = "/sys/devices/virtual/dmi/id/sys_vendor";
constexpr const char* kpc_DmiProductVersion   = "/sys/devices/virtual/dmi/id/product_version";
constexpr const char* kpc_DmiBoardName        = "/sys/devices/virtual/dmi/id/board_name";
constexpr const char* kpc_DmiBoardVendor      = "/sys/devices/virtual/dmi/id/board_vendor";
constexpr const char* kpc_PathDeviceTreeModel = "/proc/device-tree/model";

constexpr std::string_view ksv_TrimWhitespaceAndNul(" \t\r\n\0", 5U);

constexpr std::array<std::string_view, 9> ska_OemPlaceholders = {
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

std::string sfstr_ReadDmiField(const char* kpc_Path) {
    std::string str_Val;
    if (!FileReader::fb_ReadFirstLine(str_Val, kpc_Path)) {
        return {};
    }
    StringHandler::fv_Trim(str_Val);
    for (std::string_view sv_Placeholder : ska_OemPlaceholders) {
        if (sv_Placeholder == str_Val) {
            return {};
        }
    }
    return str_Val;
}

} // namespace

/**
 * @brief Construct CProductInfo and read DMI / device-tree hardware details.
 */
CProductInfo::CProductInfo() {
    mstr_Name    = sfstr_ReadDmiField(kpc_DmiProductName);
    mstr_Family  = sfstr_ReadDmiField(kpc_DmiProductFamily);
    mstr_Sku     = sfstr_ReadDmiField(kpc_DmiProductSku);
    mstr_Vendor  = sfstr_ReadDmiField(kpc_DmiSysVendor);
    mstr_Version = sfstr_ReadDmiField(kpc_DmiProductVersion);

    if (mstr_Name.empty()) {
        mstr_Name = sfstr_ReadDmiField(kpc_DmiBoardName);
    }
    if (mstr_Name.empty()) {
        std::string str_DtModel;
        if (FileReader::fb_ReadFirstLine(str_DtModel, kpc_PathDeviceTreeModel)) {
            StringHandler::fv_Trim(str_DtModel, ksv_TrimWhitespaceAndNul);
            mstr_Name = std::move(str_DtModel);
        }
    }
    if (mstr_Vendor.empty()) {
        mstr_Vendor = sfstr_ReadDmiField(kpc_DmiBoardVendor);
    }
}
