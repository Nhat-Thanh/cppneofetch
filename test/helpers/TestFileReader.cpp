#include "TestCommon.hpp"
#include "helpers/FileReader.hpp"
#include <iostream>

void fv_RunTestFileReader() {
    // Temp directory created for this suite and released automatically on scope exit
    CScopedTempDir tempDir("filereader_");

    // 1. fb_ReadFirstLine: normal, CRLF, no trailing newline, empty, >4096 bytes, missing, directory
    {
        const std::string str_Multi = tempDir.fstr_WriteFile("multi.txt", "first line\nsecond line\n");
        std::string str_Out = "sentinel";
        TEST_CHECK(FileReader::fb_ReadFirstLine(str_Out, str_Multi));
        TEST_CHECK_EQ("first line", str_Out);

        const std::string str_Crlf = tempDir.fstr_WriteFile("crlf.txt", "windows line\r\nnext\r\n");
        TEST_CHECK(FileReader::fb_ReadFirstLine(str_Out, str_Crlf));
        TEST_CHECK_EQ("windows line", str_Out);

        const std::string str_NoNl = tempDir.fstr_WriteFile("nonl.txt", "single line");
        TEST_CHECK(FileReader::fb_ReadFirstLine(str_Out, str_NoNl));
        TEST_CHECK_EQ("single line", str_Out);

        const std::string str_Empty = tempDir.fstr_WriteFile("empty.txt", "");
        str_Out = "nonempty";
        TEST_CHECK(FileReader::fb_ReadFirstLine(str_Out, str_Empty));
        TEST_CHECK(str_Out.empty());

        const std::string str_LongLine(5000U, 'Z');
        const std::string str_LongFile = tempDir.fstr_WriteFile("longline.txt", str_LongLine + "\nsecond");
        TEST_CHECK(FileReader::fb_ReadFirstLine(str_Out, str_LongFile));
        TEST_CHECK_EQ(str_LongLine, str_Out);

        // Error cases must leave rstr_Dest untouched
        str_Out = "keep_me";
        TEST_CHECK(!FileReader::fb_ReadFirstLine(str_Out, (tempDir.fkr_Path() / "missing.txt").string()));
        TEST_CHECK_EQ("keep_me", str_Out);

        TEST_CHECK(!FileReader::fb_ReadFirstLine(str_Out, tempDir.fkr_Path().string()));
        TEST_CHECK_EQ("keep_me", str_Out);
    }

    // 2. fb_ReadAll: normal, empty, embedded NULs (/proc cmdline style), >8192 bytes, missing
    {
        const std::string str_NulContent("kitty\0--single-instance\0", 24U);
        const std::string str_Cmdline = tempDir.fstr_WriteFile("cmdline", str_NulContent);
        std::string str_Out;
        TEST_CHECK(FileReader::fb_ReadAll(str_Out, str_Cmdline));
        TEST_CHECK_EQ(str_NulContent, str_Out);

        const std::string str_Large(10000U, 'X');
        const std::string str_LargePath = tempDir.fstr_WriteFile("large.bin", str_Large);
        TEST_CHECK(FileReader::fb_ReadAll(str_Out, str_LargePath));
        TEST_CHECK_EQ(str_Large, str_Out);

        const std::string str_Empty = tempDir.fstr_WriteFile("empty_all.txt", "");
        str_Out = "previous";
        TEST_CHECK(FileReader::fb_ReadAll(str_Out, str_Empty));
        TEST_CHECK(str_Out.empty());

        str_Out = "untouched";
        TEST_CHECK(!FileReader::fb_ReadAll(str_Out, (tempDir.fkr_Path() / "no_such_file").string()));
        TEST_CHECK_EQ("untouched", str_Out);
    }

    // 3. fb_ReadLines: normal, CRLF, blank lines, empty file, missing file
    {
        const std::string str_Path = tempDir.fstr_WriteFile("lines.txt", "one\r\n\ntwo\nthree");
        std::vector<std::string> vec_Lines = { "stale" };
        TEST_CHECK(FileReader::fb_ReadLines(vec_Lines, str_Path));
        TEST_CHECK_EQ(4U, vec_Lines.size());
        TEST_CHECK_EQ("one", vec_Lines[0]);
        TEST_CHECK_EQ("", vec_Lines[1]);
        TEST_CHECK_EQ("two", vec_Lines[2]);
        TEST_CHECK_EQ("three", vec_Lines[3]);

        const std::string str_Empty = tempDir.fstr_WriteFile("empty_lines.txt", "");
        TEST_CHECK(FileReader::fb_ReadLines(vec_Lines, str_Empty));
        TEST_CHECK(vec_Lines.empty());

        vec_Lines = { "keep" };
        TEST_CHECK(!FileReader::fb_ReadLines(vec_Lines, (tempDir.fkr_Path() / "missing_lines.txt").string()));
        TEST_CHECK_EQ(1U, vec_Lines.size());
        TEST_CHECK_EQ("keep", vec_Lines[0]);
    }

    // 4. fvecstr_FindLines: maxCount 0, 1, < matches, > matches, no match, missing
    {
        const std::string str_Path = tempDir.fstr_WriteFile(
            "find.txt", "alpha\r\nbeta key=1\ngamma\nbeta key=2\nbeta key=3\n");

        TEST_CHECK(FileReader::fvecstr_FindLines(str_Path, "beta", 0U).empty());

        const auto vec_One = FileReader::fvecstr_FindLines(str_Path, "beta", 1U);
        TEST_CHECK_EQ(1U, vec_One.size());
        TEST_CHECK_EQ("beta key=1", vec_One[0]);

        const auto vec_Two = FileReader::fvecstr_FindLines(str_Path, "beta", 2U);
        TEST_CHECK_EQ(2U, vec_Two.size());
        TEST_CHECK_EQ("beta key=2", vec_Two[1]);

        const auto vec_All = FileReader::fvecstr_FindLines(str_Path, "beta", 10U);
        TEST_CHECK_EQ(3U, vec_All.size());

        TEST_CHECK(FileReader::fvecstr_FindLines(str_Path, "not_present", 5U).empty());
        TEST_CHECK(FileReader::fvecstr_FindLines((tempDir.fkr_Path() / "missing").string(), "beta", 1U).empty());
    }

    // 5. fb_CountLinesContaining: normal, zero matches, empty file, missing file
    {
        const std::string str_Path = tempDir.fstr_WriteFile(
            "count.txt", "Package: a\nStatus: install ok installed\nPackage: b\nStatus: deinstall\n");
        uint32_t ui32_Count = 999U;
        TEST_CHECK(FileReader::fb_CountLinesContaining(ui32_Count, str_Path, "Status: install ok installed"));
        TEST_CHECK_EQ(1U, ui32_Count);

        TEST_CHECK(FileReader::fb_CountLinesContaining(ui32_Count, str_Path, "Package: "));
        TEST_CHECK_EQ(2U, ui32_Count);

        TEST_CHECK(FileReader::fb_CountLinesContaining(ui32_Count, str_Path, "missing_token"));
        TEST_CHECK_EQ(0U, ui32_Count);

        ui32_Count = 777U;
        TEST_CHECK(!FileReader::fb_CountLinesContaining(ui32_Count, (tempDir.fkr_Path() / "missing").string(), "x"));
        TEST_CHECK_EQ(777U, ui32_Count);
    }

    // 6. fb_ReadUint64: decimal, 0, UINT64_MAX, padded, hex (0x/0X/bare), invalid, overflow, missing
    {
        uint64_t ui64_Val = 12345ULL;

        const std::string str_Dec = tempDir.fstr_WriteFile("u64_dec.txt", "  18446744073709551615 \r\n");
        TEST_CHECK(FileReader::fb_ReadUint64(ui64_Val, str_Dec));
        TEST_CHECK_EQ(UINT64_MAX, ui64_Val);

        const std::string str_Zero = tempDir.fstr_WriteFile("u64_zero.txt", "0\n");
        TEST_CHECK(FileReader::fb_ReadUint64(ui64_Val, str_Zero));
        TEST_CHECK_EQ(0ULL, ui64_Val);

        const std::string str_HexLower = tempDir.fstr_WriteFile("hex_l.txt", "0x030000\n");
        TEST_CHECK(FileReader::fb_ReadUint64(ui64_Val, str_HexLower, NumericBase::ki32_Hex));
        TEST_CHECK_EQ(0x030000ULL, ui64_Val);

        const std::string str_HexUpper = tempDir.fstr_WriteFile("hex_u.txt", "0X10DE\n");
        TEST_CHECK(FileReader::fb_ReadUint64(ui64_Val, str_HexUpper, NumericBase::ki32_Hex));
        TEST_CHECK_EQ(0x10DEULL, ui64_Val);

        const std::string str_HexBare = tempDir.fstr_WriteFile("hex_b.txt", "8086\n");
        TEST_CHECK(FileReader::fb_ReadUint64(ui64_Val, str_HexBare, NumericBase::ki32_Hex));
        TEST_CHECK_EQ(0x8086ULL, ui64_Val);

        ui64_Val = 555ULL;
        const std::string str_Invalid = tempDir.fstr_WriteFile("u64_bad.txt", "not_a_number\n");
        TEST_CHECK(!FileReader::fb_ReadUint64(ui64_Val, str_Invalid));
        TEST_CHECK_EQ(555ULL, ui64_Val);

        const std::string str_EmptyHex = tempDir.fstr_WriteFile("hex_empty.txt", "0x\n");
        TEST_CHECK(!FileReader::fb_ReadUint64(ui64_Val, str_EmptyHex, NumericBase::ki32_Hex));
        TEST_CHECK_EQ(555ULL, ui64_Val);

        const std::string str_Overflow = tempDir.fstr_WriteFile("u64_ovf.txt", "999999999999999999999999\n");
        TEST_CHECK(!FileReader::fb_ReadUint64(ui64_Val, str_Overflow));
        TEST_CHECK_EQ(555ULL, ui64_Val);

        TEST_CHECK(!FileReader::fb_ReadUint64(ui64_Val, (tempDir.fkr_Path() / "missing_num").string()));
        TEST_CHECK_EQ(555ULL, ui64_Val);
    }

    std::cout << "[PASS] TestFileReader\n";
}
