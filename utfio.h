// utfio.h --- The UTF IO library
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>

#ifdef _WIN32
    #ifndef _INC_WINDOWS
        #include <windows.h>
    #endif
#endif

//////////////////////////////////////////////////////////////////////////////
// Common

#ifndef UTFIO_INVALID_CHAR
    #define UTFIO_INVALID_CHAR '?'
#endif

#ifndef UTFIO_THROW
    #define UTFIO_THROW() assert(0)
#endif

#ifndef UTFIO_CODEPAGE
    #define UTFIO_CODEPAGE 0
#endif

#ifdef _WIN32
    #ifndef UTFIO_SUPPORT_ANSI
        #define UTFIO_SUPPORT_ANSI
    #endif
#endif

namespace khmz
{
#ifdef _WIN32
    #ifdef _UNICODE
        using tstring = std::wstring;
    #else
        using tstring = std::string;
    #endif

    #ifdef _UNICODE
        template <typename T_ITEM>
        inline tstring to_tstring(const T_ITEM& item)
        {
            return std::to_wstring(item);
        }
    #else
        template <typename T_ITEM>
        inline tstring to_tstring(const T_ITEM& item)
        {
            return std::to_string(item);
        }
    #endif
#endif

    enum BOM
    {
        BOM_NO_BOM = 0,
        BOM_UTF8,
        BOM_UTF16LE,
        BOM_UTF16BE,
        BOM_UTF32LE,
        BOM_UTF32BE,
        BOM_UTF7,
    };

    inline bool write_bom(FILE *fp, BOM bom)
    {
        switch (bom)
        {
        default:
        case BOM_NO_BOM:    return true;
        case BOM_UTF8:      return !!std::fwrite("\xEF\xBB\xBF", 3, 1, fp);
        case BOM_UTF16LE:   return !!std::fwrite("\xFF\xFE",     2, 1, fp);
        case BOM_UTF16BE:   return !!std::fwrite("\xFE\xFF",     2, 1, fp);
        case BOM_UTF32LE:   return !!std::fwrite("\xFF\xFE\0\0", 4, 1, fp);
        case BOM_UTF32BE:   return !!std::fwrite("\0\0\xFE\xFF", 4, 1, fp);
        case BOM_UTF7:      return !!std::fwrite("\x2B\x2F\x76", 3, 1, fp);
        }
    }

    inline BOM detect_bom(FILE *fp)
    {
        char buf[4];
        size_t len = std::fread(buf, 1, sizeof(buf), fp);
        if (len >= 3 && memcmp(buf, "\xEF\xBB\xBF", 3) == 0)
        {
            std::fseek(fp, -(long)(len - 3), SEEK_CUR);
            return BOM_UTF8;
        }
        if (len >= 2 && memcmp(buf, "\xFF\xFE", 2) == 0)
        {
            std::fseek(fp, -(long)(len - 2), SEEK_CUR);
            return BOM_UTF16LE;
        }
        if (len >= 2 && memcmp(buf, "\xFE\xFF", 2) == 0)
        {
            std::fseek(fp, -(long)(len - 2), SEEK_CUR);
            return BOM_UTF16BE;
        }
        if (len >= 4 && memcmp(buf, "\xFF\xFE\0\0", 4) == 0)
        {
            std::fseek(fp, -(long)(len - 4), SEEK_CUR);
            return BOM_UTF16LE;
        }
        if (len >= 4 && memcmp(buf, "\0\0\xFE\xFF", 4) == 0)
        {
            std::fseek(fp, -(long)(len - 4), SEEK_CUR);
            return BOM_UTF16BE;
        }
        if (len >= 3 && memcmp(buf, "\x2B\x2F\x76", 3) == 0)
        {
            std::fseek(fp, -(long)(len - 3), SEEK_CUR);
            return BOM_UTF7;
        }
        std::fseek(fp, -(long)len, SEEK_CUR);
        return BOM_NO_BOM;
    }

