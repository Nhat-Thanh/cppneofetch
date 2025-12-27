#include "CMemInfo.hpp"
#include "CStringHelper.hpp"
#include "Definitions.hpp"
#include <fstream>
#include <vector>

#define PATH_MEMINFO_VFS "/proc/meminfo"

CMemInfo::CMemInfo() :
    mui32_Total(UINT32_INIT),
    mui32_Available(UINT32_INIT),
    mui32_Free(UINT32_INIT),
    mui32_Shmem(UINT32_INIT),
    mui32_Buffers(UINT32_INIT),
    mui32_Cached(UINT32_INIT),
    mui32_TotalSwap(UINT32_INIT),
    mui32_FreeSwap(UINT32_INIT),
    mui32_KReclaimable(UINT32_INIT),
    mui32_SReclaimable(UINT32_INIT),
    mui32_KernelStack(UINT32_INIT),
    mui32_PageTables(UINT32_INIT),
    mui32_VmallocTotal(UINT32_INIT),
    mui32_VmallocUsed(UINT32_INIT),
    mui32_VmallocChunk(UINT32_INIT) {
}

uint32_t CMemInfo::fui32_GetTotal() {
    if (UINT32_INIT == mui32_Total) {
        _fv_ReadMemInfoFs();
    }
    return mui32_Total;
}

uint32_t CMemInfo::fui32_GetAvailable() {
    if (UINT32_INIT == mui32_Available) {
        _fv_ReadMemInfoFs();
    }
    return mui32_Available;
}

uint32_t CMemInfo::fui32_GetFree() {
    if (UINT32_INIT == mui32_Free) {
        _fv_ReadMemInfoFs();
    }
    return mui32_Free;
}

uint32_t CMemInfo::fui32_GetUsed() {
    return fui32_GetTotal() - fui32_GetAvailable();
}

uint32_t CMemInfo::fui32_GetShmem() {
    if (UINT32_INIT == mui32_Shmem) {
        _fv_ReadMemInfoFs();
    }
    return mui32_Shmem;
}

uint32_t CMemInfo::fui32_GetBuffers() {
    if (UINT32_INIT == mui32_Buffers) {
        _fv_ReadMemInfoFs();
    }
    return mui32_Buffers;
}

uint32_t CMemInfo::fui32_GetCached() {
    if (UINT32_INIT == mui32_Cached) {
        _fv_ReadMemInfoFs();
    }
    return mui32_Cached;
}

uint32_t CMemInfo::fui32_GetTotalSwap() {
    if (UINT32_INIT == mui32_TotalSwap) {
        _fv_ReadMemInfoFs();
    }
    return mui32_TotalSwap;
}

uint32_t CMemInfo::fui32_GetFreeSwap() {
    if (UINT32_INIT == mui32_FreeSwap) {
        _fv_ReadMemInfoFs();
    }
    return mui32_FreeSwap;
}

uint32_t CMemInfo::fui32_GetKReclaimable() {
    if (UINT32_INIT == mui32_KReclaimable) {
        _fv_ReadMemInfoFs();
    }
    return mui32_KReclaimable;
}

uint32_t CMemInfo::fui32_GetSReclaimable() {
    if (UINT32_INIT == mui32_SReclaimable) {
        _fv_ReadMemInfoFs();
    }
    return mui32_SReclaimable;
}

uint32_t CMemInfo::fui32_GetKernelStack() {
    if (UINT32_INIT == mui32_KernelStack) {
        _fv_ReadMemInfoFs();
    }
    return mui32_KernelStack;
}

uint32_t CMemInfo::fui32_GetPageTables() {
    if (UINT32_INIT == mui32_PageTables) {
        _fv_ReadMemInfoFs();
    }
    return mui32_PageTables;
}

uint32_t CMemInfo::fui32_GetVmallocTotal() {
    if (UINT32_INIT == mui32_VmallocTotal) {
        _fv_ReadMemInfoFs();
    }
    return mui32_VmallocTotal;
}

uint32_t CMemInfo::fui32_GetVmallocUsed() {
    if (UINT32_INIT == mui32_VmallocUsed) {
        _fv_ReadMemInfoFs();
    }
    return mui32_VmallocUsed;
}

uint32_t CMemInfo::fui32_GetVmallocChunk() {
    if (UINT32_INIT == mui32_VmallocChunk) {
        _fv_ReadMemInfoFs();
    }
    return mui32_VmallocChunk;
}


void CMemInfo::_fv_ReadMemInfoFs() {
    typedef struct {
        const char* kpc_Format;
        void* pv_Variable;
        bool b_Scanned;
    } sscanf_arg_t;

    std::vector<sscanf_arg_t> vec_sscanfArgs {
        { "MemTotal: %d",     &mui32_Total,        false },
        { "MemAvailable: %d", &mui32_Available,    false },
        { "MemFree: %d",      &mui32_Free,         false },
        { "Shmem: %d",        &mui32_Shmem,        false },
        { "Buffers: %d",      &mui32_Buffers,      false },
        { "Cached: %d",       &mui32_Cached,       false },
        { "TotalSwap: %d",    &mui32_TotalSwap,    false },
        { "FreeSwap: %d",     &mui32_FreeSwap,     false },
        { "KReclaimable: %d", &mui32_KReclaimable, false },
        { "SReclaimable: %d", &mui32_SReclaimable, false },
        { "KernelStack: %d",  &mui32_KernelStack,  false },
        { "PageTables: %d",   &mui32_PageTables,   false },
        { "VmallocTotal: %d", &mui32_VmallocTotal, false },
        { "VmallocUsed: %d",  &mui32_VmallocUsed,  false },
        { "VmallocChunk: %d", &mui32_VmallocChunk, false }
    };

    std::ifstream ifs;
    ifs.open(PATH_MEMINFO_VFS);
    if (ifs.is_open()) {
        std::string str_Line;
        while (std::getline(ifs, str_Line)) {
            for (sscanf_arg_t& arg : vec_sscanfArgs) {
                if (arg.b_Scanned) {
                    continue;
                }
                if (sscanf(str_Line.c_str(), arg.kpc_Format, arg.pv_Variable)) {
                    arg.b_Scanned = true;
                    break;
                }
            }
        }
    }
    ifs.close();
}
