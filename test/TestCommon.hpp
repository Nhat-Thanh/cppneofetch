#ifndef CPPNEOFETCH_TEST_COMMON_HPP
#define CPPNEOFETCH_TEST_COMMON_HPP

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unistd.h>

#define TEST_CHECK(cond)                                                               \
    do {                                                                               \
        if (!(cond)) {                                                                 \
            std::ostringstream oss_Err;                                                \
            oss_Err << "Assertion failed at " << __FILE__ << ":" << __LINE__           \
                    << " -> " << #cond;                                                \
            throw std::runtime_error(oss_Err.str());                                   \
        }                                                                              \
    } while (false)

#define TEST_CHECK_EQ(lhs, rhs)                                                        \
    do {                                                                               \
        if (!((lhs) == (rhs))) {                                                       \
            std::ostringstream oss_Err;                                                \
            oss_Err << "Equality failed at " << __FILE__ << ":" << __LINE__            \
                    << " -> " << #lhs << " != " << #rhs;                               \
            throw std::runtime_error(oss_Err.str());                                   \
        }                                                                              \
    } while (false)

/**
 * @brief RAII temporary directory deleted automatically after each test case.
 */
class CScopedTempDir {
public:
    explicit CScopedTempDir(std::string_view sv_Prefix = "cppneofetch_test_") {
        std::string str_Template =
            (std::filesystem::temp_directory_path() / (std::string(sv_Prefix) + "XXXXXX")).string();
        if (nullptr == ::mkdtemp(str_Template.data())) {
            throw std::runtime_error("Failed to create temporary test directory");
        }
        m_Path = str_Template;
    }

    ~CScopedTempDir() {
        std::error_code ec;
        std::filesystem::remove_all(m_Path, ec);
    }

    CScopedTempDir(const CScopedTempDir&) = delete;
    CScopedTempDir& operator=(const CScopedTempDir&) = delete;

    const std::filesystem::path& fkr_Path() const noexcept { return m_Path; }

    std::string fstr_WriteFile(std::string_view sv_RelName, std::string_view sv_Content) const {
        const std::filesystem::path p_File = m_Path / sv_RelName;
        std::filesystem::create_directories(p_File.parent_path());
        std::ofstream ofs(p_File, std::ios::binary | std::ios::trunc);
        if (!ofs.is_open()) {
            throw std::runtime_error("Failed to open temp file for writing: " + p_File.string());
        }
        ofs.write(sv_Content.data(), static_cast<std::streamsize>(sv_Content.size()));
        ofs.close();
        return p_File.string();
    }

private:
    std::filesystem::path m_Path;
};

/**
 * @brief RAII guard that saves and restores an environment variable on scope exit.
 */
class CScopedEnvVar {
public:
    explicit CScopedEnvVar(const char* kpc_Name) : mpc_Name(kpc_Name) {
        if (const char* kpc_Val = std::getenv(mpc_Name); nullptr != kpc_Val) {
            mopt_Original = std::string(kpc_Val);
        }
    }

    ~CScopedEnvVar() {
        if (mopt_Original) {
            ::setenv(mpc_Name, mopt_Original->c_str(), 1);
        } else {
            ::unsetenv(mpc_Name);
        }
    }

    CScopedEnvVar(const CScopedEnvVar&) = delete;
    CScopedEnvVar& operator=(const CScopedEnvVar&) = delete;

    void fv_Set(const char* kpc_Val) const { ::setenv(mpc_Name, kpc_Val, 1); }
    void fv_Unset() const { ::unsetenv(mpc_Name); }

private:
    const char* mpc_Name;
    std::optional<std::string> mopt_Original{""};
};

void fv_RunTestStringHandler();
void fv_RunTestFileReader();
void fv_RunTestRaiiHandles();
void fv_RunTestHostInfo();
void fv_RunTestOsInfo();
void fv_RunTestTerminalInfo();
void fv_RunTestProductInfo();
void fv_RunTestPackageManInfo();
void fv_RunTestMemInfo();
void fv_RunTestCpuInfo();
void fv_RunTestGpuInfo();

#endif // CPPNEOFETCH_TEST_COMMON_HPP
