// CSTR_dump_tests.cpp --- The tests for CSTR_dump
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#define UTFIO_CODEPAGE 932
#define UTFIO_CLASSES
#define CSTR_DUMP_DETAIL
#include "CSTR_dump.h"

int main(void)
{
    using namespace khmz;

    // ANSI reading
#ifdef UTFIO_SUPPORT_ANSI
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderA a_input("a_input.txt");
        FileWriterA a_2_a_output("a_2_a_output.txt");
        CSTR_dump(a_2_a_output, a_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderA a_input("a_input.txt");
        FileWriterU8 a_2_u8_output("a_2_u8_output.txt");
        CSTR_dump(a_2_u8_output, a_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderA a_input("a_input.txt");
        FileWriterU16 a_2_u16_output("a_2_u16_output.txt");
        CSTR_dump(a_2_u16_output, a_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderA a_input("a_input.txt");
        FileWriterU16BE a_2_u16be_output("a_2_u16be_output.txt");
        CSTR_dump(a_2_u16be_output, a_input, flags);
    }
#endif

    // UTF-8 reading
#ifdef UTFIO_SUPPORT_ANSI
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU8 u8_input("u8_input.txt");
        FileWriterA u8_2_a_output("u8_2_a_output.txt");
        CSTR_dump(u8_2_a_output, u8_input, flags);
    }
#endif
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU8 u8_input("u8_input.txt");
        FileWriterU8 u8_2_u8_output("u8_2_u8_output.txt");
        CSTR_dump(u8_2_u8_output, u8_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU8 u8_input("u8_input.txt");
        FileWriterU16 u8_2_u16_output("u8_2_u16_output.txt");
        CSTR_dump(u8_2_u16_output, u8_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU8 u8_input("u8_input.txt");
        FileWriterU16BE u8_2_u16be_output("u8_2_u16be_output.txt");
        CSTR_dump(u8_2_u16be_output, u8_input, flags);
    }

    // UTF-16 reading
#ifdef UTFIO_SUPPORT_ANSI
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU16 u16_input("u16_input.txt");
        FileWriterA u16_2_a_output("u16_2_a_output.txt");
        CSTR_dump(u16_2_a_output, u16_input, flags);
    }
#endif
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU16 u16_input("u16_input.txt");
        FileWriterU8 u16_2_u8_output("u16_2_u8_output.txt");
        CSTR_dump(u16_2_u8_output, u16_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU16 u16_input("u16_input.txt");
        FileWriterU16 u16_2_u16_output("u16_2_u16_output.txt");
        CSTR_dump(u16_2_u16_output, u16_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU16 u16_input("u16_input.txt");
        FileWriterU16BE u16_2_u16be_output("u16_2_u16be_output.txt");
        CSTR_dump(u16_2_u16be_output, u16_input, flags);
    }

    // UTF-16 BE reading
#ifdef UTFIO_SUPPORT_ANSI
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU16BE u16be_input("u16be_input.txt");
        FileWriterA u16be_2_a_output("u16be_2_a_output.txt");
        CSTR_dump(u16be_2_a_output, u16be_input, flags);
    }
#endif
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU16BE u16be_input("u16be_input.txt");
        FileWriterU8 u16be_2_u8_output("u16be_2_u8_output.txt");
        CSTR_dump(u16be_2_u8_output, u16be_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU16BE u16be_input("u16be_input.txt");
        FileWriterU16 u16be_2_u16_output("u16be_2_u16_output.txt");
        CSTR_dump(u16be_2_u16_output, u16be_input, flags);
    }
    {
        int flags = CSTR_DUMP_ESCAPE_NOCONV;
        FileReaderU16BE u16be_input("u16be_input.txt");
        FileWriterU16BE u16be_2_u16be_output("u16be_2_u16be_output.txt");
        CSTR_dump(u16be_2_u16be_output, u16be_input, flags);
    }

    return 0;
}
