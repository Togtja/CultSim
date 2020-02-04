#include <cultsim/application.h>

#include <vector>

int main(int argc, char* argv[])
{
    const auto args = std::vector(argv, argv + argc);

    cs::Application cultsim{};
    cultsim.run(args);

    return 0;
}
