#ifndef CPPNEOFETCH_CPRODUCTINFO_HPP
#define CPPNEOFETCH_CPRODUCTINFO_HPP

#include <string>

/**
 * @brief A class to get hardware product (model) and vendor information.
 */
class CProductInfo {
public:
    /**
     * @brief Construct CProductInfo and read DMI / device-tree hardware details.
     */
    CProductInfo();

    /**
     * @brief Get the hardware product or board model name.
     *
     * @return Const reference to the product name string
     */
    const std::string& fstr_GetName() const noexcept { return mstr_Name; }

    /**
     * @brief Get the hardware product family.
     *
     * @return Const reference to the product family string
     */
    const std::string& fstr_GetFamily() const noexcept { return mstr_Family; }

    /**
     * @brief Get the hardware product SKU.
     *
     * @return Const reference to the product SKU string
     */
    const std::string& fstr_GetSku() const noexcept { return mstr_Sku; }

    /**
     * @brief Get the hardware system or board vendor name.
     *
     * @return Const reference to the vendor name string
     */
    const std::string& fstr_GetVendor() const noexcept { return mstr_Vendor; }

    /**
     * @brief Get the hardware product version string.
     *
     * @return Const reference to the product version string
     */
    const std::string& fstr_GetVersion() const noexcept { return mstr_Version; }

private:
    std::string mstr_Name;
    std::string mstr_Family;
    std::string mstr_Sku;
    std::string mstr_Vendor;
    std::string mstr_Version;
};

#endif // CPPNEOFETCH_CPRODUCTINFO_HPP
