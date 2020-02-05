#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#include <doctest/doctest.h>

int main(int argc, char** argv)
{
    doctest::Context context(argc, argv);
    int res = context.run();

    if (context.shouldExit())
    {
        return res;
    }

    return res;
}
