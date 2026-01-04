#include "CHostInfo.hpp"
#include "Definitions.hpp"
#include <unistd.h>
#include <limits.h>

CHostInfo::CHostInfo() {
}

std::string CHostInfo::fstr_GetName() const {
    if (mstr_Name.empty()) {
        char ac_Buffer[HOST_NAME_MAX];
        if (SYSCALL_SUCCESS == gethostname(ac_Buffer, HOST_NAME_MAX)) {
            mstr_Name.assign(ac_Buffer);
        }
    }
    return mstr_Name;
}

std::string CHostInfo::fstr_GetUser() const {
    if (mstr_User.empty()) {
        char ac_Buffer[NAME_MAX];
        if (SYSCALL_SUCCESS == getlogin_r(ac_Buffer, NAME_MAX)) {
            mstr_User.assign(ac_Buffer);
        }
    }
    return mstr_User;
}