#include "TestCommon.hpp"
#include "CMemInfo.hpp"

#include <iostream>

void fv_RunTestMemInfo() {
    const CMemInfo mem;

    // 1. Exercise all 16 getters
    const uint64_t ui64_Total        = mem.fui64_GetTotal();
    const uint64_t ui64_Available    = mem.fui64_GetAvailable();
    const uint64_t ui64_Free         = mem.fui64_GetFree();
    const uint64_t ui64_Used         = mem.fui64_GetUsed();
    const uint64_t ui64_Shmem        = mem.fui64_GetShmem();
    const uint64_t ui64_Buffers      = mem.fui64_GetBuffers();
    const uint64_t ui64_Cached       = mem.fui64_GetCached();
    const uint64_t ui64_TotalSwap    = mem.fui64_GetTotalSwap();
    const uint64_t ui64_FreeSwap     = mem.fui64_GetFreeSwap();
    const uint64_t ui64_KReclaimable = mem.fui64_GetKReclaimable();
    const uint64_t ui64_SReclaimable = mem.fui64_GetSReclaimable();
    const uint64_t ui64_KernelStack  = mem.fui64_GetKernelStack();
    const uint64_t ui64_PageTables   = mem.fui64_GetPageTables();
    const uint64_t ui64_VmallocTotal = mem.fui64_GetVmallocTotal();
    const uint64_t ui64_VmallocUsed  = mem.fui64_GetVmallocUsed();
    const uint64_t ui64_VmallocChunk = mem.fui64_GetVmallocChunk();

    // 2. Physical & kernel memory invariants
    TEST_CHECK(0ULL < ui64_Total);
    TEST_CHECK(ui64_Total >= ui64_Used);
    TEST_CHECK(ui64_Total >= ui64_Free);
    TEST_CHECK(ui64_Total >= ui64_Available);
    TEST_CHECK(ui64_TotalSwap >= ui64_FreeSwap);
    TEST_CHECK(ui64_Total >= ui64_Shmem);
    TEST_CHECK(ui64_Total >= ui64_Buffers);
    TEST_CHECK(ui64_Total >= ui64_Cached);
    TEST_CHECK(0ULL == ui64_KReclaimable || ui64_SReclaimable <= ui64_KReclaimable);
    TEST_CHECK(0ULL < ui64_KernelStack);
    TEST_CHECK(ui64_Total >= ui64_PageTables);
    TEST_CHECK(0ULL < ui64_VmallocTotal);
    TEST_CHECK(ui64_VmallocTotal >= ui64_VmallocUsed);
    TEST_CHECK(ui64_VmallocTotal >= ui64_VmallocChunk);

    if (0ULL < ui64_Available) {
        TEST_CHECK_EQ(ui64_Total - ui64_Available, ui64_Used);
    }

    // On 64-bit Linux, VmallocTotal is typically ~32 TiB (exceeding uint32_t max)
    if (8U == sizeof(void*)) {
        TEST_CHECK(static_cast<uint64_t>(UINT32_MAX) < ui64_VmallocTotal);
    }

    std::cout << "[PASS] TestCMemInfo\n";
}
