#ifndef _CPRODUCTINFO_HPP_
#define _CPRODUCTINFO_HPP_

#include <string>

/**
 * @brief A class to get product (model) information
 */
class CProductInfo {
public:
    CProductInfo();
    CProductInfo(const CProductInfo &) = default;
    CProductInfo& operator=(const CProductInfo&) = default;
    CProductInfo(CProductInfo &&) = default;
    CProductInfo & operator=(CProductInfo&&) = default;
    virtual ~CProductInfo() = default;


    /**
     * @brief Get product name
     *
     * @param None
     *
     * @return A std::string - The product name
     */
    std::string fstr_GetName();

    /**
     * @brief Get product family name
     *
     * @param None
     *
     * @return A std::string - The product family name
     */
    std::string fstr_GetFamily();

    /**
     * @brief Get product SKU
     *
     * @param None
     *
     * @return A std::string - The product SKU
     */
    std::string fstr_GetSku();

    /**
     * @brief Get product vendor name
     *
     * @param None
     *
     * @return A std::string - The product vendor name
     */
    std::string fstr_GetVendor();

    /**
     * @brief Get product version name
     *
     * @param None
     *
     * @return A std::string - The product version name
     */
    std::string fstr_GetVersion();

    #ifdef SYSTEM_INSTALL

    /**
     * @brief Get product serial number
     *
     * @param None
     *
     * @return A std::string - The product serial number
     */
    std::string fstr_GetSerial();

    /**
     * @brief Get product uuid number
     *
     * @param None
     *
     * @return A std::string - The product uuid number
     */
    std::string fstr_GetUuid();
    #endif // SYSTEM_INSTALL

private:
    std::string mstr_Name;
    std::string mstr_Family;
    std::string mstr_Sku;
    std::string mstr_Vendor;
    std::string mstr_Version;
    #ifdef SYSTEM_INSTALL
    std::string mstr_Serial;
    std::string mstr_Uuid;
    #endif // SYSTEM_INSTALL

};

#endif
