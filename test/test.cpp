#include "TestCommon.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        fv_RunTestStringHandler();
        fv_RunTestFileReader();
        fv_RunTestRaiiHandles();

        fv_RunTestHostInfo();
        fv_RunTestOsInfo();
        fv_RunTestTerminalInfo();
        fv_RunTestProductInfo();
        fv_RunTestPackageManInfo();
        fv_RunTestMemInfo();
        fv_RunTestCpuInfo();
        fv_RunTestGpuInfo();

        std::cout << "\n[SUMMARY] All 11 test suites passed\n";
        return 0;
    } catch (const std::exception& kr_Ex) {
        std::cerr << "[FAIL] " << kr_Ex.what() << '\n';
        return 1;
    }
}
