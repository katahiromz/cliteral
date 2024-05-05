// CSTR_parse.h --- Parsing C string literals
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utfio.h"

//////////////////////////////////////////////////////////////////////////////
// Common

namespace khmz
{
    enum CSTR_PARSE
    {
        CSTR_PARSE_SYNTAX_ERROR           = (1 << 0),
        CSTR_PARSE_QUOTE_NOT_FOUND        = (1 << 1),
        CSTR_PARSE_QUOTE_NOT_CLOSED       = (1 << 2),
        CSTR_PARSE_PREFIX_MISMATCH        = (1 << 3),
        CSTR_PARSE_INVALID_ESCAPE         = (1 << 4),
        CSTR_PARSE_NOT_QUOTED             = (1 << 5),
        CSTR_PARSE_INVALID_CHARSET        = (1 << 6),
        CSTR_PARSE_DELIMITER_TOO_LONG     = (1 << 7),
        CSTR_PARSE_RAW_LITERAL_NOT_CLOSED = (1 << 8),
        CSTR_PARSE_ERRORS = (CSTR_PARSE_SYNTAX_ERROR |
                             CSTR_PARSE_QUOTE_NOT_FOUND |
                             CSTR_PARSE_QUOTE_NOT_CLOSED |
                             CSTR_PARSE_PREFIX_MISMATCH |
                             CSTR_PARSE_INVALID_ESCAPE |
                             CSTR_PARSE_NOT_QUOTED |
                             CSTR_PARSE_INVALID_CHARSET |
                             CSTR_PARSE_DELIMITER_TOO_LONG |
                             CSTR_PARSE_RAW_LITERAL_NOT_CLOSED),
        CSTR_PARSE_PREFIX_A               = (1 << 9),
        CSTR_PARSE_PREFIX_L               = (1 << 10),
        CSTR_PARSE_PREFIX_U8              = (1 << 11),
        CSTR_PARSE_PREFIX_U16             = (1 << 12),
        CSTR_PARSE_PREFIX_U32             = (1 << 13),
        CSTR_PARSE_PREFIX_ALL = (CSTR_PARSE_PREFIX_A |
                                 CSTR_PARSE_PREFIX_L |
                                 CSTR_PARSE_PREFIX_U8 |
                                 CSTR_PARSE_PREFIX_U16 |
                                 CSTR_PARSE_PREFIX_U32),
    };
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////

#ifdef CSTR_PARSE_DETAIL
    #include "CSTR_parse_detail.h"
#endif

//////////////////////////////////////////////////////////////////////////////

namespace khmz
{
#ifdef UTFIO_SUPPORT_ANSI
    bool CSTR_parse_ansi(std::string&    output, const char     *input, size_t input_size, int& flags);
#endif
    bool CSTR_parse_u8  (std::string&    output, const char     *input, size_t input_size, int& flags);
    bool CSTR_parse_u16 (std::u16string& output, const char16_t *input, size_t input_size, int& flags);
    bool CSTR_parse_u32 (std::u32string& output, const char32_t *input, size_t input_size, int& flags);
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////

namespace khmz
{
#ifdef UTFIO_SUPPORT_ANSI
    inline bool CSTR_parse_ansi(std::string& output, const std::string& input, int& flags)
    {
        return CSTR_parse_ansi(output, input.data(), input.size(), flags);
    }
#endif
    inline bool CSTR_parse_u8(std::string& output, const std::string& input, int& flags)
    {
        return CSTR_parse_u8(output, input.data(), input.size(), flags);
    }
    inline bool CSTR_parse_u16(std::u16string& output, const std::u16string& input, int& flags)
    {
        return CSTR_parse_u16(output, input.data(), input.size(), flags);
    }
    inline bool CSTR_parse_u32(std::u32string& output, const std::u32string& input, int& flags)
    {
        return CSTR_parse_u32(output, input.data(), input.size(), flags);
    }
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////
// Wide

#ifdef _WIN32
namespace khmz
{
    inline bool CSTR_parse_wide(std::wstring& output, const wchar_t *input, size_t input_length, int& flags)
    {
        std::u16string u16str;
        bool ret = CSTR_parse_u16(u16str, reinterpret_cast<const char16_t*>(input), input_length, flags);
        output.assign(reinterpret_cast<const wchar_t*>(u16str.data()), u16str.size());
        return ret;
    }
    inline bool CSTR_parse_wide(std::wstring& output, const std::wstring& input, int& flags)
    {
        return CSTR_parse_wide(output, input.data(), input.size(), flags);
    }

    #ifdef _UNICODE
        #define CSTR_parse_text CSTR_parse_wide
    #else
        #define CSTR_parse_text CSTR_parse_ansi
    #endif
} // namespace khmz
#endif

//////////////////////////////////////////////////////////////////////////////
