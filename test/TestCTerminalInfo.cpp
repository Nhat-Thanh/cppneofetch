#include "TestCommon.hpp"
#include "CTerminalInfo.hpp"

#include <iostream>

void fv_RunTestTerminalInfo() {
    // Save and restore all terminal environment variables on scope exit
    CScopedEnvVar envKittyWin("KITTY_WINDOW_ID");
    CScopedEnvVar envKittyPid("KITTY_PID");
    CScopedEnvVar envAlacrittySock("ALACRITTY_SOCKET");
    CScopedEnvVar envAlacrittyLog("ALACRITTY_LOG");
    CScopedEnvVar envGhostty("GHOSTTY_RESOURCES_DIR");
    CScopedEnvVar envWezterm("WEZTERM_EXECUTABLE");
    CScopedEnvVar envKonsole("KONSOLE_VERSION");
    CScopedEnvVar envWt("WT_SESSION");
    CScopedEnvVar envTermProg("TERM_PROGRAM");
    CScopedEnvVar envTerm("TERM");

    auto fn_ClearAll = [&]() {
        envKittyWin.fv_Unset();
        envKittyPid.fv_Unset();
        envAlacrittySock.fv_Unset();
        envAlacrittyLog.fv_Unset();
        envGhostty.fv_Unset();
        envWezterm.fv_Unset();
        envKonsole.fv_Unset();
        envWt.fv_Unset();
        envTermProg.fv_Unset();
    };

    // 1. Normal env-based terminal detection
    {
        fn_ClearAll();
        envKittyWin.fv_Set("1");
        const CTerminalInfo termKittyWin;
        TEST_CHECK_EQ("kitty", termKittyWin.fstr_GetName());

        fn_ClearAll();
        envKittyPid.fv_Set("12345");
        const CTerminalInfo termKittyPid;
        TEST_CHECK_EQ("kitty", termKittyPid.fstr_GetName());

        fn_ClearAll();
        envAlacrittySock.fv_Set("/run/user/1000/Alacritty.sock");
        const CTerminalInfo termAlacritty;
        TEST_CHECK_EQ("alacritty", termAlacritty.fstr_GetName());

        fn_ClearAll();
        envGhostty.fv_Set("/usr/share/ghostty");
        const CTerminalInfo termGhostty;
        TEST_CHECK_EQ("ghostty", termGhostty.fstr_GetName());

        fn_ClearAll();
        envWezterm.fv_Set("/usr/bin/wezterm");
        const CTerminalInfo termWezterm;
        TEST_CHECK_EQ("wezterm", termWezterm.fstr_GetName());

        fn_ClearAll();
        envKonsole.fv_Set("240800");
        const CTerminalInfo termKonsole;
        TEST_CHECK_EQ("konsole", termKonsole.fstr_GetName());

        fn_ClearAll();
        envWt.fv_Set("guid-session");
        const CTerminalInfo termWt;
        TEST_CHECK_EQ("Windows Terminal", termWt.fstr_GetName());
    }

    // 2. Corner cases: TERM_PROGRAM normal vs multiplexer bypass & empty values
    {
        fn_ClearAll();
        envTermProg.fv_Set("vscode");
        const CTerminalInfo termVscode;
        TEST_CHECK_EQ("vscode", termVscode.fstr_GetName());

        // Empty env vars must be ignored
        fn_ClearAll();
        envKittyWin.fv_Set("");
        envTermProg.fv_Set("");
        envTerm.fv_Set("xterm-256color");
        const CTerminalInfo termFallback;
        TEST_CHECK(!termFallback.fstr_GetName().empty());

        // Multiplexer values should allow process-tree walk or fallback without crashing
        fn_ClearAll();
        envTermProg.fv_Set("tmux");
        envTerm.fv_Set("tmux-256color");
        const CTerminalInfo termTmux;
        TEST_CHECK(!termTmux.fstr_GetName().empty());
    }

    std::cout << "[PASS] TestCTerminalInfo\n";
}
