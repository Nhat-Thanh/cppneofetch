#ifndef CPPNEOFETCH_FILE_READER_HPP
#define CPPNEOFETCH_FILE_READER_HPP

#include "utils/Definitions.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Utility functions for reading and parsing system, procfs, and sysfs files.
 */
namespace FileReader {

/**
 * @brief Read the first line of a file, stripping trailing CR/LF characters.
 *
 * @param rstr_Dest - Destination string updated on success (untouched on error)
 * @param krstr_Path - Path to the file to read
 *
 * @return true if the file was opened and read successfully, false otherwise
 */
bool fb_ReadFirstLine(std::string& rstr_Dest,
                      const std::string& krstr_Path);

/**
 * @brief Read the entire contents of a file into a single string.
 *
 * @param rstr_Dest - Destination string updated on success (untouched on error)
 * @param krstr_Path - Path to the file to read
 *
 * @return true if the file was opened and read successfully, false otherwise
 */
bool fb_ReadAll(std::string& rstr_Dest,
                const std::string& krstr_Path);

/**
 * @brief Read all lines of a file into a vector of strings.
 *
 * @param rvecstr_Dest - Destination vector updated on success (untouched on error)
 * @param krstr_Path - Path to the file to read
 *
 * @return true if the file was opened and read successfully, false otherwise
 */
bool fb_ReadLines(std::vector<std::string>& rvecstr_Dest,
                  const std::string& krstr_Path);

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
                                           std::size_t ui64_MaxCount = 1U);

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
                             std::string_view sv_Substr);

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
                   int i32_Base = NumericBase::ki32_Decimal);

} // namespace FileReader

#endif // CPPNEOFETCH_FILE_READER_HPP
