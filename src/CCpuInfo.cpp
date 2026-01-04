#include "CCpuInfo.hpp"
#include "CFileReader.hpp"
#include "CStringHelper.hpp"
#include "Definitions.hpp"
#include <sys/sysinfo.h>
#include <limits.h>
#include <fstream>
#include <cstdlib>

#define PATH_CPUINFO_VFS "/proc/cpuinfo"
#define PATH_CPUMAXFREQ_FMT "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq"
#define PATH_CPUMINFREQ_FMT "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_min_freq"
#define BASE_DECIMAL 10

CCpuInfo::CCpuInfo(uint32_t ui32_CpuId):
    mui32_CpuId(ui32_CpuId),
    mui32_MaxFreq(UINT32_INIT),
    mui32_MinFreq(UINT32_INIT),
    mui32_Cache(UINT32_INIT),
    mui32_LogicalCores(UINT32_INIT),
    mui32_PhysicalCores(UINT32_INIT)
{
}

void CCpuInfo::_fv_ReadCpuInfoFS() const {
    char ac_ModelName[NAME_MAX];
    uint32_t ui32_Processor0 = 0;
    uint32_t ui32_CacheSize = 0;
    uint32_t ui32_PhysicalId = 0;
    uint32_t ui32_Siblings = 0;
    uint32_t ui32_CpuCores = 0;
    std::string str_Line;

    std::ifstream ifs(PATH_CPUINFO_VFS);
    if (ifs.is_open()) {
        typedef struct {
            const char* str_Format;
            void* pv_Value;
            bool b_Scanned;
        } sscanf_arg_t;

        std::vector<sscanf_arg_t> vec_Args = {
            { "processor : %d",       &ui32_Processor0, false },
            { "model name : %[^\n@]", ac_ModelName,     false },
            { "cache size : %d",      &ui32_CacheSize,  false },
            { "physical id : %d",     &ui32_PhysicalId, false },
            { "siblings : %d",        &ui32_Siblings,   false },
            { "cpu cores : %d",       &ui32_CpuCores,   false }
        };

        while (std::getline(ifs, str_Line)) {
            // end of a cpu core info
            if (str_Line.empty()) {
                if (ui32_PhysicalId == mui32_CpuId) {
                    mui32_LogicalCores = ui32_Siblings;
                    mui32_Cache = ui32_CacheSize;
                    mui32_PhysicalCores = ui32_CpuCores;
                    mstr_Name.assign(ac_ModelName);
                    CStringHelper::fv_RightTrim(mstr_Name, ' ');
                    break;
                    // After got the processor id of "core0" of target CPU, we don't need to read this file more
                }
                for (sscanf_arg_t& arg : vec_Args) {
                    arg.b_Scanned = false;
                }
            }

            for (sscanf_arg_t& arg : vec_Args) {
                if (arg.b_Scanned) {
                    continue;
                }
                if (sscanf(str_Line.c_str(), arg.str_Format, arg.pv_Value)) {
                    arg.b_Scanned = true;
                    break;
                }
            }
        }
    }
    ifs.close();

    if (ui32_PhysicalId == mui32_CpuId) {
        _fv_ReadCpuMaxFreqFS(ui32_Processor0);
        _fv_ReadCpuMinFreqFS(ui32_Processor0);
    }
}

void CCpuInfo::_fv_ReadCpuMaxFreqFS(uint32_t ui32_Processor) const {
    std::string str_Line;
    std::string str_CpuFreqPath;
    CStringHelper::fv_Format(str_CpuFreqPath, PATH_CPUMAXFREQ_FMT, ui32_Processor);
    if (EXIT_SUCCESS == CFileReader::fi64_GetLine_nth(str_Line, str_CpuFreqPath)) {
        mui32_MaxFreq = strtoul(str_Line.c_str(), NULL, BASE_DECIMAL);
    }
}

void CCpuInfo::_fv_ReadCpuMinFreqFS(uint32_t ui32_Processor) const {
    std::string str_Line;
    std::string str_CpuFreqPath;
    CStringHelper::fv_Format(str_CpuFreqPath, PATH_CPUMINFREQ_FMT, ui32_Processor);
    if (EXIT_SUCCESS == CFileReader::fi64_GetLine_nth(str_Line, str_CpuFreqPath)) {
        mui32_MaxFreq = strtoul(str_Line.c_str(), NULL, BASE_DECIMAL);
    }
}


std::string CCpuInfo::fstr_GetName() const {
    if (mstr_Name.empty()) {
        _fv_ReadCpuInfoFS();
    }
    return mstr_Name;
}

uint32_t CCpuInfo::fui32_GetCpuId() const {
    return mui32_CpuId;
}

uint32_t CCpuInfo::fui32_GetMaxFreq() const {
    if (UINT32_INIT == mui32_MaxFreq) {
        _fv_ReadCpuInfoFS();
    }
    return mui32_MaxFreq;
}

uint32_t CCpuInfo::fui32_GetMinFreq() const {
    if (UINT32_INIT == mui32_MinFreq) {
        _fv_ReadCpuInfoFS();
    }
    return mui32_MinFreq;
}

uint32_t CCpuInfo::fui32_GetCache() const {
    if (UINT32_INIT == mui32_Cache) {
        _fv_ReadCpuInfoFS();
    }
    return mui32_Cache;
}

uint32_t CCpuInfo::fui32_GetLogicalCores() const {
    if (UINT32_INIT == mui32_LogicalCores) {
        _fv_ReadCpuInfoFS();
    }
    return mui32_LogicalCores;
}

uint32_t CCpuInfo::fui32_GetPhysicalCores() const {
    if (UINT32_INIT == mui32_PhysicalCores) {
        _fv_ReadCpuInfoFS();
    }
    return mui32_PhysicalCores;
}
