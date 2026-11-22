#include "CTerminalInfo.hpp"
#include "utils/Definitions.hpp"
#include "helpers/FileReader.hpp"
#include "helpers/StringHandler.hpp"

#include <unistd.h>
#include <array>
#include <cstdlib>
#include <optional>

namespace {

constexpr int         ki32_MaxWalkDepth           = 32;
constexpr pid_t       ki32_InitPid                = 1;
constexpr std::size_t kui64_StatAfterParenOffset  = 2U; // Skip ") " after comm in /proc/<pid>/stat
constexpr int         ki32_StatParseFieldCount    = 2;

constexpr const char* kpc_EnvTermProgram          = "TERM_PROGRAM";
constexpr const char* kpc_EnvTerm                 = "TERM";
constexpr const char* kpc_StatParseFmt            = "%c %d";

constexpr std::string_view ksv_DevPrefix          = "/dev/";
constexpr std::string_view ksv_WrappedSuffix      = "-wrapped";
constexpr std::string_view ksv_DotPrefix          = ".";
constexpr std::string_view ksv_GnomeTerminalPfx   = "gnome-terminal";
constexpr std::string_view ksv_GnomeTerminalName  = "gnome-terminal";
constexpr std::string_view ksv_GnomeConsoleAlias  = "kgx";
constexpr std::string_view ksv_GnomeConsoleName   = "gnome-console";
constexpr std::string_view ksv_UrxvtDaemonAlias   = "urxvtd";
constexpr std::string_view ksv_UrxvtName          = "urxvt";

typedef struct {
    const char* kpc_EnvVar;
    const char* kpc_TerminalName;
} env_terminal_t;

constexpr std::array<env_terminal_t, 8> ska_EnvTerminals = {{
    { "KITTY_WINDOW_ID",       "kitty"            },
    { "KITTY_PID",             "kitty"            },
    { "ALACRITTY_SOCKET",      "alacritty"        },
    { "ALACRITTY_LOG",         "alacritty"        },
    { "GHOSTTY_RESOURCES_DIR", "ghostty"          },
    { "WEZTERM_EXECUTABLE",    "wezterm"          },
    { "KONSOLE_VERSION",       "konsole"          },
    { "WT_SESSION",            "Windows Terminal" },
}};

constexpr std::array<std::string_view, 3> ska_Multiplexers = {
    "tmux", "tmux: server", "screen"
};

constexpr std::array<std::string_view, 22> ska_SkipProcesses = {
    "bash", "zsh", "fish", "sh", "dash", "ash", "ksh", "tcsh", "csh",
    "nu", "elvish", "xonsh", "ion",
    "sudo", "doas", "su", "env", "make", "strace", "time", "gdb",
    "cppneofetch"
};

constexpr std::array<std::string_view, 15> ska_StopProcesses = {
    "systemd", "init", "launchd", "gnome-shell", "plasmashell",
    "Hyprland", "sway", "river", "niri", "i3", "openbox",
    "xfce4-session", "dbus-daemon", "sshd", "login"
};

bool sfb_IsInList(std::string_view sv_Val, const auto& kra_List) {
    for (std::string_view sv_Item : kra_List) {
        if (sv_Item == sv_Val) {
            return true;
        }
    }
    return false;
}

std::string sfstr_NormalizeTerminalName(std::string str_Name) {
    if (str_Name.starts_with(ksv_GnomeTerminalPfx)) {
        return std::string(ksv_GnomeTerminalName);
    }
    if (ksv_GnomeConsoleAlias == str_Name) {
        return std::string(ksv_GnomeConsoleName);
    }
    if (ksv_UrxvtDaemonAlias == str_Name || ksv_UrxvtName == str_Name) {
        return std::string(ksv_UrxvtName);
    }
    if (str_Name.starts_with(ksv_DotPrefix)) {
        str_Name.erase(0U, ksv_DotPrefix.size());
    }
    if (str_Name.ends_with(ksv_WrappedSuffix)) {
        str_Name.erase(str_Name.size() - ksv_WrappedSuffix.size());
    }
    return str_Name;
}

bool sfb_DetectFromEnv(std::string& rstr_Out) {
    for (const env_terminal_t& kr_Entry : ska_EnvTerminals) {
        const char* kpc_Val = std::getenv(kr_Entry.kpc_EnvVar);
        if (nullptr != kpc_Val && '\0' != kpc_Val[0]) {
            rstr_Out.assign(kr_Entry.kpc_TerminalName);
            return true;
        }
    }
    if (const char* kpc_TermProg = std::getenv(kpc_EnvTermProgram);
        nullptr != kpc_TermProg && '\0' != kpc_TermProg[0]) {
        const std::string_view sv_TermProg(kpc_TermProg);
        if (!sfb_IsInList(sv_TermProg, ska_Multiplexers)) {
            rstr_Out.assign(sv_TermProg);
            return true;
        }
    }
    return false;
}

// TODO: Potential bug: If the cmd take a paths as arguments, `sfstr_ReadProcessName` would retrieve wrong information
std::string sfstr_ReadProcessName(pid_t i32_Pid) {
    // Prefer /proc/<pid>/cmdline argv[0] to avoid 15-char truncation in /proc/<pid>/comm
    const std::string str_CmdlinePath = StringHandler::fstr_Format(Paths::kpc_ProcCmdlineFmt, i32_Pid);
    std::string str_Raw;
    if (FileReader::fb_ReadAll(str_Raw, str_CmdlinePath) && !str_Raw.empty()) {
        const std::size_t ui64_NulPos = str_Raw.find('\0');
        std::string_view sv_Argv0(str_Raw);
        if (std::string_view::npos != ui64_NulPos) {
            sv_Argv0 = sv_Argv0.substr(0U, ui64_NulPos);
        }
        if (!sv_Argv0.empty()) {
            const std::size_t ui64_Slash = sv_Argv0.rfind('/');
            if (std::string_view::npos != ui64_Slash) {
                sv_Argv0.remove_prefix(ui64_Slash + 1U);
            }
            if (!sv_Argv0.empty()) {
                return std::string(sv_Argv0);
            }
        }
    }

    const std::string str_CommPath = StringHandler::fstr_Format(Paths::kpc_ProcCommFmt, i32_Pid);
    std::string str_Comm;
    if (FileReader::fb_ReadFirstLine(str_Comm, str_CommPath)) {
        StringHandler::fv_Trim(str_Comm);
        return str_Comm;
    }
    return {};
}

std::optional<pid_t> sfopt_ReadParentPid(pid_t i32_Pid) {
    const std::string str_StatPath = StringHandler::fstr_Format(Paths::kpc_ProcStatFmt, i32_Pid);
    std::string str_Line;
    if (!FileReader::fb_ReadFirstLine(str_Line, str_StatPath)) {
        return std::nullopt;
    }
    // Find last ')' so process names containing spaces/parens parse safely
    const std::size_t ui64_RParen = str_Line.rfind(')');
    if (std::string::npos == ui64_RParen || str_Line.size() <= ui64_RParen + kui64_StatAfterParenOffset) {
        return std::nullopt;
    }
    int i32_PPid = 0;
    char c_State = '\0';
    if (ki32_StatParseFieldCount == std::sscanf(str_Line.c_str() + ui64_RParen + kui64_StatAfterParenOffset,
                                               kpc_StatParseFmt, &c_State, &i32_PPid) &&
        0 < i32_PPid) {
        return static_cast<pid_t>(i32_PPid);
    }
    return std::nullopt;
}

} // namespace

