// CSTR_parse_tests.cpp --- The tests for CSTR_parse
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#include "CSTR_parse.h"
#include <cassert>

int main(void)
{
    using namespace khmz;
#ifdef UTFIO_SUPPORT_ANSI
    {
        int flags = 0;
        std::string output;
        bool ret = CSTR_parse_ansi(output, "\"Test\\t<>\\n()\"", flags);
        assert(ret);
        std::printf("%s\n", output.c_str());
        ret = ret;
    }
#endif
    {
        int flags = 0;
        std::string output;
        bool ret = CSTR_parse_u8(output, u8"\"Test\\t<>\\n()\"", flags);
        assert(ret);
        std::printf("%s\n", output.c_str());
        ret = ret;
    }
    {
        int flags = 0;
        std::u16string output;
        bool ret = CSTR_parse_u16(output, u"\"Test\\t<>\\n()\"", flags);
        assert(ret);
        std::string u8str;
        ret = u8_from_u16(u8str, output);
        assert(ret);
        std::printf("%s\n", u8str.c_str());
        ret = ret;
    }
    {
        int flags = 0;
        std::u32string output;
        bool ret = CSTR_parse_u32(output, U"\"Test\\t<>\\n()\"", flags);
        assert(ret);
        std::string u8str;
        ret = u8_from_u32(u8str, output);
        assert(ret);
        std::printf("%s\n", u8str.c_str());
        ret = ret;
    }
    std::puts("end");
    return 0;
}
