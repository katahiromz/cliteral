// utfio_classes.h --- UTF IO classes
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utfio.h"

static_assert(UTFIO_INVALID_CHAR <= 0xFFFF, "UTFIO_INVALID_CHAR must be less than 0x10000");

enum READ_RET
{
    READ_RET_EOS = 1,
    READ_RET_OK = 0,
    READ_RET_CUTOFF = -1,
    READ_RET_INVALID = -2,
};

//////////////////////////////////////////////////////////////////////////////
// Reader

namespace khmz
{
    template <typename T_CHAR>
    class GenericReader
    {
    public:
        virtual bool can_read() const = 0;
        virtual bool read_raw(T_CHAR& value) = 0;
        READ_RET read_u32(char32_t& codepoint);
    };

    template <>
    inline READ_RET GenericReader<char>::read_u32(char32_t& codepoint)
    {
        uint8_t ch0, ch1, ch2, ch3;

        if (!read_raw((char&)ch0))
            return READ_RET_EOS;

        if ((ch0 & 0x80) == 0)
        {
            codepoint = ch0;
            return READ_RET_OK;
        }

        if (!read_raw((char&)ch1))
            return READ_RET_CUTOFF;

        if ((ch0 & 0xE0) == 0xC0)
        {
            codepoint = ((ch0 & 0x1F) << 6) | (ch1 & 0x3F);
            return READ_RET_OK;
        }

        if (!read_raw((char&)ch2))
            return READ_RET_CUTOFF;

        if ((ch0 & 0xF0) == 0xE0)
        {
            codepoint = ((ch0 & 0x0F) << 12) | ((ch1 & 0x3F) << 6) | (ch2 & 0x3F);
            return READ_RET_OK;
        }

        if (!read_raw((char&)ch3))
            return READ_RET_CUTOFF;

        if ((ch0 & 0xF8) == 0xF0)
        {
            codepoint = ((ch0 & 0x07) << 18) |
                        ((ch1 & 0x3F) << 12) |
                        ((ch2 & 0x3F) << 6) |
                        (ch3 & 0x3F);
            return READ_RET_OK;
        }

        UTFIO_THROW();
        codepoint = UTFIO_INVALID_CHAR;
        return READ_RET_INVALID;
    }

#ifdef UTFIO_SUPPORT_ANSI
    // Multi-byte
    template <>
    inline READ_RET GenericReader<uint8_t>::read_u32(char32_t& codepoint)
    {
        char buf[3];
        wchar_t wsz[2];
        int ret;

        if (!read_raw((uint8_t&)buf[0]))
            return READ_RET_EOS;

        if (!IsDBCSLeadByteEx(UTFIO_CODEPAGE, buf[0]))
        {
            ::MultiByteToWideChar(UTFIO_CODEPAGE, 0, buf, 1, wsz, 2);
            codepoint = wsz[0];
            return READ_RET_OK;
        }

        if (!read_raw((uint8_t&)buf[1]))
            return READ_RET_CUTOFF;

        ret = ::MultiByteToWideChar(UTFIO_CODEPAGE, MB_ERR_INVALID_CHARS, buf, 2, wsz, 2);
        if (!ret && GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
            return READ_RET_INVALID;

        if (!(0xD800 <= wsz[0] && wsz[0] <= 0xDBFF))
        {
            codepoint = wsz[0];
            return READ_RET_OK;
        }

        if (!(0xDC00 <= wsz[1] && wsz[1] <= 0xDFFF))
            return READ_RET_INVALID;

        codepoint = ((uint32_t)(wsz[0] - 0xD800) << 10) + (wsz[1] - 0xDC00) + 0x10000;
        return READ_RET_OK;
    }
#endif

    template <>
    inline READ_RET GenericReader<char16_t>::read_u32(char32_t& codepoint)
    {
        char16_t ch0, ch1;

        if (!read_raw(ch0))
            return READ_RET_EOS;

        if (!(0xD800 <= ch0 && ch0 <= 0xDBFF))
        {
            codepoint = ch0;
            return READ_RET_OK;
        }

        if (!read_raw(ch1))
            return READ_RET_CUTOFF;

        if (0xDC00 <= ch1 && ch1 <= 0xDFFF)
        {
            codepoint = ((uint32_t)(ch0 - 0xD800) << 10) +
                        (ch1 - 0xDC00) + 0x10000;
            return READ_RET_OK;
        }

        UTFIO_THROW();
        codepoint = UTFIO_INVALID_CHAR;
        return READ_RET_INVALID;
    }

    template <>
    inline READ_RET GenericReader<char32_t>::read_u32(char32_t& codepoint)
    {
        if (!read_raw(codepoint))
            return READ_RET_EOS;

        return READ_RET_OK;
    }

    template <typename T_INPUT_ITERATOR, typename T_CHAR>
    class IteratorReader : public GenericReader<T_CHAR>
    {
    public:
        T_INPUT_ITERATOR m_first, m_last;

        IteratorReader()
        {
        }
        IteratorReader(T_INPUT_ITERATOR first, T_INPUT_ITERATOR last)
            : m_first(first)
            , m_last(last)
        {
        }

        bool can_read() const override
        {
            return m_first != m_last;
        }

        bool read_raw(T_CHAR& value) override
        {
            if (!can_read())
                return false;

            value = *m_first++;
            return true;
        }

        T_INPUT_ITERATOR save()
        {
            return m_first;
        }

        void restore(T_INPUT_ITERATOR it)
        {
            m_first = it;
        }
    };

    template <typename T_CHAR>
    class StringReader : public GenericReader<T_CHAR>
    {
    public:
        const T_CHAR *m_first;
        const T_CHAR *m_last;

        StringReader(const T_CHAR *input, size_t input_size)
            : m_first(input), m_last(input + input_size)
        {
        }

        bool can_read() const override
        {
            return m_first != m_last;
        }

        bool read_raw(T_CHAR& value) override
        {
            if (!can_read())
                return false;

            value = *m_first++;
            return true;
        }

        const T_CHAR *save()
        {
            return m_first;
        }

        void restore(const T_CHAR *it)
        {
            m_first = it;
        }
    };

#ifdef UTFIO_SUPPORT_ANSI
    struct StringReaderA : public GenericReader<uint8_t>
    {
    public:
        const char *m_first;
        const char *m_last;

        StringReaderA(const char *input, size_t input_size)
            : m_first(input), m_last(input + input_size)
        {
        }

        bool can_read() const override
        {
            return m_first != m_last;
        }

        bool read_raw(uint8_t& value) override
        {
            if (!can_read())
                return false;

            value = *m_first++;
            return true;
        }

        const char *save()
        {
            return m_first;
        }

        void restore(const char *it)
        {
            m_first = it;
        }
    };
#endif

    using StringReaderU8  = StringReader<char>;
    using StringReaderU16 = StringReader<char16_t>;
    using StringReaderU32 = StringReader<char32_t>;

    class StringReaderU16BE : public StringReader<char16_t>
    {
    public:
        using StringReader<char16_t>::StringReader;

        bool read_raw(char16_t& value) override
        {
            if (!this->can_read())
                return false;

            value = swap_bytes(*this->m_first++);
            return true;
        }
    };

    template <typename T_CHAR, bool t_swab = false>
    class FileReader : public GenericReader<T_CHAR>
    {
    public:
        using value_type = T_CHAR;

        FILE *m_fp = nullptr;

        FileReader()
        {
        }
        FileReader(FILE *fp) : m_fp(fp)
        {
            detect_bom(m_fp);
        }
        FileReader(const char *fname)
            : m_fp(fopen(fname, "rb"))
        {
            detect_bom(m_fp);
        }
#ifdef _WIN32
        FileReader(const wchar_t *fname)
            : m_fp(_wfopen(fname, L"rb"))
        {
            detect_bom(m_fp);
        }
#endif

        bool can_read() const override
        {
            return !std::feof(m_fp);
        }

        bool read_raw(T_CHAR& value) override
        {
            return !!std::fread(&value, sizeof(T_CHAR), 1, m_fp);
        }

        long save()
        {
            return std::ftell(m_fp);
        }

        void restore(long offset)
        {
            std::fseek(m_fp, offset, SEEK_SET);
        }
    };

#ifdef UTFIO_SUPPORT_ANSI
    using FileReaderA   = FileReader<uint8_t , false>;
#endif
    using FileReaderU8  = FileReader<char    , false>;
    using FileReaderU16 = FileReader<char16_t, false>;
    using FileReaderU32 = FileReader<char32_t, false>;

    class FileReaderU16BE : public FileReader<char16_t, true>
    {
    public:
        using FileReader<char16_t, true>::FileReader;

        bool read_raw(char16_t& value) override
        {
            char16_t tmp;
            if (!std::fread(&tmp, sizeof(char16_t), 1, m_fp))
                return false;

            value = swap_bytes(tmp);
            return true;
        }
    };
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////
// Writer

namespace khmz
{
    class GenericWriter
    {
    public:
        virtual bool can_write() const = 0;
        virtual bool write_raw(uint32_t value) = 0;
        virtual bool write_u32(char32_t ch) = 0;
    };

#ifdef UTFIO_SUPPORT_ANSI
    // Multi-byte
    class WriterA : public GenericWriter
    {
    public:
        char m_invalid[8] = { 0 };

        bool write_u32(char32_t ch) override
        {
            wchar_t wsz[2];
            int i, len;
            char sz[8];

            if (!m_invalid[0])
            {
                wsz[0] = UTFIO_INVALID_CHAR;
                wsz[1] = 0;
                ::WideCharToMultiByte(UTFIO_CODEPAGE, 0, wsz, -1, m_invalid, 8, nullptr, nullptr);
            }

            if (ch <= 0xFFFF)
            {
                wsz[0] = (wchar_t)ch;
                len = 1;
            }
            else if (ch <= 0x10FFFF)
            {
                wsz[0] = wchar_t((ch >> 10) + 0xD7C0);
                wsz[1] = wchar_t((ch & 0x3FF) | 0xDC00);
                len = 2;
            }
            else
            {
                return false;
            }

            BOOL bDefault = FALSE;
            len = ::WideCharToMultiByte(UTFIO_CODEPAGE, 0, wsz, len, sz, 8, m_invalid, &bDefault);
            if (bDefault)
                return false;

            sz[len] = 0;

            for (i = 0; i < len && sz[i]; ++i)
            {
                if (!write_raw(sz[i]))
                    return false;
            }

            return true;
        }
    };
#endif

    class WriterU8 : public GenericWriter
    {
    public:
        bool write_u32(char32_t ch) override
        {
            if (ch <= 0x7F)
            {
                return write_raw(ch);
            }
            else if (ch <= 0x7FF)
            {
                return write_raw(0xC0 | (ch >> 6)) &&
                       write_raw(0x80 | (ch & 0x3F));
            }
            else if (ch <= 0xFFFF)
            {
                return write_raw(0xE0 | (ch >> 12)) &&
                       write_raw(0x80 | ((ch >> 6) & 0x3F)) &&
                       write_raw(0x80 | (ch & 0x3F));
            }
            else if (ch <= 0x10FFFF)
            {
                return write_raw(((ch >> 18) & 0x07) | 0xF0) &&
                       write_raw(((ch >> 12) & 0x3F) | 0x80) &&
                       write_raw(((ch >> 6) & 0x3F) | 0x80) &&
                       write_raw((ch & 0x3F) | 0x80);
            }

            UTFIO_THROW();
            return write_raw(UTFIO_INVALID_CHAR);
        }
    };

    class WriterU16 : public GenericWriter
    {
    public:
        bool write_u32(char32_t ch) override
        {
            if (ch <= 0xFFFF)
            {
                return write_raw(ch);
            }
            else if (ch <= 0x10FFFF)
            {
                return write_raw((ch >> 10) + 0xD7C0) &&
                       write_raw((ch & 0x3FF) | 0xDC00);
            }

            UTFIO_THROW();
            return write_raw(UTFIO_INVALID_CHAR);
        }
    };

    class WriterU32 : public GenericWriter
    {
    public:
        bool write_u32(char32_t ch) override
        {
            if (ch <= 0x10FFFF)
                return write_raw(ch);

            UTFIO_THROW();
            return write_raw(UTFIO_INVALID_CHAR);
        }
    };

    template <typename T_OUTPUT_ITERATOR, typename T_BASE>
    class IteratorWriter : public T_BASE
    {
    public:
        T_OUTPUT_ITERATOR m_first, m_last;

        IteratorWriter(T_OUTPUT_ITERATOR first, T_OUTPUT_ITERATOR last)
            : m_first(first)
            , m_last(last)
        {
        }

        bool can_write() const override
        {
            return m_first != m_last;
        }

        bool write_raw(uint32_t value) override
        {
            if (!can_write())
                return false;
            *m_first++ = decltype(*m_first)(value);
            return true;
        }
    };

#ifdef UTFIO_SUPPORT_ANSI
    template <typename T_OUTPUT_ITERATOR>
    using IteratorWriterA   = IteratorWriter<T_OUTPUT_ITERATOR, WriterA>;
#endif

    template <typename T_OUTPUT_ITERATOR>
    using IteratorWriterU8  = IteratorWriter<T_OUTPUT_ITERATOR, WriterU8>;

    template <typename T_OUTPUT_ITERATOR>
    using IteratorWriterU16 = IteratorWriter<T_OUTPUT_ITERATOR, WriterU16>;

    template <typename T_OUTPUT_ITERATOR>
    using IteratorWriterU32 = IteratorWriter<T_OUTPUT_ITERATOR, WriterU32>;

    template <typename T_OUTPUT_ITERATOR>
    class IteratorWriterU16BE : public IteratorWriter<T_OUTPUT_ITERATOR, WriterU16>
    {
    public:
        using IteratorWriter<T_OUTPUT_ITERATOR, WriterU16>::IteratorWriter;

        bool write_raw(uint32_t value) override
        {
            if (!this->can_write())
                return false;
            *this->m_first++ = swap_bytes(decltype(*this->m_first)(value));
            return true;
        }
    };

    template <typename T_BASE, typename T_CHAR>
    class BufferWriter : public T_BASE
    {
    public:
        using value_type = T_CHAR;
        T_CHAR *m_first;
        T_CHAR *m_last;

        BufferWriter(T_CHAR* first, T_CHAR* last)
            : m_first(first), m_last(last)
        {
        }
        BufferWriter(T_CHAR* ptr, size_t length)
            : m_first(ptr), m_last(ptr + length)
        {
        }
        template <size_t t_count>
        BufferWriter(T_CHAR (&array)[t_count])
            : m_first(array), m_last(&array[t_count])
        {
        }

        bool can_write() const override
        {
            return m_first != m_last;
        }

        bool write_raw(uint32_t value) override
        {
            if (!can_write())
                return false;
            *m_first++ = decltype(*m_first)(value);
            return true;
        }
    };

#ifdef UTFIO_SUPPORT_ANSI
    using BufferWriterA     = BufferWriter<WriterA,     uint8_t>;
#endif
    using BufferWriterU8    = BufferWriter<WriterU8,    char>;
    using BufferWriterU16   = BufferWriter<WriterU16,   char16_t>;
    using BufferWriterU32   = BufferWriter<WriterU32,   char32_t>;

    class BufferWriterU16BE : public BufferWriter<WriterU16, char16_t>
    {
    public:
        using BufferWriter<WriterU16, char16_t>::BufferWriter;

        bool write_raw(uint32_t value) override
        {
            if (!can_write())
                return false;
            *m_first++ = swap_bytes(char16_t(value));
            return true;
        }
    };

    template <typename T_CHAR, typename T_BASE>
    class StringWriter : public T_BASE
    {
    public:
        using value_type = T_CHAR;

        std::basic_string<T_CHAR>& m_str;

        StringWriter(std::basic_string<T_CHAR>& str) : m_str(str)
        {
        }

        bool can_write() const
        {
            return true;
        }

        bool write_raw(uint32_t value)
        {
            m_str.push_back(value_type(value));
            return true;
        }

        void reserve(size_t length)
        {
            m_str.reserve(length);
        }
    };

#ifdef UTFIO_SUPPORT_ANSI
    class StringWriterA : public WriterA
    {
    public:
        using value_type = char;

        std::basic_string<char>& m_str;

        StringWriterA(std::string& str) : m_str(str)
        {
        }

        bool can_write() const
        {
            return true;
        }

        bool write_raw(uint32_t value)
        {
            m_str.push_back(value_type(value));
            return true;
        }

        void reserve(size_t length)
        {
            m_str.reserve(length);
        }
    };
#endif

    using StringWriterU8    = StringWriter<char,     WriterU8   >;
    using StringWriterU16   = StringWriter<char16_t, WriterU16  >;
    using StringWriterU32   = StringWriter<char32_t, WriterU32  >;

    class StringWriterU16BE : public StringWriter<char16_t, WriterU16>
    {
    public:
        using StringWriter<char16_t, WriterU16>::StringWriter;

        bool write_raw(uint32_t value)
        {
            m_str.push_back(swap_bytes(char16_t(value)));
            return true;
        }
    };

    template <typename T_BASE, typename T_CHAR>
    class FileWriter : public T_BASE
    {
    public:
        using value_type = T_CHAR;

        FILE *m_fp = nullptr;

        FileWriter()
        {
        }
        FileWriter(FILE *fp) : m_fp(fp)
        {
        }
        FileWriter(const char *fname)
            : m_fp(std::fopen(fname, "wb"))
        {
        }
#ifdef _WIN32
        FileWriter(const wchar_t *fname)
            : m_fp(_wfopen(fname, L"wb"))
        {
        }
#endif
        ~FileWriter()
        {
            if (m_fp)
                std::fclose(m_fp);
        }

        bool can_write() const override
        {
            return !std::feof(m_fp);
        }

        bool write_raw(uint32_t value) override
        {
            if (!can_write())
                return false;

            T_CHAR ch = T_CHAR(value);
            return !!std::fwrite(&ch, sizeof(ch), 1, m_fp);
        }
    };

#ifdef UTFIO_SUPPORT_ANSI
    using FileWriterA     = FileWriter<WriterA,     uint8_t>;
#endif
    using FileWriterU8    = FileWriter<WriterU8,    char>;
    using FileWriterU16   = FileWriter<WriterU16,   char16_t>;
    using FileWriterU32   = FileWriter<WriterU32,   char32_t>;

    class FileWriterU16BE : public FileWriter<WriterU16, char16_t>
    {
    public:
        using FileWriter<WriterU16, char16_t>::FileWriter;

        bool write_raw(uint32_t value) override
        {
            if (!can_write())
                return false;

            char16_t ch = swap_bytes(char16_t(value));
            return !!std::fwrite(&ch, sizeof(ch), 1, m_fp);
        }
    };
} // namespace khmz

//////////////////////////////////////////////////////////////////////////////
// Converting

namespace khmz
{
    template <typename T_WRITER, typename T_READER>
    bool convert(T_WRITER& writer, T_READER& reader)
    {
        bool ret = true;
        char32_t u32;
        while (writer.can_write())
        {
            READ_RET read_ret = reader.read_u32(u32);
            if (read_ret != READ_RET_OK)
            {
                if (read_ret > READ_RET_OK)
                    break;

                writer.write_u32(UTFIO_INVALID_CHAR);
                ret = false;
                continue;
            }
            if (!writer.write_u32(u32))
                ret = false;
        }
        return ret;
    }
} // namespace khmz
