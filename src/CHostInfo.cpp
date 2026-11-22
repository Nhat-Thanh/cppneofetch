#include "CHostInfo.hpp"
#include "utils/Definitions.hpp"
#include "helpers/FileReader.hpp"

#include <limits.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <cstdlib>
#include <vector>

namespace {

constexpr const char* kpc_PathHostname = "/etc/hostname";
constexpr const char* kpc_EnvUser      = "USER";

} // namespace

/**
 * @brief Construct CHostInfo and eagerly resolve host and user names.
 */
CHostInfo::CHostInfo() {
    char ac_HostBuf[HOST_NAME_MAX + 1]{};
    if (0 == ::gethostname(ac_HostBuf, sizeof(ac_HostBuf) - 1U)) {
        ac_HostBuf[sizeof(ac_HostBuf) - 1U] = '\0';
        mstr_Name.assign(ac_HostBuf);
    } else {
        struct utsname uts_Info{};
        if (0 == ::uname(&uts_Info)) {
            mstr_Name.assign(uts_Info.nodename);
        } else {
            FileReader::fb_ReadFirstLine(mstr_Name, kpc_PathHostname);
        }
    }

    struct passwd pwd{};
    struct passwd* p_Result = nullptr;
    std::vector<char> vec_PwBuf(BufferSizes::kui64_PwEntryBuf);
    const uid_t ui32_Uid = ::geteuid();
    if (0 == ::getpwuid_r(ui32_Uid, &pwd, vec_PwBuf.data(), vec_PwBuf.size(), &p_Result) &&
        nullptr != p_Result) {
        mstr_User.assign(p_Result->pw_name);
    } else if (const char* kpc_EnvUserVal = std::getenv(kpc_EnvUser);
               nullptr != kpc_EnvUserVal && '\0' != kpc_EnvUserVal[0]) {
        mstr_User.assign(kpc_EnvUserVal);
    } else {
        char ac_LoginBuf[LOGIN_NAME_MAX + 1]{};
        if (0 == ::getlogin_r(ac_LoginBuf, sizeof(ac_LoginBuf) - 1U)) {
            ac_LoginBuf[sizeof(ac_LoginBuf) - 1U] = '\0';
            mstr_User.assign(ac_LoginBuf);
        }
    }
}