/**
 * @brief Construct CTerminalInfo and detect the active terminal application.
 */
CTerminalInfo::CTerminalInfo() {
    if (sfb_DetectFromEnv(mstr_Name)) {
        return;
    }

    pid_t i32_Pid = ::getppid();
    for (int i32_Depth = 0; i32_Depth < ki32_MaxWalkDepth && i32_Pid > ki32_InitPid; ++i32_Depth) {
        std::string str_ProcName = sfstr_ReadProcessName(i32_Pid);
        if (!str_ProcName.empty()) {
            if (sfb_IsInList(str_ProcName, ska_StopProcesses)) {
                break;
            }
            if (!sfb_IsInList(str_ProcName, ska_SkipProcesses) &&
                !sfb_IsInList(str_ProcName, ska_Multiplexers)) {
                mstr_Name = sfstr_NormalizeTerminalName(std::move(str_ProcName));
                return;
            }
        }
        const std::optional<pid_t> opt_NextPid = sfopt_ReadParentPid(i32_Pid);
        if (!opt_NextPid || i32_Pid == *opt_NextPid) {
            break;
        }
        i32_Pid = *opt_NextPid;
    }

    if (const char* kpc_TermProg = std::getenv(kpc_EnvTermProgram);
        nullptr != kpc_TermProg && '\0' != kpc_TermProg[0]) {
        mstr_Name = kpc_TermProg;
        return;
    }

    if (const char* kpc_Tty = ::ttyname(STDIN_FILENO);
        nullptr != kpc_Tty && '\0' != kpc_Tty[0]) {
        std::string_view sv_Tty(kpc_Tty);
        if (sv_Tty.starts_with(ksv_DevPrefix)) {
            sv_Tty.remove_prefix(ksv_DevPrefix.size());
        }
        mstr_Name = std::string(sv_Tty);
        return;
    }

    if (const char* kpc_Term = std::getenv(kpc_EnvTerm);
        nullptr != kpc_Term && '\0' != kpc_Term[0]) {
        mstr_Name = kpc_Term;
    }
}
