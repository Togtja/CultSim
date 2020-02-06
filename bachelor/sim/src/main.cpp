#include "application.h"

#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

int main(int argc, char* argv[])
{
    const auto args = std::vector(argv, argv + argc);

    /** Set up testing context and set some default options */
    doctest::Context context{};
    context.setOption("abort-after", 5);
    context.setOption("no-breaks", true);
    context.applyCommandLine(args.size(), args.data());

    /** Check if we still want to run the program */
    int res = context.run();
    if (context.shouldExit())
    {
        return res;
    }

    /** Run the program */
    cs::Application cultsim{};
    cultsim.run(args);

    return 0;
}
