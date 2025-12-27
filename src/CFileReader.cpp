#include "CFileReader.hpp"
#include <fstream>

long CFileReader::fi64_GetLine_nth(std::string& rstr_Dest,
                                   const std::string& krstr_Path,
                                   uint32_t ui32_nth) {
    long i64_Status = EXIT_SUCCESS;
    if (0 < ui32_nth) {
        std::ifstream ifs;
        ifs.open(krstr_Path);
        if (ifs.is_open()) {
            std::string str_Line;
            while (0 < ui32_nth && std::getline(ifs, str_Line)) {
                --ui32_nth;
            }
            rstr_Dest.assign(str_Line);
        } else {
            i64_Status = errno;
        }
        ifs.close();
    } else {
        i64_Status = EINVAL;
    }
    return i64_Status;
}

long CFileReader::fi64_GetContent(std::vector<std::string>& rvecstr_Dest,
                                  const std::string& krstr_Path) {
    long i64_Status = EXIT_SUCCESS;
    std::ifstream ifs;
    ifs.open(krstr_Path);
    if (ifs.is_open()) {
        std::string str_Line;
        // Keep output argument to be untouched if file is empty
        if (std::getline(ifs, str_Line)) {
            rvecstr_Dest.clear();
            rvecstr_Dest.emplace_back(str_Line);
        }
        while (std::getline(ifs, str_Line)) {
            rvecstr_Dest.emplace_back(str_Line);
        }
    } else {
        i64_Status = errno;
    }
    ifs.close();
    return i64_Status;
}

long CFileReader::fi64_FindLine(std::vector<std::string>& rvecstr_Dest,
                                const std::string& krstr_Path,
                                const std::string& krstr_Substr,
                                uint32_t ui32_Quantity) {
    long i64_Status = EXIT_SUCCESS;
    std::ifstream ifs;
    ifs.open(krstr_Path);
    if (ifs.is_open()) {
        std::string str_Line;
        // Keep output argument to be untouched if file is empty
        while (std::getline(ifs, str_Line)) {
            if (std::string::npos != str_Line.find(krstr_Substr)) {
                rvecstr_Dest.clear();
                rvecstr_Dest.emplace_back(str_Line);
                --ui32_Quantity;
            }
            if (0 == ui32_Quantity) {
                break;
            }
        }
        while (std::getline(ifs, str_Line)) {
            if (std::string::npos != str_Line.find(krstr_Substr)) {
                rvecstr_Dest.emplace_back(str_Line);
            }
        }
    } else {
        i64_Status = errno;
    }
    ifs.close();
    return i64_Status;
}


long CFileReader::fi64_CountLine(uint32_t& rui64_Count,
                                 const std::string& krstr_Path,
                                 const std::string& krstr_Substr) {
    long i64_Status = EXIT_SUCCESS;
    std::ifstream ifs;
    ifs.open(krstr_Path);
    if (ifs.is_open()) {
        rui64_Count = 0U;
        std::string str_Line;
        while (std::getline(ifs, str_Line)) {
            if (std::string::npos != str_Line.find(krstr_Substr)) {
                ++rui64_Count;
            }
        }
    } else {
        i64_Status = errno;
    }
    ifs.close();
    return i64_Status;
}