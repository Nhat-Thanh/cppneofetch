#include "COsInfo.hpp"
#include "CStringHelper.hpp"
#include "CFileReader.hpp"
#include "Definitions.hpp"
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

#define PATH_OSRELEASE_VFS "/etc/os-release"

COsInfo::COsInfo():
    mui64_Uptime(UINT64_INIT) {
}

std::string COsInfo::fstr_GetName() {
    if (mstr_Name.empty()) {
        _fv_ReadOsReleaseFs();
    }
    return mstr_Name;
}

std::string COsInfo::fstr_GetKernel() {
    if (mstr_Kernel.empty()) {
        struct utsname utsname_Info;
        if (SYSCALL_SUCCESS == uname(&utsname_Info)) {
            mstr_Kernel.assign(utsname_Info.release);
        }
    }
    return mstr_Kernel;
}

uint64_t COsInfo::fui64_GetUptime() {
    struct sysinfo SystemInfo;
    if (SYSCALL_SUCCESS == sysinfo(&SystemInfo)) {
        typedef struct {
            const char* kpc_Unit;
            long i64_time;
        } time_unit_t;
        mui64_Uptime = SystemInfo.uptime;
    }
    return mui64_Uptime;
}

std::string COsInfo::fstr_GetShell() {
    if (mstr_Shell.empty()) {
        _fv_ReadShellFs();
    }
    return mstr_Shell;
}


void COsInfo::_fv_ReadOsReleaseFs() {
    std::vector<std::string> vecstr_Lines;
    std::string str_Keyword = "PRETTY_NAME=";
    CFileReader::fi64_FindLine(vecstr_Lines, PATH_OSRELEASE_VFS, str_Keyword);
    mstr_Name.assign(vecstr_Lines.front().begin() + str_Keyword.size(), vecstr_Lines.front().end());
    CStringHelper::fv_Trim(mstr_Name, CHAR_QUOTE);
}

void COsInfo::_fv_ReadShellFs() {
    CFileReader::fi64_GetLine_nth(mstr_Shell, CStringHelper::fstr_Format(PATH_PROC_COMM_FORMAT, getppid()));
}

