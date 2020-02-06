#include "application.h"

#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

int main(int argc, char* argv[])
{
    const auto args = std::vector(argv, argv + argc);

    doctest::Context context(args.size(), args.data());
    int res = context.run();

    if (context.shouldExit())
    {
        return res;
    }

    cs::Application cultsim{};
    cultsim.run(args);

    return 0;
}
