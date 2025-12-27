#include "CProductInfo.hpp"
#include "CFileReader.hpp"

#define PATH_PRODUCT_FAMILY_VFS  "/sys/devices/virtual/dmi/id/product_family"
#define PATH_PRODUCT_NAME_VFS    "/sys/devices/virtual/dmi/id/product_name"
#define PATH_PRODUCT_SERIAL_VFS  "/sys/devices/virtual/dmi/id/product_serial"
#define PATH_PRODUCT_SKU_VFS     "/sys/devices/virtual/dmi/id/product_sku"
#define PATH_PRODUCT_UUID_VFS    "/sys/devices/virtual/dmi/id/product_uuid"
#define PATH_PRODUCT_VERSION_VFS "/sys/devices/virtual/dmi/id/product_version"
#define PATH_PRODUCT_VENDOR_VFS  "/sys/devices/virtual/dmi/id/sys_vendor"


CProductInfo::CProductInfo() {
}

std::string CProductInfo::fstr_GetName() {
    if (mstr_Name.empty()) {
        CFileReader::fi64_GetLine_nth(mstr_Name, PATH_PRODUCT_NAME_VFS);
    }
    return mstr_Name;
}

std::string CProductInfo::fstr_GetFamily() {
    if (mstr_Family.empty()) {
        CFileReader::fi64_GetLine_nth(mstr_Family, PATH_PRODUCT_FAMILY_VFS);
    }
    return mstr_Family;
}

std::string CProductInfo::fstr_GetSku() {
    if (mstr_Sku.empty()) {
        CFileReader::fi64_GetLine_nth(mstr_Sku, PATH_PRODUCT_SKU_VFS);
    }
    return mstr_Sku;
}

std::string CProductInfo::fstr_GetVendor() {
    if (mstr_Vendor.empty()) {
        CFileReader::fi64_GetLine_nth(mstr_Vendor, PATH_PRODUCT_VENDOR_VFS);
    }
    return mstr_Vendor;
}

std::string CProductInfo::fstr_GetVersion() {
    if (mstr_Version.empty()) {
        CFileReader::fi64_GetLine_nth(mstr_Version, PATH_PRODUCT_VERSION_VFS);
    }
    return mstr_Version;
}


#ifdef SYSTEM_INSTALL
std::string CProductInfo::fstr_GetSerial() {
    if (mstr_Serial.empty()) {
        CFileReader::fi64_GetLine_nth(mstr_Serial, PATH_PRODUCT_SERIAL_VFS);
    }
    return mstr_Serial;
}

std::string CProductInfo::fstr_GetUuid() {
    if (mstr_Uuid.empty()) {
        CFileReader::fi64_GetLine_nth(mstr_Uuid, PATH_PRODUCT_UUID_VFS);
    }
    return mstr_Uuid;
}
#endif // SYSTEM_INSTALL