    inline uint16_t swap_bytes(uint16_t value)
    {
        uint32_t lo = uint8_t(value & 0xFF);
        uint32_t hi = uint8_t(value >> 8);
        return uint16_t(hi | (lo << 8));
    }
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////

#ifdef UTFIO_CLASSES
    #include "utfio_classes.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// Converting

namespace khmz
{
#ifdef UTFIO_SUPPORT_ANSI
    bool ansi_from_ansi(std::string&       output, const char     *input, size_t input_length);
    bool ansi_from_u8  (std::string&       output, const char     *input, size_t input_length);
    bool ansi_from_u16 (std::string&       output, const char16_t *input, size_t input_length);
    bool ansi_from_u32 (std::string&       output, const char32_t *input, size_t input_length);
#endif

#ifdef UTFIO_SUPPORT_ANSI
    bool u8_from_ansi  (std::string&       output, const char     *input, size_t input_length);
#endif
    bool u8_from_u8    (std::string&       output, const char     *input, size_t input_length);
    bool u8_from_u16   (std::string&       output, const char16_t *input, size_t input_length);
    bool u8_from_u32   (std::string&       output, const char32_t *input, size_t input_length);

#ifdef UTFIO_SUPPORT_ANSI
    bool u16_from_ansi (std::u16string&    output, const char     *input, size_t input_length);
#endif
    bool u16_from_u8   (std::u16string&    output, const char     *input, size_t input_length);
    bool u16_from_u16  (std::u16string&    output, const char16_t *input, size_t input_length);
    bool u16_from_u32  (std::u16string&    output, const char32_t *input, size_t input_length);

#ifdef UTFIO_SUPPORT_ANSI
    bool u32_from_ansi (std::u32string&    output, const char     *input, size_t input_length);
#endif
    bool u32_from_u8   (std::u32string&    output, const char     *input, size_t input_length);
    bool u32_from_u16  (std::u32string&    output, const char16_t *input, size_t input_length);
    bool u32_from_u32  (std::u32string&    output, const char32_t *input, size_t input_length);
} //namespace khmz

//////////////////////////////////////////////////////////////////////////////
// Converting 2

namespace khmz
{
#ifdef UTFIO_SUPPORT_ANSI
    inline bool ansi_from_ansi(std::string&       output, const std::string& input)
    {
        return ansi_from_ansi(output, input.data(), input.size());
    }
    inline bool ansi_from_u8  (std::string&       output, const std::string& input)
    {
        return ansi_from_u8(output, input.data(), input.size());
    }
    inline bool ansi_from_u16 (std::string&       output, const std::u16string& input)
    {
        return ansi_from_u16(output, input.data(), input.size());
    }
    inline bool ansi_from_u32 (std::string&       output, const std::u32string& input)
    {
        return ansi_from_u32(output, input.data(), input.size());
    }
#endif

#ifdef UTFIO_SUPPORT_ANSI
    inline bool u8_from_ansi  (std::string&       output, const std::string& input)
    {
        return u8_from_ansi(output, input.data(), input.size());
    }
#endif
    inline bool u8_from_u8    (std::string&       output, const std::string& input)
    {
        return u8_from_u8(output, input.data(), input.size());
    }
    inline bool u8_from_u16   (std::string&       output, const std::u16string& input)
    {
        return u8_from_u16(output, input.data(), input.size());
    }
    inline bool u8_from_u32   (std::string&       output, const std::u32string& input)
    {
        return u8_from_u32(output, input.data(), input.size());
    }

#ifdef UTFIO_SUPPORT_ANSI
    inline bool u16_from_ansi (std::u16string&    output, const std::string& input)
    {
        return u16_from_ansi(output, input.data(), input.size());
    }
#endif
    inline bool u16_from_u8   (std::u16string&    output, const std::string& input)
    {
        return u16_from_u8(output, input.data(), input.size());
    }
    inline bool u16_from_u16  (std::u16string&    output, const std::u16string& input)
    {
        return u16_from_u16(output, input.data(), input.size());
    }
    inline bool u16_from_u32  (std::u16string&    output, const std::u32string& input)
    {
        return u16_from_u32(output, input.data(), input.size());
    }

#ifdef UTFIO_SUPPORT_ANSI
    inline bool u32_from_ansi (std::u32string&    output, const std::string& input)
    {
        return u32_from_ansi(output, input.data(), input.size());
    }
#endif
    inline bool u32_from_u8   (std::u32string&    output, const std::string& input)
    {
        return u32_from_u8(output, input.data(), input.size());
    }
    inline bool u32_from_u16  (std::u32string&    output, const std::u16string& input)
    {
        return u32_from_u16(output, input.data(), input.size());
    }
    inline bool u32_from_u32  (std::u32string&    output, const std::u32string& input)
    {
        return u32_from_u32(output, input.data(), input.size());
    }
} //namespace khmz

//////////////////////////////////////////////////////////////////////////////
// Wide

#ifdef _WIN32
namespace khmz
{
    inline bool wide_from_ansi(std::wstring& output, const char *input, size_t input_length)
    {
        std::u16string u16;
        bool ret = u16_from_ansi(u16, input, input_length);
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }
    inline bool wide_from_u8(std::wstring& output, const char *input, size_t input_length)
    {
        std::u16string u16;
        bool ret = u16_from_u8(u16, input, input_length);
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }
    inline bool wide_from_u16(std::wstring& output, const char16_t *input, size_t input_length)
    {
        std::u16string u16;
        bool ret = u16_from_u16(u16, input, input_length);
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }
    inline bool wide_from_u32(std::wstring& output, const char32_t *input, size_t input_length)
    {
        std::u16string u16;
        bool ret = u16_from_u32(u16, input, input_length);
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }

