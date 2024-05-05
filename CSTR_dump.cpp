// CSTR_dump.cpp --- C string literal dumper
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#define UTFIO_CLASSES
#define CSTR_DUMP_DETAIL
#include "CSTR_dump.h"

namespace khmz
{
#ifdef UTFIO_SUPPORT_ANSI
    bool CSTR_dump_ansi(std::string& output, const char *input, size_t input_length, int& flags)
    {
        StringWriterA writer(output);
        writer.reserve(input_length);
        StringReaderA reader(input, input_length);
        return CSTR_dump(writer, reader, flags);
    }
#endif
    bool CSTR_dump_u8(std::string& output, const char *input, size_t input_length, int& flags)
    {
        StringWriterU8 writer(output);
        writer.reserve(input_length);
        StringReaderU8 reader(input, input_length);
        return CSTR_dump(writer, reader, flags);
    }
    bool CSTR_dump_u16(std::u16string& output, const char16_t *input, size_t input_length, int& flags)
    {
        StringWriterU16 writer(output);
        writer.reserve(input_length);
        StringReaderU16 reader(input, input_length);
        return CSTR_dump(writer, reader, flags);
    }
    bool CSTR_dump_u32(std::u32string& output, const char32_t *input, size_t input_length, int& flags)
    {
        StringWriterU32 writer(output);
        writer.reserve(input_length);
        StringReaderU32 reader(input, input_length);
        return CSTR_dump(writer, reader, flags);
    }
} // namespace khmz
