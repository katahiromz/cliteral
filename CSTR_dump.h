// CSTR_dump.h --- C string literal dumper
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utfio.h"

//////////////////////////////////////////////////////////////////////////////

namespace khmz
{
    enum CSTR_DUMP
    {
        CSTR_DUMP_NUL           = (1 << 0),
        CSTR_DUMP_ESCAPE_NOCONV = (1 << 1),
        CSTR_DUMP_ESCAPE_OCTAL  = (1 << 2),
        CSTR_DUMP_ESCAPE_U16    = (1 << 3),
        CSTR_DUMP_ESCAPE_U32    = (1 << 4),
        CSTR_DUMP_PREFIX_L      = (1 << 5),
        CSTR_DUMP_PREFIX_U8     = (1 << 6),
        CSTR_DUMP_PREFIX_U16    = (1 << 7),
        CSTR_DUMP_PREFIX_U32    = (1 << 8),
        CSTR_DUMP_PREFIX_ALL = (CSTR_DUMP_PREFIX_L |
                                CSTR_DUMP_PREFIX_U8 |
                                CSTR_DUMP_PREFIX_U16 |
                                CSTR_DUMP_PREFIX_U32),
    };
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////

#ifdef CSTR_DUMP_DETAIL
    #include "CSTR_dump_detail.h"
#endif

//////////////////////////////////////////////////////////////////////////////

namespace khmz
{
#ifdef UTFIO_SUPPORT_ANSI
    bool CSTR_dump_ansi(std::   string& output, const char     *input, size_t input_length, int& flags);
#endif
    bool CSTR_dump_u8  (std::   string& output, const char     *input, size_t input_length, int& flags);
    bool CSTR_dump_u16 (std::u16string& output, const char16_t *input, size_t input_length, int& flags);
    bool CSTR_dump_u32 (std::u32string& output, const char32_t *input, size_t input_length, int& flags);
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////

namespace khmz
{
#ifdef UTFIO_SUPPORT_ANSI
    inline bool CSTR_dump_ansi(std::string& output, const std::string   & input, int& flags)
    {
        return CSTR_dump_ansi(output, input.data(), input.size(), flags);
    }
#endif
    inline bool CSTR_dump_u8(std::string& output, const std::string   & input, int& flags)
    {
        return CSTR_dump_u8(output, input.data(), input.size(), flags);
    }
    inline bool CSTR_dump_u16(std::u16string& output, const std::u16string& input, int& flags)
    {
        return CSTR_dump_u16(output, input.data(), input.size(), flags);
    }
    inline bool CSTR_dump_u32(std::u32string& output, const std::u32string& input, int& flags)
    {
        return CSTR_dump_u32(output, input.data(), input.size(), flags);
    }
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
namespace khmz
{
    inline bool CSTR_dump_wide(std::wstring& output, const wchar_t *input, size_t input_length, int& flags)
    {
        std::u16string u16str;
        bool ret = CSTR_dump_u16(u16str, reinterpret_cast<const char16_t*>(input), input_length, flags);
        output.assign(reinterpret_cast<const wchar_t*>(u16str.data()), u16str.size());
        return ret;
    }
    inline bool CSTR_dump_wide(std::wstring& output, const std::wstring& input, int& flags)
    {
        return CSTR_dump_wide(output, input.data(), input.size(), flags);
    }

    #ifdef _UNICODE
        #define CSTR_dump_text CSTR_dump_wide
    #else
        #define CSTR_dump_text CSTR_dump_ansi
    #endif
} // namespace khmz
#endif

//////////////////////////////////////////////////////////////////////////////