    inline bool ansi_from_wide(std::string& output, const wchar_t *input, size_t input_length)
    {
        return ansi_from_u16(output, reinterpret_cast<const char16_t*>(input), input_length);
    }
    inline bool u8_from_wide(std::string& output, const wchar_t *input, size_t input_length)
    {
        return u8_from_u16(output, reinterpret_cast<const char16_t*>(input), input_length);
    }
    inline bool u16_from_wide(std::u16string& output, const wchar_t *input, size_t input_length)
    {
        return u16_from_u16(output, reinterpret_cast<const char16_t*>(input), input_length);
    }
    inline bool u32_from_wide(std::u32string& output, const wchar_t *input, size_t input_length)
    {
        return u32_from_u16(output, reinterpret_cast<const char16_t*>(input), input_length);
    }

    inline bool wide_from_wide(std::wstring& output, const wchar_t *input, size_t input_length)
    {
        std::u16string u16;
        bool ret = u16_from_u16(u16, reinterpret_cast<const char16_t*>(input), input_length);
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }

    inline bool wide_from_ansi(std::wstring& output, const std::string& input)
    {
        std::u16string u16;
        bool ret = u16_from_ansi(u16, input.data(), input.size());
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }
    inline bool wide_from_u8(std::wstring& output, const std::string& input)
    {
        std::u16string u16;
        bool ret = u16_from_u8(u16, input.data(), input.size());
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }
    inline bool wide_from_u16(std::wstring& output, const std::u16string& input)
    {
        std::u16string u16;
        bool ret = u16_from_u16(u16, input.data(), input.size());
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }
    inline bool wide_from_u32(std::wstring& output, const std::u32string& input)
    {
        std::u16string u16;
        bool ret = u16_from_u32(u16, input.data(), input.size());
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }

    inline bool ansi_from_wide(std::string& output, const std::wstring& input)
    {
        return ansi_from_wide(output, input.data(), input.size());
    }
    inline bool u8_from_wide(std::string& output, const std::wstring& input)
    {
        return u8_from_wide(output, input.data(), input.size());
    }
    inline bool u16_from_wide(std::u16string& output, const std::wstring& input)
    {
        return u16_from_wide(output, input.data(), input.size());
    }
    inline bool u32_from_wide(std::u32string& output, const std::wstring& input)
    {
        return u32_from_wide(output, input.data(), input.size());
    }

    inline bool wide_from_wide(std::wstring& output, const std::wstring& input)
    {
        std::u16string u16;
        bool ret = u16_from_u16(u16, reinterpret_cast<const char16_t*>(input.data()), input.size());
        output.assign(reinterpret_cast<const wchar_t*>(u16.data()), u16.size());
        return ret;
    }

    #ifdef _UNICODE
        #define text_from_ansi      wide_from_ansi
        #define text_from_u8        wide_from_u8
        #define text_from_u16       wide_from_u16
        #define text_from_u32       wide_from_u32
        #define text_from_wide      wide_from_wide

        #define ansi_from_text      ansi_from_wide
        #define   u8_from_text        u8_from_wide
        #define  u16_from_text       u16_from_wide
        #define  u32_from_text       u32_from_wide
        #define wide_from_text      wide_from_wide

        #define text_from_text      wide_from_wide
    #else
        #define text_from_ansi      ansi_from_ansi
        #define text_from_u8        ansi_from_u8
        #define text_from_u16       ansi_from_u16
        #define text_from_u32       ansi_from_u32
        #define text_from_wide      ansi_from_wide

        #define ansi_from_text      ansi_from_ansi
        #define   u8_from_text        u8_from_ansi
        #define  u16_from_text       u16_from_ansi
        #define  u32_from_text       u32_from_ansi
        #define wide_from_text      wide_from_ansi

        #define text_from_text      ansi_from_ansi
    #endif
} //namespace khmz
#endif

//////////////////////////////////////////////////////////////////////////////
