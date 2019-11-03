#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#include <doctest/doctest.h>

int main(int argc, char** argv)
{
    doctest::Context test(argc, argv);
    return test.run();
}
