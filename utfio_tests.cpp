// utfio_tests.h --- The tests for UTF IO
// License: MIT
//////////////////////////////////////////////////////////////////////////////

#define UTFIO_CODEPAGE 932
#define UTFIO_CLASSES
#include "utfio.h"
#include <cassert>

int main(void)
{
    using namespace khmz;

#ifdef UTFIO_SUPPORT_ANSI
    // to ansi
    {
        std::string output;
        ansi_from_ansi(output, "khmz");
        assert(output == "khmz");
    }
    {
        std::string output;
        ansi_from_u8(output, "khmz");
        assert(output == "khmz");
    }
    {
        std::string output;
        ansi_from_u16(output, u"khmz");
        assert(output == "khmz");
    }
    {
        std::string output;
        ansi_from_u32(output, U"khmz");
        assert(output == "khmz");
    }
#endif

    // to u8
#ifdef UTFIO_SUPPORT_ANSI
    {
        std::string output;
        u8_from_ansi(output, "khmz");
        assert(output == "khmz");
    }
#endif
    {
        std::string output;
        u8_from_u8(output, "khmz");
        assert(output == "khmz");
    }
    {
        std::string output;
        u8_from_u16(output, u"khmz");
        assert(output == "khmz");
    }
    {
        std::string output;
        u8_from_u32(output, U"khmz");
        assert(output == "khmz");
    }

    // to u16
#ifdef UTFIO_SUPPORT_ANSI
    {
        std::u16string output;
        u16_from_ansi(output, "khmz");
        assert(output == u"khmz");
    }
#endif
    {
        std::u16string output;
        u16_from_u8(output, "khmz");
        assert(output == u"khmz");
    }
    {
        std::u16string output;
        u16_from_u16(output, u"khmz");
        assert(output == u"khmz");
    }
    {
        std::u16string output;
        u16_from_u32(output, U"khmz");
        assert(output == u"khmz");
    }

    // to u32
#ifdef UTFIO_SUPPORT_ANSI
    {
        std::u32string output;
        u32_from_ansi(output, "khmz");
        assert(output == U"khmz");
    }
#endif
    {
        std::u32string output;
        u32_from_u8(output, "khmz");
        assert(output == U"khmz");
    }
    {
        std::u32string output;
        u32_from_u16(output, u"khmz");
        assert(output == U"khmz");
    }
    {
        std::u32string output;
        u32_from_u32(output, U"khmz");
        assert(output == U"khmz");
    }

#ifdef UTFIO_SUPPORT_ANSI
    // ANSI reading
    {
        FileReaderA a_input("a_input.txt");
        FileWriterA a_2_a_output("a_2_a_output.txt");
        convert(a_2_a_output, a_input);
    }
    {
        FileReaderA a_input("a_input.txt");
        FileWriterU8 a_2_u8_output("a_2_u8_output.txt");
        convert(a_2_u8_output, a_input);
    }
    {
        FileReaderA a_input("a_input.txt");
        FileWriterU16 a_2_u16_output("a_2_u16_output.txt");
        convert(a_2_u16_output, a_input);
    }
    {
        FileReaderA a_input("a_input.txt");
        FileWriterU16BE a_2_u16be_output("a_2_u16be_output.txt");
        convert(a_2_u16be_output, a_input);
    }
#endif

    // UTF-8 reading
#ifdef UTFIO_SUPPORT_ANSI
    {
        FileReaderU8 u8_input("u8_input.txt");
        FileWriterA u8_2_a_output("u8_2_a_output.txt");
        convert(u8_2_a_output, u8_input);
    }
#endif
    {
        FileReaderU8 u8_input("u8_input.txt");
        FileWriterU8 u8_2_u8_output("u8_2_u8_output.txt");
        convert(u8_2_u8_output, u8_input);
    }
    {
        FileReaderU8 u8_input("u8_input.txt");
        FileWriterU16 u8_2_u16_output("u8_2_u16_output.txt");
        convert(u8_2_u16_output, u8_input);
    }
    {
        FileReaderU8 u8_input("u8_input.txt");
        FileWriterU16BE u8_2_u16be_output("u8_2_u16be_output.txt");
        convert(u8_2_u16be_output, u8_input);
    }

    // UTF-16 reading
#ifdef UTFIO_SUPPORT_ANSI
    {
        FileReaderU16 u16_input("u16_input.txt");
        FileWriterA u16_2_a_output("u16_2_a_output.txt");
        convert(u16_2_a_output, u16_input);
    }
#endif
    {
        FileReaderU16 u16_input("u16_input.txt");
        FileWriterU8 u16_2_u8_output("u16_2_u8_output.txt");
        convert(u16_2_u8_output, u16_input);
    }
    {
        FileReaderU16 u16_input("u16_input.txt");
        FileWriterU16 u16_2_u16_output("u16_2_u16_output.txt");
        convert(u16_2_u16_output, u16_input);
    }
    {
        FileReaderU16 u16_input("u16_input.txt");
        FileWriterU16BE u16_2_u16be_output("u16_2_u16be_output.txt");
        convert(u16_2_u16be_output, u16_input);
    }

    // UTF-16 BE reading
#ifdef UTFIO_SUPPORT_ANSI
    {
        FileReaderU16BE u16be_input("u16be_input.txt");
        FileWriterA u16be_2_a_output("u16be_2_a_output.txt");
        convert(u16be_2_a_output, u16be_input);
    }
#endif
    {
        FileReaderU16BE u16be_input("u16be_input.txt");
        FileWriterU8 u16be_2_u8_output("u16be_2_u8_output.txt");
        convert(u16be_2_u8_output, u16be_input);
    }
    {
        FileReaderU16BE u16be_input("u16be_input.txt");
        FileWriterU16 u16be_2_u16_output("u16be_2_u16_output.txt");
        convert(u16be_2_u16_output, u16be_input);
    }
    {
        FileReaderU16BE u16be_input("u16be_input.txt");
        FileWriterU16BE u16be_2_u16be_output("u16be_2_u16be_output.txt");
        convert(u16be_2_u16be_output, u16be_input);
    }

    std::puts("ok");
    return 0;
}
