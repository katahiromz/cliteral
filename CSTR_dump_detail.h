#pragma once

#include "CSTR_dump.h"

namespace khmz
{
    static constexpr auto HEX = "0123456789ABCDEF";

    template <typename T_WRITER>
    bool escape_x(T_WRITER& writer, uint8_t ch)
    {
        return writer.write_raw('\\') &&
               writer.write_raw('x') &&
               writer.write_raw(HEX[ch >> 4]) &&
               writer.write_raw(HEX[ch & 0xF]);
    }
    template <typename T_WRITER>
    bool escape_x(T_WRITER& writer, char16_t ch)
    {
        return writer.write_raw('\\') &&
               writer.write_raw('x') &&
               writer.write_raw(HEX[(ch >> 12) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 8) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 4) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 0) & 0xF]);
    }
    template <typename T_WRITER>
    bool escape_x(T_WRITER& writer, char32_t ch)
    {
        return writer.write_raw('\\') &&
               writer.write_raw('x') &&
               writer.write_raw(HEX[(ch >> 28) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 24) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 20) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 16) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 12) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 8) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 4) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 0) & 0xF]);
    }

    template <typename T_WRITER>
    bool escape_o(T_WRITER& writer, uint8_t ch)
    {
        return writer.write_raw('\\') &&
               writer.write_raw(HEX[(ch >> 6) & 0x7]) &&
               writer.write_raw(HEX[(ch >> 3) & 0x7]) &&
               writer.write_raw(HEX[(ch >> 0) & 0x7]);
    }

    template <typename T_WRITER>
    bool escape_u16(T_WRITER& writer, char16_t ch)
    {
        return writer.write_raw('\\') &&
               writer.write_raw('u') &&
               writer.write_raw(HEX[(ch >> 12) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 8) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 4) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 0) & 0xF]);
    }

    template <typename T_WRITER>
    bool escape_u32(T_WRITER& writer, char32_t ch)
    {
        return writer.write_raw('\\') &&
               writer.write_raw('U') &&
               writer.write_raw(HEX[(ch >> 28) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 24) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 20) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 16) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 12) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 8) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 4) & 0xF]) &&
               writer.write_raw(HEX[(ch >> 0) & 0xF]);
    }

    template <typename T_WRITER>
    bool CSTR_prefix_dump(T_WRITER& writer, int& flags)
    {
        if (flags & CSTR_DUMP_PREFIX_L)
        {
            return writer.write_raw('L');
        }
        else if (flags & CSTR_DUMP_PREFIX_U8)
        {
            return writer.write_raw('u') && writer.write_raw('8');
        }
        else if (flags & CSTR_DUMP_PREFIX_U16)
        {
            return writer.write_raw('u');
        }
        else if (flags & CSTR_DUMP_PREFIX_U32)
        {
            return writer.write_raw('U');
        }
        return true;
    }

    template <typename T_WRITER, typename T_READER>
    bool CSTR_dump(T_WRITER& writer, T_READER& reader, int& flags)
    {
        bool ret = true;
        bool next_should_not_be_hex = false;

        CSTR_prefix_dump(writer, flags) && writer.write_raw('"');

        while (writer.can_write())
        {
            char32_t u32;
            READ_RET read_ret = reader.read_u32(u32);
            if (read_ret != READ_RET_OK)
            {
                if (read_ret > READ_RET_OK)
                    break;

                writer.write_u32(UTFIO_INVALID_CHAR);
                ret = false;
                continue;
            }

            if (next_should_not_be_hex && u32 <= 0x7F && std::isxdigit((uint8_t)u32))
            {
                next_should_not_be_hex = false;
                writer.write_raw('"') &&
                writer.write_raw(' ') &&
                CSTR_prefix_dump(writer, flags) &&
                writer.write_raw('"');
                writer.write_u32(u32);
                continue;
            }
            next_should_not_be_hex = false;

            switch (u32)
            {
            case '\'': writer.write_raw('\\') && writer.write_raw('\''); break;
            case '\"': writer.write_raw('\\') && writer.write_raw('\"'); break;
            case '\?': writer.write_raw('\\') && writer.write_raw('\?'); break;
            case '\\': writer.write_raw('\\') && writer.write_raw('\\'); break;
            case '\a': writer.write_raw('\\') && writer.write_raw('a'); break;
            case '\b': writer.write_raw('\\') && writer.write_raw('b'); break;
            case '\f': writer.write_raw('\\') && writer.write_raw('f'); break;
            case '\n': writer.write_raw('\\') && writer.write_raw('n'); break;
            case '\r': writer.write_raw('\\') && writer.write_raw('r'); break;
            case '\t': writer.write_raw('\\') && writer.write_raw('t'); break;
            case '\v': writer.write_raw('\\') && writer.write_raw('v'); break;
            default:
                if (u32 <= 0x7F && std::isprint((uint8_t)u32))
                {
                    writer.write_raw((typename T_WRITER::value_type)u32);
                }
                else if (u32 == 0 && !reader.can_read())
                {
                    writer.write_raw('\\') && writer.write_raw('0');
                    break;
                }
                else if (flags & CSTR_DUMP_ESCAPE_NOCONV)
                {
                    writer.write_u32(u32);
                }
                else if ((flags & CSTR_DUMP_ESCAPE_OCTAL) && u32 <= 0xFF)
                {
                    escape_o(writer, uint8_t(u32));
                }
                else if ((flags & CSTR_DUMP_ESCAPE_U16) && u32 <= 0xFFFF)
                {
                    escape_u16(writer, char16_t(u32));
                    next_should_not_be_hex = true;
                }
                else if (flags & CSTR_DUMP_ESCAPE_U32)
                {
                    escape_u32(writer, u32);
                    next_should_not_be_hex = true;
                }
                else
                {
                    if (u32 <= 0xFF)
                        escape_x(writer, uint8_t(u32));
                    else if (u32 <= 0xFFFF)
                        escape_x(writer, char16_t(u32));
                    else
                        escape_x(writer, u32);
                    next_should_not_be_hex = true;
                }
                break;
            }
        }

        writer.write_raw('"');

        if (flags & CSTR_DUMP_NUL)
            writer.write_raw(0);

        return ret;
    } // CSTR_dump
} // namespace khmz
