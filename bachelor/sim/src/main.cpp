#include "application.h"
#include "constants.h"

#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

int main(int argc, char* argv[])
{
    const auto args = std::vector(argv, argv + argc);

    /** Disable logs before running tests as we don't want them to pollute test information */
    spdlog::set_level(spdlog::level::off);

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

    /** Based on DEBUG or not, set the log level thereafter */
    if constexpr (CULTSIM_DEBUG)
    {
        spdlog::set_level(spdlog::level::debug);
    } else
    {
        spdlog::set_level(spdlog::level::info);
    }

    /** Run the program */
    cs::Application cultsim{};
    cultsim.run(args);

    return 0;
}
