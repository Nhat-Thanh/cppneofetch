#ifndef _FILEREADER_HPP_
#define _FILEREADER_HPP_

#include <vector>
#include <string>
#include <cstdint>

/**
 * @brief The class handler of reading file, all methods are static ones
 */
class CFileReader {
public:
    CFileReader() = delete;
    virtual ~CFileReader() = default;

    /**
     * @brief Read and get the nth line in the file
     *
     * @param rstr_Dest - The destination of output, keep untouched if errors occur
     * @param krstr_Path - The path of the file
     * @param ui32_nth - The target line
     *
     * @return A long type status code - EXIT_SUCESS if no errors or errno otherwise
     */
    static long fi64_GetLine_nth(std::string& rstr_Dest,
                                 const std::string& krstr_Path,
                                 uint32_t ui32_nth = 1UL);

    /**
     * @brief Read and get all lines in the file
     *
     * @param rvecstr_Dest - The destination of output, keep untouched if errors occur
     * @param krstr_Path - The path of the file
     *
     * @return A long type status code - EXIT_SUCESS if no errors or errno otherwise
     */
    static long fi64_GetContent(std::vector<std::string>& rvecstr_Dest,
                                const std::string& krstr_Path);

    /**
     * @brief Read and find particular quantity of lines have the substring
     *
     * Read and get equal or less than a particular quantity of lines have the substring
     *
     * @param rvecstr_Dest - The destination of output, keep untouched if errors occur
     * @param krstr_Path - The path of the file
     * @param krstr_Substr - The substring
     * @param ui32_Quantity  - The quantity
     *
     * @return A long type status code - EXIT_SUCESS if no errors or errno otherwise
     */
    static long fi64_FindLine(std::vector<std::string>& rvecstr_Dest,
                              const std::string& krstr_Path,
                              const std::string& krstr_Substr,
                              uint32_t ui32_Quantity = 1U);

    /**
     * @brief Read and count lines have the substring in the file
     *
     * @param rui32_Count - The destination of output, keep untouched if errors occur
     * @param krstr_Path - The path of the file
     * @param krstr_Substr - The substring
     *
     * @return A long type status code - EXIT_SUCESS if no errors or errno otherwise
     */
    static long fi64_CountLine(uint32_t& rui32_Count,
                               const std::string& krstr_Path,
                               const std::string& krstr_Substr);
};

#endif
