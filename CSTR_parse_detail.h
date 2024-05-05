#pragma once

#include "CSTR_parse.h"

namespace khmz
{
    template <typename T_WRITER>
    bool CSTR_parse(T_WRITER& writer, const char32_t *first, const char32_t *last, int& flags)
    {
        char32_t ch;
        bool in_quote = false;
        bool in_escape = false;
        bool in_r = false;
        bool quote_found = false;
        char32_t ach[8 + 1];
        char sz[8 + 1];
        size_t ich;
        char32_t ch0;

        while (first != last)
        {
            if (in_escape)
            {
                // invaild escape sequence
                flags |= CSTR_PARSE_INVALID_ESCAPE;
                in_quote = false;
                break;
            }

            if (in_quote)
            {
                if (in_r) // R"delimiter(...)delimiter"
                {
                    auto delimiter = first;

                    ich = 0;
                    while (first != last && *first != '(')
                        first++;

                    if (first == last)
                    {
                        flags |= CSTR_PARSE_QUOTE_NOT_CLOSED;
                        break;
                    }

                    auto delimiter_length = first - delimiter;
                    if (delimiter_length > 16)
                    {
                        flags |= CSTR_PARSE_DELIMITER_TOO_LONG;
                        break;
                    }

                    if (first != last && *first == '(')
                        ++first;

                    auto start = first;
                    do
                    {
                        while (first != last && *first != ')')
                            ++first;

                        if (first == last)
                        {
                            flags |= CSTR_PARSE_RAW_LITERAL_NOT_CLOSED;
                            break;
                        }

                        if (memcmp(first + 1, delimiter, delimiter_length * sizeof(char32_t)) == 0 &&
                            *(first + 1 + delimiter_length) == '\"')
                        {
                            while (start != first)
                                writer.write_u32(*start++);

                            ++first;
                            first += delimiter_length + 1;
                            break;
                        }

                        ++first;
                    } while (first != last);

                    in_r = in_quote = false;
                    continue;
                }

                ch = *first++;

                if (ch == '\"')
                {
                    in_quote = false;
                    if (first == last)
                        break;

                    continue;
                }

                if (ch != '\\') // non-escape
                {
                    writer.write_u32(ch);
                    continue;
                }

                // escape sequence begins
                in_escape = true;

                if (first == last) break; else ch = *first++;

                switch (ch)
                {
                case '\'': writer.write_raw('\''); in_escape = false; continue;
                case '\"': writer.write_raw('\"'); in_escape = false; continue;
                case '\?': writer.write_raw('\?'); in_escape = false; continue;
                case '\\': writer.write_raw('\\'); in_escape = false; continue;
                case 'a' : writer.write_raw('\a'); in_escape = false; continue;
                case 'b' : writer.write_raw('\b'); in_escape = false; continue;
                case 'f' : writer.write_raw('\f'); in_escape = false; continue;
                case 'n' : writer.write_raw('\n'); in_escape = false; continue;
                case 'r' : writer.write_raw('\r'); in_escape = false; continue;
                case 't' : writer.write_raw('\t'); in_escape = false; continue;
                case 'v' : writer.write_raw('\v'); in_escape = false; continue;
                case 'x': // "\xXX..."
                    for (ich = 0; ich < 8; ++ich)
                    {
                        if (first == last)
                        {
                            sz[ich] = 0;
                            break;
                        }
                        ach[ich] = *first++;
                        if (ach[ich] <= 0x7F && std::isxdigit((char)ach[ich]))
                        {
                            sz[ich] = (char)ach[ich];
                            continue;
                        }
                        sz[ich] = 0;
                        --first;
                        break;
                    }

                    if (sz[0])
                    {
                        ch0 = (char32_t)strtoul(sz, NULL, 16);
                        if (ch0 <= 0x10FFFF)
                        {
                            writer.write_u32(ch0);
                            in_escape = false;
                            continue;
                        }
                        flags |= CSTR_PARSE_INVALID_ESCAPE;
                    }
                    break;

                case 'u': // "\uXXXX"
                    ich = 0;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (ich == 4 &&
                        ach[0] <= 0x7F && std::isxdigit((char)ach[0]) &&
                        ach[1] <= 0x7F && std::isxdigit((char)ach[1]) &&
                        ach[2] <= 0x7F && std::isxdigit((char)ach[2]) &&
                        ach[3] <= 0x7F && std::isxdigit((char)ach[3]))
                    {
                        sz[0] = (char)ach[0];
                        sz[1] = (char)ach[1];
                        sz[2] = (char)ach[2];
                        sz[3] = (char)ach[3];
                        sz[4] = 0;
                        ch0 = (char32_t)strtoul(sz, NULL, 16);
                        writer.write_u32(ch0);
                        in_escape = false;
                        continue;
                    }
                    break;
                case 'U': // "\UXXXXXXXX"
                    ich = 0;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (first != last) ach[ich++] = *first++;
                    if (ich == 8 &&
                        ach[0] <= 0x7F && std::isxdigit((char)ach[0]) &&
                        ach[1] <= 0x7F && std::isxdigit((char)ach[1]) &&
                        ach[2] <= 0x7F && std::isxdigit((char)ach[2]) &&
                        ach[3] <= 0x7F && std::isxdigit((char)ach[3]) &&
                        ach[4] <= 0x7F && std::isxdigit((char)ach[4]) &&
                        ach[5] <= 0x7F && std::isxdigit((char)ach[5]) &&
                        ach[6] <= 0x7F && std::isxdigit((char)ach[6]) &&
                        ach[7] <= 0x7F && std::isxdigit((char)ach[7]))
                    {
                        sz[0] = (char)ach[0];
                        sz[1] = (char)ach[1];
                        sz[2] = (char)ach[2];
                        sz[3] = (char)ach[3];
                        sz[4] = (char)ach[4];
                        sz[5] = (char)ach[5];
                        sz[6] = (char)ach[6];
                        sz[7] = (char)ach[7];
                        sz[8] = 0;
                        ch0 = (char32_t)strtoul(sz, NULL, 16);
                        if (ch0 <= 0x10FFFF)
                        {
                            writer.write_u32(ch0);
                            in_escape = false;
                            continue;
                        }
                        flags |= CSTR_PARSE_INVALID_ESCAPE;
                        break;
                    }
                    break;
                default:
                    if (!('0' <= ch && ch <= '7')) // invalid escape sequence?
                    {
                        writer.write_u32(ch);
                        in_escape = false;
                        continue;
                    }

                    // Octal escape
                    ich = 0;
                    sz[ich++] = (char)ch;
                    ch = (first != last) ? *first++ : 0;

                    if (('0' <= ch && ch <= '7'))
                    {
                        sz[ich++] = (char)ch;
                        ch = (first != last) ? *first++ : 0;

                        if (('0' <= ch && ch <= '7'))
                        {
                            sz[ich++] = (char)ch;
                        }
                    }
                    sz[ich] = 0;

                    if (!('0' <= ch && ch <= '7'))
                        --first;

                    ch0 = (char32_t)strtoul(sz, NULL, 8);
                    writer.write_u32(ch0);
                    in_escape = false;
                    continue;
                }
            }
            else
            {
                ch = *first++;

                if (ch <= 0x7F && std::isspace((char)ch))
                    continue;

                ich = 0;
                if (ch != '"' && ch <= 0x7F)
                {
                    sz[ich++] = (char)ch;
                    ch = (first != last) ? *first++ : 0;

                    if (ch != '"' && ch <= 0x7F)
                    {
                        sz[ich++] = (char)ch;
                        ch = (first != last) ? *first++ : 0;

                        if (ch != '"' && ch <= 0x7F)
                        {
                            sz[ich++] = (char)ch;
                            ch = (first != last) ? *first++ : 0;
                         
                            if (ch != '"' && ch <= 0x7F)
                            {
                                sz[ich++] = (char)ch;
                                ch = (first != last) ? *first++ : 0;
                            }
                        }
                    }
                }
                sz[ich] = 0;

                if (ch == '"')
                {
                    quote_found = true;
                }
                else
                {
                    flags |= CSTR_PARSE_SYNTAX_ERROR;
                    break;
                }

                if (ich == 0 || (ich == 1 && sz[0] == 'R')) // "..." (ANSI)
                {
                    if (ich == 1 && sz[0] == 'R')
                        in_r = true;

                    if ((flags & CSTR_PARSE_PREFIX_ALL) == 0 ||
                        (flags & CSTR_PARSE_PREFIX_ALL) == CSTR_PARSE_PREFIX_A)
                    {
                        flags |= CSTR_PARSE_PREFIX_A;
                        in_quote = true;
                        continue;
                    }

                    UTFIO_THROW();
                    flags |= CSTR_PARSE_PREFIX_MISMATCH;
                    break;
                }

                if (sz[0] == 'L' && (ich == 1 || (ich == 2 && sz[1] == 'R'))) // L"..." (Wide)
                {
                    if (ich == 2 && sz[1] == 'R')
                        in_r = true;

                    if ((flags & CSTR_PARSE_PREFIX_ALL) == 0 ||
                        (flags & CSTR_PARSE_PREFIX_ALL) == CSTR_PARSE_PREFIX_L)
                    {
                        flags |= CSTR_PARSE_PREFIX_L;
                        in_quote = true;
                        continue;
                    }

                    UTFIO_THROW();
                    flags |= CSTR_PARSE_PREFIX_MISMATCH;
                    break;
                }

                if (sz[0] == 'u' && (ich == 1 || (ich == 2 && sz[1] == 'R'))) // u"..." (UTF-16)
                {
                    if (ich == 2 && sz[1] == 'R')
                        in_r = true;

                    if ((flags & CSTR_PARSE_PREFIX_ALL) == 0 ||
                        (flags & CSTR_PARSE_PREFIX_ALL) == CSTR_PARSE_PREFIX_U16)
                    {
                        flags |= CSTR_PARSE_PREFIX_U16;
                        in_quote = true;
                        continue;
                    }

                    UTFIO_THROW();
                    flags |= CSTR_PARSE_PREFIX_MISMATCH;
                    break;
                }

                if (sz[0] == 'u' && sz[1] == '8' &&
                    (ich == 2 || (ich == 3 && sz[2] == 'R'))) // u8"..." (UTF-8)
                {
                    if (ich == 3 && sz[2] == 'R')
                        in_r = true;

                    if ((flags & CSTR_PARSE_PREFIX_ALL) == 0 ||
                        (flags & CSTR_PARSE_PREFIX_ALL) == CSTR_PARSE_PREFIX_U8)
                    {
                        flags |= CSTR_PARSE_PREFIX_U8;
                        in_quote = true;
                        continue;
                    }

                    UTFIO_THROW();
                    flags |= CSTR_PARSE_PREFIX_MISMATCH;
                    break;
                }

                if (sz[0] == 'U' && (ich == 1 || (ich == 2 && sz[1] == 'R'))) // U"..." (UTF-32)
                {
                    if (ich == 2 && sz[1] == 'R')
                        in_r = true;

                    if ((flags & CSTR_PARSE_PREFIX_ALL) == 0 ||
                        (flags & CSTR_PARSE_PREFIX_ALL) == CSTR_PARSE_PREFIX_U32)
                    {
                        flags |= CSTR_PARSE_PREFIX_U32;
                        in_quote = true;
                        continue;
                    }

                    UTFIO_THROW();
                    flags |= CSTR_PARSE_PREFIX_MISMATCH;
                    break;
                }

                flags |= CSTR_PARSE_NOT_QUOTED;
                break;
            }
        }

        if (!quote_found)
            flags |= CSTR_PARSE_QUOTE_NOT_FOUND;

        if (in_quote)
            flags |= CSTR_PARSE_QUOTE_NOT_CLOSED;

        return !(flags & CSTR_PARSE_ERRORS);
    }

    template <typename T_WRITER, typename T_READER>
    bool CSTR_parse(T_WRITER& writer, T_READER& reader, int& flags)
    {
        std::u32string str;
        StringWriterU32 string_writer(str);
        convert(string_writer, reader);
        const auto first = str.data();
        const auto last = first + str.size();
        return CSTR_parse(writer, first, last, flags);
    }
} // namespace khmz
