#include "TestCommon.hpp"
#include "helpers/StringHandler.hpp"
#include <iostream>

void fv_RunTestStringHandler() {
    // 1. fv_LeftTrim: normal, custom chars, already trimmed, empty, all-matching
    {
        std::string str_Val = "  \t\r\nhello  ";
        StringHandler::fv_LeftTrim(str_Val);
        TEST_CHECK_EQ("hello  ", str_Val);

        str_Val = "///usr/bin";
        StringHandler::fv_LeftTrim(str_Val, "/");
        TEST_CHECK_EQ("usr/bin", str_Val);

        str_Val = "abc";
        StringHandler::fv_LeftTrim(str_Val, "xyz");
        TEST_CHECK_EQ("abc", str_Val);

        str_Val = "";
        StringHandler::fv_LeftTrim(str_Val);
        TEST_CHECK(str_Val.empty());

        str_Val = "xxxx";
        StringHandler::fv_LeftTrim(str_Val, "x");
        TEST_CHECK(str_Val.empty());
    }

    // 2. fv_RightTrim: normal, custom chars, already trimmed, empty, all-matching
    {
        std::string str_Val = "  hello \t\r\n";
        StringHandler::fv_RightTrim(str_Val);
        TEST_CHECK_EQ("  hello", str_Val);

        str_Val = "gnome-terminal---";
        StringHandler::fv_RightTrim(str_Val, "-");
        TEST_CHECK_EQ("gnome-terminal", str_Val);

        str_Val = "abc";
        StringHandler::fv_RightTrim(str_Val, "xyz");
        TEST_CHECK_EQ("abc", str_Val);

        str_Val = "";
        StringHandler::fv_RightTrim(str_Val);
        TEST_CHECK(str_Val.empty());

        str_Val = "yyyy";
        StringHandler::fv_RightTrim(str_Val, "y");
        TEST_CHECK(str_Val.empty());
    }

    // 3. fv_Trim: normal, internal whitespace preserved, all-matching, empty
    {
        std::string str_Val = " \t hello world \r\n ";
        StringHandler::fv_Trim(str_Val);
        TEST_CHECK_EQ("hello world", str_Val);

        str_Val = "   \t\n";
        StringHandler::fv_Trim(str_Val);
        TEST_CHECK(str_Val.empty());

        str_Val = "";
        StringHandler::fv_Trim(str_Val);
        TEST_CHECK(str_Val.empty());
    }

    // 4. fstr_ReplaceAll: normal, no match, empty src, empty from, empty to, self-overlapping
    {
        TEST_CHECK_EQ("baz bar baz", StringHandler::fstr_ReplaceAll("foo bar foo", "foo", "baz"));
        TEST_CHECK_EQ("hello", StringHandler::fstr_ReplaceAll("hello", "xyz", "123"));
        TEST_CHECK_EQ("", StringHandler::fstr_ReplaceAll("", "foo", "bar"));
        TEST_CHECK_EQ("abc", StringHandler::fstr_ReplaceAll("abc", "", "x"));
        TEST_CHECK_EQ("Intel Core(TM)", StringHandler::fstr_ReplaceAll("Intel(R) Core(TM)", "(R)", ""));
        TEST_CHECK_EQ("ba", StringHandler::fstr_ReplaceAll("aaa", "aa", "b"));
        TEST_CHECK_EQ("aaaa", StringHandler::fstr_ReplaceAll("aa", "a", "aa"));
    }

    // 5. fstr_GetSubstringBetween: normal, nested, multiple pairs, unmatched, empty, custom delims
    {
        TEST_CHECK_EQ(
            "GeForce RTX 2060",
            StringHandler::fstr_GetSubstringBetween("NVIDIA TU106 [GeForce RTX 2060]"));
        TEST_CHECK_EQ(
            "Outer [Inner]",
            StringHandler::fstr_GetSubstringBetween("Device [Outer [Inner]]"));
        TEST_CHECK_EQ(
            "First] middle [Second",
            StringHandler::fstr_GetSubstringBetween("[First] middle [Second]"));
        TEST_CHECK_EQ("", StringHandler::fstr_GetSubstringBetween("Empty []"));
        TEST_CHECK_EQ("", StringHandler::fstr_GetSubstringBetween("Unclosed [abc"));
        TEST_CHECK_EQ("", StringHandler::fstr_GetSubstringBetween("Unopened abc]"));
        TEST_CHECK_EQ("", StringHandler::fstr_GetSubstringBetween("Reversed ]abc["));
        TEST_CHECK_EQ("", StringHandler::fstr_GetSubstringBetween("No brackets"));
        TEST_CHECK_EQ("", StringHandler::fstr_GetSubstringBetween(""));
        TEST_CHECK_EQ("16", StringHandler::fstr_GetSubstringBetween("CPU (16)", '(', ')'));
    }

    // 6. fstr_Format: normal, empty format, nullptr format, large (>4096) string, percent literal
    {
        const char* kpc_EmptyFmt = "";
        TEST_CHECK_EQ("arch-42-12.35%", StringHandler::fstr_Format("%s-%d-%.2f%%", "arch", 42, 12.345));
        TEST_CHECK_EQ("", StringHandler::fstr_Format(kpc_EmptyFmt));
        TEST_CHECK_EQ("", StringHandler::fstr_Format(nullptr));

        const std::string str_Large(6000U, 'A');
        const std::string str_Formatted = StringHandler::fstr_Format("%s!", str_Large.c_str());
        TEST_CHECK_EQ(6001U, str_Formatted.size());
        TEST_CHECK_EQ('!', str_Formatted.back());
    }

    // 7. fui64_Utf8DisplayWidth: empty, ASCII, 3-byte UTF-8 box chars, 4-byte emoji
    {
        TEST_CHECK_EQ(0U, StringHandler::fui64_Utf8DisplayWidth(""));
        TEST_CHECK_EQ(10U, StringHandler::fui64_Utf8DisplayWidth("Arch Linux"));
        TEST_CHECK_EQ(4U, StringHandler::fui64_Utf8DisplayWidth("▄█▟ "));
        TEST_CHECK_EQ(2U, StringHandler::fui64_Utf8DisplayWidth("🚀a"));
    }

    std::cout << "[PASS] TestStringHandler\n";
}
