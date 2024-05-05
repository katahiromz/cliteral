// utfio.cpp --- The UTF IO library
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#define UTFIO_CLASSES
#include "utfio.h"

namespace khmz
{
    bool ansi_from_ansi(std::string& output, const char *input, size_t input_length)
    {
        StringWriterA writer(output);
        writer.reserve(input_length);
        StringReaderA reader(input, input_length);
        return convert(writer, reader);
    }

    bool ansi_from_u8(std::string& output, const char *input, size_t input_length)
    {
        StringWriterA writer(output);
        writer.reserve(input_length);
        StringReaderU8 reader(input, input_length);
        return convert(writer, reader);
    }

    bool ansi_from_u16(std::string& output, const char16_t *input, size_t input_length)
    {
        StringWriterA writer(output);
        writer.reserve(input_length);
        StringReaderU16 reader(input, input_length);
        return convert(writer, reader);
    }

    bool ansi_from_u32(std::string& output, const char32_t *input, size_t input_length)
    {
        StringWriterA writer(output);
        writer.reserve(input_length);
        StringReaderU32 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u8_from_ansi(std::string& output, const char *input, size_t input_length)
    {
        StringWriterU8 writer(output);
        writer.reserve(input_length);
        StringReaderA reader(input, input_length);
        return convert(writer, reader);
    }

    bool u8_from_u8 (std::string& output, const char *input, size_t input_length)
    {
        StringWriterU8 writer(output);
        writer.reserve(input_length);
        StringReaderU8 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u8_from_u16(std::string& output, const char16_t *input, size_t input_length)
    {
        StringWriterU8 writer(output);
        writer.reserve(input_length);
        StringReaderU16 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u8_from_u32(std::string& output, const char32_t *input, size_t input_length)
    {
        StringWriterU8 writer(output);
        writer.reserve(input_length);
        StringReaderU32 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u16_from_ansi(std::u16string& output, const char *input, size_t input_length)
    {
        StringWriterU16 writer(output);
        writer.reserve(input_length);
        StringReaderA reader(input, input_length);
        return convert(writer, reader);
    }

    bool u16_from_u8(std::u16string& output, const char *input, size_t input_length)
    {
        StringWriterU16 writer(output);
        writer.reserve(input_length);
        StringReaderU8 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u16_from_u16(std::u16string& output, const char16_t *input, size_t input_length)
    {
        StringWriterU16 writer(output);
        writer.reserve(input_length);
        StringReaderU16 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u16_from_u32(std::u16string& output, const char32_t *input, size_t input_length)
    {
        StringWriterU16 writer(output);
        writer.reserve(input_length);
        StringReaderU32 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u32_from_ansi(std::u32string& output, const char *input, size_t input_length)
    {
        StringWriterU32 writer(output);
        writer.reserve(input_length);
        StringReaderA reader(input, input_length);
        return convert(writer, reader);
    }

    bool u32_from_u8(std::u32string& output, const char *input, size_t input_length)
    {
        StringWriterU32 writer(output);
        writer.reserve(input_length);
        StringReaderU8 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u32_from_u16(std::u32string& output, const char16_t *input, size_t input_length)
    {
        StringWriterU32 writer(output);
        writer.reserve(input_length);
        StringReaderU16 reader(input, input_length);
        return convert(writer, reader);
    }

    bool u32_from_u32(std::u32string& output, const char32_t *input, size_t input_length)
    {
        StringWriterU32 writer(output);
        writer.reserve(input_length);
        StringReaderU32 reader(input, input_length);
        return convert(writer, reader);
    }
} //namespace khmz
