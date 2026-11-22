#include "helpers/FileReader.hpp"
#include "helpers/RaiiHandles.hpp"
#include "helpers/StringHandler.hpp"

#include <cerrno>
#include <charconv>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>

namespace FileReader {

namespace {

constexpr std::string_view ksv_HexPrefixLower = "0x";
constexpr std::string_view ksv_HexPrefixUpper = "0X";

} // namespace

/**
 * @brief Read the first line of a file, stripping trailing CR/LF characters.
 *
 * @param rstr_Dest - Destination string updated on success (untouched on error)
 * @param krstr_Path - Path to the file to read
 *
 * @return true if the file was opened and read successfully, false otherwise
 */
bool fb_ReadFirstLine(std::string& rstr_Dest,
                      const std::string& krstr_Path) {
    CUniqueFd fd(::open(krstr_Path.c_str(), O_RDONLY | O_CLOEXEC));
    if (!fd) {
        return false;
    }

    char ac_Buf[BufferSizes::kui64_LineBuf];
    std::string str_Line;
    // reading unitl reach the end line character
    while (true) {
        const ssize_t i64_Read = ::read(fd.fi32_Get(), ac_Buf, sizeof(ac_Buf));
        if (0 > i64_Read) {
            if (EINTR == errno) {
                continue;
            }
            return false;
        }
        if (0 == i64_Read) {
            break;
        }
        const std::string_view sv_Chunk(ac_Buf, static_cast<std::size_t>(i64_Read));
        const std::size_t ui64_EndlPos = sv_Chunk.find('\n');
        if (std::string_view::npos != ui64_EndlPos) {
            str_Line.append(sv_Chunk.substr(0U, ui64_EndlPos));
            break;
        }
        str_Line.append(sv_Chunk);
    }

    while (!str_Line.empty() && '\r' == str_Line.back()) {
        str_Line.pop_back();
    }
    rstr_Dest = std::move(str_Line);
    return true;
}

/**
 * @brief Read the entire contents of a file into a single string.
 *
 * @param rstr_Dest - Destination string updated on success (untouched on error)
 * @param krstr_Path - Path to the file to read
 *
 * @return true if the file was opened and read successfully, false otherwise
 */
bool fb_ReadAll(std::string& rstr_Dest,
                const std::string& krstr_Path) {
    CUniqueFd fd(::open(krstr_Path.c_str(), O_RDONLY | O_CLOEXEC));
    if (!fd) {
        return false;
    }

    char ac_Buf[BufferSizes::kui64_ReadBuf];
    std::string str_Content;
    while (true) {
        const ssize_t i64_Read = ::read(fd.fi32_Get(), ac_Buf, sizeof(ac_Buf));
        if (0 > i64_Read) {
            if (EINTR == errno) {
                continue;
            }
            return false;
        }
        if (0 == i64_Read) {
            break;
        }
        str_Content.append(ac_Buf, static_cast<std::size_t>(i64_Read));
    }
    rstr_Dest = std::move(str_Content);
    return true;
}

/**
 * @brief Read all lines of a file into a vector of strings.
 *
 * @param rvecstr_Dest - Destination vector updated on success (untouched on error)
 * @param krstr_Path - Path to the file to read
 *
 * @return true if the file was opened and read successfully, false otherwise
 */
bool fb_ReadLines(std::vector<std::string>& rvecstr_Dest,
                  const std::string& krstr_Path) {
    std::ifstream ifs(krstr_Path);
    if (!ifs.is_open()) {
        return false;
    }
    std::vector<std::string> vecstr_Lines;
    std::string str_Line;
    while (std::getline(ifs, str_Line)) {
        while (!str_Line.empty() && '\r' == str_Line.back()) {
            str_Line.pop_back();
        }
        vecstr_Lines.emplace_back(std::move(str_Line));
    }
    rvecstr_Dest = std::move(vecstr_Lines);
    return true;
}

/**
 * @brief Find up to ui64_MaxCount lines containing a given substring.
 *
 * @param krstr_Path - Path to the file to search
 * @param sv_Substr - Substring to match in each line
 * @param ui64_MaxCount - Maximum number of matching lines to return (defaults to 1)
 *
 * @return Vector of matching lines (empty if file missing or no matches)
 */
std::vector<std::string> fvecstr_FindLines(const std::string& krstr_Path,
                                           std::string_view sv_Substr,
                                           std::size_t ui64_MaxCount) {
    std::vector<std::string> vecstr_Matches;
    if (0U == ui64_MaxCount) {
        return vecstr_Matches;
    }
    std::ifstream ifs(krstr_Path);
    if (!ifs.is_open()) {
        return vecstr_Matches;
    }
    std::string str_Line;
    while (std::getline(ifs, str_Line)) {
        // if (!str_Line.empty() && '\r' == str_Line.back()) {
        //     str_Line.pop_back();
        // }
        if (std::string::npos != str_Line.find(sv_Substr)) {
            vecstr_Matches.emplace_back(std::move(str_Line));
            if (ui64_MaxCount <= vecstr_Matches.size()) {
                break;
            }
        }
    }
    return vecstr_Matches;
}

/**
 * @brief Count lines in a file that contain a given substring.
 *
 * @param rui32_Count - Output line count updated on success (untouched on error)
 * @param krstr_Path - Path to the file to scan
 * @param sv_Substr - Substring to search for on each line
 *
 * @return true if the file was opened and scanned successfully, false otherwise
 */
bool fb_CountLinesContaining(uint32_t& rui32_Count,
                             const std::string& krstr_Path,
                             std::string_view sv_Substr) {
    std::ifstream ifs(krstr_Path);
    if (!ifs.is_open()) {
        return false;
    }
    uint32_t ui32_Count = 0U;
    std::string str_Line;
    while (std::getline(ifs, str_Line)) {
        if (std::string::npos != str_Line.find(sv_Substr)) {
            ++ui32_Count;
        }
    }
    rui32_Count = ui32_Count;
    return true;
}

/**
 * @brief Parse an unsigned 64-bit integer from the first line of a file.
 *
 * Supports optional "0x"/"0X" prefix when i32_Base == NumericBase::ki32_Hex.
 *
 * @param rui64_Out - Output integer updated on success (untouched on error)
 * @param krstr_Path - Path to the file to read
 * @param i32_Base - Numeric base for parsing (defaults to NumericBase::ki32_Decimal)
 *
 * @return true if a valid integer was parsed, false otherwise
 */
bool fb_ReadUint64(uint64_t& rui64_Out,
                   const std::string& krstr_Path,
                   int i32_Base) {
    std::string str_Line;
    if (!fb_ReadFirstLine(str_Line, krstr_Path)) {
        return false;
    }
    StringHandler::fv_Trim(str_Line);
    std::string_view sv_Val(str_Line);
    if (NumericBase::ki32_Hex == i32_Base) {
        if (sv_Val.starts_with(ksv_HexPrefixLower) || sv_Val.starts_with(ksv_HexPrefixUpper)) {
            sv_Val.remove_prefix(ksv_HexPrefixLower.size());
        }
    }
    if (sv_Val.empty()) {
        return false;
    }
    uint64_t ui64_Val = 0ULL;
    const char* kpc_Begin = sv_Val.data();
    const char* kpc_End = sv_Val.data() + sv_Val.size();
    const auto [p_Ptr, errCode] = std::from_chars(kpc_Begin, kpc_End, ui64_Val, i32_Base);
    if (std::errc{} != errCode || kpc_Begin == p_Ptr) {
        return false;
    }
    rui64_Out = ui64_Val;
    return true;
}

} // namespace FileReader
