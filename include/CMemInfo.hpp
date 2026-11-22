#ifndef CPPNEOFETCH_CMEMINFO_HPP
#define CPPNEOFETCH_CMEMINFO_HPP

#include <cstdint>

/**
 * @brief A class to get current system memory information (in KiB) from /proc/meminfo.
 */
class CMemInfo {
public:
    /**
     * @brief Construct CMemInfo and parse /proc/meminfo.
     */
    CMemInfo();

    /**
     * @brief Get total physical RAM in KiB.
     *
     * @return Total memory in KiB
     */
    uint64_t fui64_GetTotal() const noexcept { return mui64_Total; }

    /**
     * @brief Get available memory in KiB.
     *
     * @return Available memory in KiB
     */
    uint64_t fui64_GetAvailable() const noexcept { return mui64_Available; }

    /**
     * @brief Get completely unallocated free memory in KiB.
     *
     * @return Free memory in KiB
     */
    uint64_t fui64_GetFree() const noexcept { return mui64_Free; }

    /**
     * @brief Get used memory in KiB (Total - Available, or Total - Free fallback).
     *
     * @return Used memory in KiB
     */
    uint64_t fui64_GetUsed() const noexcept;

    /**
     * @brief Get shared memory (Shmem) size in KiB.
     *
     * @return Shared memory size in KiB
     */
    uint64_t fui64_GetShmem() const noexcept { return mui64_Shmem; }

    /**
     * @brief Get kernel buffer memory size in KiB.
     *
     * @return Buffer memory size in KiB
     */
    uint64_t fui64_GetBuffers() const noexcept { return mui64_Buffers; }

    /**
     * @brief Get page cache size in KiB.
     *
     * @return Page cache size in KiB
     */
    uint64_t fui64_GetCached() const noexcept { return mui64_Cached; }

    /**
     * @brief Get total swap space in KiB.
     *
     * @return Total swap space in KiB
     */
    uint64_t fui64_GetTotalSwap() const noexcept { return mui64_TotalSwap; }

    /**
     * @brief Get free swap space in KiB.
     *
     * @return Free swap space in KiB
     */
    uint64_t fui64_GetFreeSwap() const noexcept { return mui64_FreeSwap; }

    /**
     * @brief Get kernel reclaimable memory in KiB.
     *
     * @return Kernel reclaimable memory in KiB
     */
    uint64_t fui64_GetKReclaimable() const noexcept { return mui64_KReclaimable; }

    /**
     * @brief Get slab reclaimable memory in KiB.
     *
     * @return Slab reclaimable memory in KiB
     */
    uint64_t fui64_GetSReclaimable() const noexcept { return mui64_SReclaimable; }

    /**
     * @brief Get kernel stack memory size in KiB.
     *
     * @return Kernel stack size in KiB
     */
    uint64_t fui64_GetKernelStack() const noexcept { return mui64_KernelStack; }

    /**
     * @brief Get page tables memory size in KiB.
     *
     * @return Page tables size in KiB
     */
    uint64_t fui64_GetPageTables() const noexcept { return mui64_PageTables; }

    /**
     * @brief Get total vmalloc virtual address space in KiB.
     *
     * @return Total vmalloc size in KiB
     */
    uint64_t fui64_GetVmallocTotal() const noexcept { return mui64_VmallocTotal; }

    /**
     * @brief Get used vmalloc memory in KiB.
     *
     * @return Used vmalloc size in KiB
     */
    uint64_t fui64_GetVmallocUsed() const noexcept { return mui64_VmallocUsed; }

    /**
     * @brief Get largest contiguous free vmalloc chunk in KiB.
     *
     * @return Vmalloc chunk size in KiB
     */
    uint64_t fui64_GetVmallocChunk() const noexcept { return mui64_VmallocChunk; }

private:
    /**
     * @brief Read and parse memory metrics from /proc/meminfo in a single pass.
     */
    void fv_ReadMemInfoFs();

    uint64_t mui64_Total{0ULL};
    uint64_t mui64_Available{0ULL};
    uint64_t mui64_Free{0ULL};
    uint64_t mui64_Shmem{0ULL};
    uint64_t mui64_Buffers{0ULL};
    uint64_t mui64_Cached{0ULL};
    uint64_t mui64_TotalSwap{0ULL};
    uint64_t mui64_FreeSwap{0ULL};
    uint64_t mui64_KReclaimable{0ULL};
    uint64_t mui64_SReclaimable{0ULL};
    uint64_t mui64_KernelStack{0ULL};
    uint64_t mui64_PageTables{0ULL};
    uint64_t mui64_VmallocTotal{0ULL};
    uint64_t mui64_VmallocUsed{0ULL};
    uint64_t mui64_VmallocChunk{0ULL};
    bool mb_HasAvailable{false};
};

#endif // CPPNEOFETCH_CMEMINFO_HPP
