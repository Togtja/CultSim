#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#include <doctest/doctest.h>

int main(int argc, char** argv)
{
    doctest::Context test{};
    test.setOption("abort-after", 5);
    test.setOption("no-breaks", true);
    test.applyCommandLine(argc, argv);
    return test.run();
}
