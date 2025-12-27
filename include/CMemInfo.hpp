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
    uint32_t fui32_GetTotal();

    /**
     * @brief Get current available memory
     *
     * @param None
     *
     * @return An uint32_t number - The current available memory
     */
    uint32_t fui32_GetAvailable();

    /**
     * @brief Get current free memory
     *
     * @param None
     *
     * @return An uint32_t number - The current free memory
     */
    uint32_t fui32_GetFree();

    /**
     * @brief Get current used memory
     *
     * @param None
     *
     * @return An uint32_t number - The used memory
     */
    uint32_t fui32_GetUsed();

    /**
     * @brief Get shared memory size
     *
     * @param None
     *
     * @return An uint32_t number - The shared memory size
     */
    uint32_t fui32_GetShmem();

    /**
     * @brief Get buffer memory
     *
     * @param None
     *
     * @return An uint32_t number - The buffer size
     */
    uint32_t fui32_GetBuffers();

    /**
     * @brief Get cache size
     *
     * @param None
     *
     * @return An uint32_t number - The cache size
     */
    uint32_t fui32_GetCached();

    /**
     * @brief Get system swap size
     *
     * @param None
     *
     * @return An uint32_t number - The system swap size
     */
    uint32_t fui32_GetTotalSwap();

    /**
     * @brief Get current free system swap
     *
     * @param None
     *
     * @return An uint32_t number - The current free system swap
     */
    uint32_t fui32_GetFreeSwap();

    /**
     * @brief Get current reclaimable memory in kernel
     *
     * @param None
     *
     * @return An uint32_t number - The size of current reclaimable memory in kernel
     */
    uint32_t fui32_GetKReclaimable();

    /**
     * @brief Get reclaimable shared memory
     *
     * @param None
     *
     * @return An uint32_t number - The reclaimable size of shared memory
     */
    uint32_t fui32_GetSReclaimable();

    /**
     * @brief Get the stack size of kernel
     *
     * @param None
     *
     * @return An uint32_t number - The stack size of kernel
     */
    uint32_t fui32_GetKernelStack();

    /**
     * @brief Get page table size
     *
     * @param None
     *
     * @return An uint32_t number - The page table size
     */
    uint32_t fui32_GetPageTables();

    /**
     * @brief Get the total size of virtual allocated memory
     *
     * @param None
     *
     * @return An uint32_t number - the total size of virtual allocated memory
     */
    uint32_t fui32_GetVmallocTotal();

    /**
     * @brief Get the host name
     *
     * @param None
     *
     * @return a string of the host name
     */
    uint32_t fui32_GetVmallocUsed();

    /**
     * @brief Get chunk size of virtual allocated memory
     *
     * @param None
     *
     * @return An uint32_t number - The chunk size of virtual allocated memory
     */
    uint32_t fui32_GetVmallocChunk();


private:
    /**
     * @brief Read meminfo filesystem to get current memory info in system
     *
     * @param None
     *
     * @return None
     */
    void _fv_ReadMemInfoFs();

    uint32_t mui32_Total;
    uint32_t mui32_Available;
    uint32_t mui32_Free;
    uint32_t mui32_Shmem;
    uint32_t mui32_Buffers;
    uint32_t mui32_Cached;
    uint32_t mui32_TotalSwap;
    uint32_t mui32_FreeSwap;
    uint32_t mui32_KReclaimable;
    uint32_t mui32_SReclaimable;
    uint32_t mui32_KernelStack;
    uint32_t mui32_PageTables;
    uint32_t mui32_VmallocTotal;
    uint32_t mui32_VmallocUsed;
    uint32_t mui32_VmallocChunk;
};

#endif
