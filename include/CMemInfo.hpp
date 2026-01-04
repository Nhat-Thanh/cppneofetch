#ifndef _CMEMINFO_HPP_
#define _CMEMINFO_HPP_

#include <string>
#include <cstdint>

/**
 * @brief A class to get current memory information
 */
class CMemInfo {
public:
    CMemInfo();
    CMemInfo(const CMemInfo &) = default;
    CMemInfo& operator=(const CMemInfo&) = default;
    CMemInfo(CMemInfo &&) = default;
    CMemInfo & operator=(CMemInfo&&) = default;
    virtual ~CMemInfo() = default;

    /**
     * @brief Get total memory
     *
     * @param None
     *
     * @return An uint32_t number - The total memory
     */
    uint32_t fui32_GetTotal() const;

    /**
     * @brief Get current available memory
     *
     * @param None
     *
     * @return An uint32_t number - The current available memory
     */
    uint32_t fui32_GetAvailable() const;

    /**
     * @brief Get current free memory
     *
     * @param None
     *
     * @return An uint32_t number - The current free memory
     */
    uint32_t fui32_GetFree() const;

    /**
     * @brief Get current used memory
     *
     * @param None
     *
     * @return An uint32_t number - The used memory
     */
    uint32_t fui32_GetUsed() const;

    /**
     * @brief Get shared memory size
     *
     * @param None
     *
     * @return An uint32_t number - The shared memory size
     */
    uint32_t fui32_GetShmem() const;

    /**
     * @brief Get buffer memory
     *
     * @param None
     *
     * @return An uint32_t number - The buffer size
     */
    uint32_t fui32_GetBuffers() const;

    /**
     * @brief Get cache size
     *
     * @param None
     *
     * @return An uint32_t number - The cache size
     */
    uint32_t fui32_GetCached() const;

    /**
     * @brief Get system swap size
     *
     * @param None
     *
     * @return An uint32_t number - The system swap size
     */
    uint32_t fui32_GetTotalSwap() const;

    /**
     * @brief Get current free system swap
     *
     * @param None
     *
     * @return An uint32_t number - The current free system swap
     */
    uint32_t fui32_GetFreeSwap() const;

    /**
     * @brief Get current reclaimable memory in kernel
     *
     * @param None
     *
     * @return An uint32_t number - The size of current reclaimable memory in kernel
     */
    uint32_t fui32_GetKReclaimable() const;

    /**
     * @brief Get reclaimable shared memory
     *
     * @param None
     *
     * @return An uint32_t number - The reclaimable size of shared memory
     */
    uint32_t fui32_GetSReclaimable() const;

    /**
     * @brief Get the stack size of kernel
     *
     * @param None
     *
     * @return An uint32_t number - The stack size of kernel
     */
    uint32_t fui32_GetKernelStack() const;

    /**
     * @brief Get page table size
     *
     * @param None
     *
     * @return An uint32_t number - The page table size
     */
    uint32_t fui32_GetPageTables() const;

    /**
     * @brief Get the total size of virtual allocated memory
     *
     * @param None
     *
     * @return An uint32_t number - the total size of virtual allocated memory
     */
    uint32_t fui32_GetVmallocTotal() const;

    /**
     * @brief Get the host name
     *
     * @param None
     *
     * @return a string of the host name
     */
    uint32_t fui32_GetVmallocUsed() const;

    /**
     * @brief Get chunk size of virtual allocated memory
     *
     * @param None
     *
     * @return An uint32_t number - The chunk size of virtual allocated memory
     */
    uint32_t fui32_GetVmallocChunk() const;


private:
    /**
     * @brief Read meminfo filesystem to get current memory info in system
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadMemInfoFs() const;

    mutable uint32_t mui32_Total;
    mutable uint32_t mui32_Available;
    mutable uint32_t mui32_Free;
    mutable uint32_t mui32_Shmem;
    mutable uint32_t mui32_Buffers;
    mutable uint32_t mui32_Cached;
    mutable uint32_t mui32_TotalSwap;
    mutable uint32_t mui32_FreeSwap;
    mutable uint32_t mui32_KReclaimable;
    mutable uint32_t mui32_SReclaimable;
    mutable uint32_t mui32_KernelStack;
    mutable uint32_t mui32_PageTables;
    mutable uint32_t mui32_VmallocTotal;
    mutable uint32_t mui32_VmallocUsed;
    mutable uint32_t mui32_VmallocChunk;
};

#endif
