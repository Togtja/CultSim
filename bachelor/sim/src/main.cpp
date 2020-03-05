#include "application.h"
#include "constants.h"

#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <spdlog/sinks/stdout_color_sinks.h>

int main(int argc, char* argv[])
{
    const auto args = std::vector(argv, argv + argc);

    auto lua_logger        = spdlog::stdout_color_st("lua");
    auto default_logger    = spdlog::stdout_color_st("default");
    auto input_logger      = spdlog::stdout_color_st("input");
    auto graphics_logger   = spdlog::stdout_color_st("graphics");
    auto filesystem_logger = spdlog::stdout_color_st("filesystem");
    auto scenario_logger   = spdlog::stdout_color_st("scenario");
    auto test_logger       = spdlog::stdout_color_st("test");

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
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    spdlog::set_pattern("[%D | %H:%M:%S.%e] [%^%n %l%$] - %v");
    spdlog::set_default_logger(default_logger);

    /** Run the program */
    cs::Application cultsim{};
    cultsim.run(args);

    return 0;
}
