# CultSim

CultSim comes from Culture and Simulation and not “Cult” as in Worship, although both Cult and Culture come from the same Etymology of Cultivate (Cultus in Latin).

## Core Developers

* Leon Cinquemani
* Tomas Himberg Berger
* Carl Findahl

## What is CultSim

CultSim is a social simulation framework in the Agent-Based Modelling family, with a focus on performance, high-quality visualizations, and a flexible scripting environment.

It is intended as an easily extendable platform, on which complex simulations concerning the interactions between agents can be developed by researchers and developers alike. The finished simulations should be usable by non-developers as well if they are interested in observing the interactions between agents. Once simulations have been run, the collected data will be available to the user for further analysis in a program of their choice.

The [Lua scripting language](https://www.lua.org/) is embedded to provide users with the ability to implement their own scenarios, agents, and systems that govern their actions and decision making, striving for users to be able to customize nearly 100% of what makes up a simulation. Lua is a fast and lightweight language designed to be embedded in other programs and is relatively quick to learn. Using the Lua API in CultSim, developers can modify existing simulations, or create new ones!

## Installation

You have two options when installing CultSim, you can download the [binary](https://drive.google.com/open?id=1vXFqkYH1JzuLAZkPGG5rCkCZCM2sN2EX) that allows for Lua API, and normal use of the program. However, if you feel like CultSim is limiting you in any way, or there are some weird behaviors, you might want to build it from the source. If you do add more features or fix some bugs, we would be happy if you created push requests with the changes, so that we might include them in future versions of CultSim.

### Binary

CultSim is compiled as a 64-bit binary and thus is only supported as such. If you wish for a 32-bit binary you need to compile it yourself, and you will need to fetch the 32-bit versions of the applicable libraries CultSim depends on.

#### Windows
For most users, unzipping the binary `.zip` file, running [vc_redist.x64.exe](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads), and making sure your graphics card is up to date should be all that is needed. However we have come across an issue with AMD graphics card needing some additional dependency, however, we have yet to isolate what that is, so post an issue if you can’t get it up and running.

#### Linux
On Linux, using a precompiled binary is currently not recommended as we can not guarantee that we are linking against the same version of the system libraries as on the distro we are building on. Therefore, you are advised to build from source, which should be relatively easy and not take too long.

### From Source

To Compile CultSim from source CMake 3.15 is needed.

#### Linux
Before you start, ensure you have the following system libraries installed (on Ubuntu):

`git`, `make`, `lua5.3`, `lua5.3-dev`, `libx11-dev`, `libxext-dev`, `clang`, `libphysfs-dev`, `libtbb-dev`, `wget`, `mesa-common-dev`, `libgl1-mesa-dev` using apt. You might not need the mesa GL drivers if you are using an Nvidia or AMD GPU. These can be gotten using apt.

1. Clone from [Git](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor.git)
2. `cd` into the root directory where you cloned
3. `mkdir build`
4. `cd build`
5. `CMake -G "Unix Makefiles" ../``
6. `make -j#` where the `#`  is the number of threads you want to compile with
 
To run the program after building assuming you are coming right from the previous steps:

1. `cd bachelor/sim`
2. `./cultsim`


#### Windows
1. Clone from [Git](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor.git)
2. Generate the project files using CMake 3.15 or later
     * If you are using the GUI version, browse to the source and build directories you want at the top of the interface. The source directory should be the folder you cloned, which contains the root `CMakeLists.txt` file.
    * Set any parameters you want in the main window.
    * Then click `Configure` at the bottom left of the UI and if you are using Visual Studio, choose a 64-bit version.
    * Once the configure stage has finished, click `Generate`, and finally `Open Project`.
4. Go to `(Folder you cloned the project into)/out/build/(the version you built, debug or release)/_deps/sdl2_external-build`
    * Some CMake configurations do not automatically differentiate between Release/Debug, so you might find it directly in the build folder.
5. Copy the `SDL2.dll` file
6. Go to `(Folder you cloned the project into)/out/build/(the version you built, debug or release)/bachelor/sim`
    * See above for CMake config specific changes to this step
7. Paste the `.dll` file here
8. repeat steps 3-6 for `physfs.dll` in `(Folder you cloned the project into)/out/build/(the version you built, debug or release)/_deps/physfs_external-build`
9. Build the project

## Usage

To get started writing scenarios with CultSim you need to use Lua and follow a basic format. You can look at the bundled basic needs scenario, or start from scratch on your own. Regardless, the documentation would be nice to have access to. If you’re building from source you can find the Lua scripting API documentation by enabling `CULTSIM_BUILD_DOCS` in the CMake configuration. This will build the documentation with Doxygen.

> **NOTE**: Currently, the Lua documentation is incomplete and needs more work to be a complete source of information. The C++ documentation is almost complete though, so you can create simulations in C++ too if you want.

To run simulations in Cultsim all you need to do is to start the program and choose the “Load Scenario”-tab. From here, the user should see a list of all Scenarios they currently have in their `\data\script\scenarios` folder. They can choose a scenario by clicking one of these. Alternatively, they may also click the “load options”-tab in the “Load Scenario”-tab to change the random seed for the simulation, or to enable or disable visualization for the simulation.

From the main menu, they may also rebind the controls via the “Keybindings”-tab. Here they may add or remove key bindings from a different context, such as “Scenario Scene”, which would be the keybindings used during the simulation.

Finally, also on the main menu, is the option to exit the program entirely via the “Exit”-tab.


## Controls

The default control scheme for CultSim is relatively small:
* `WASD` or `arrow keys` to move the camera/viewport around
* `Q` or `Scroll up` to zoom the camera/viewport in
* `E` or `Scroll down` to zoom the camera/viewport out
* `Mouse Button 1` to select an entity
* `F` to follow a selected entity around
* `P` to open up a pause menu
* `Space Bar` to pause a simulation
* `.` to multiply the current simulation speed by 2
* `,` to divide the current simulation speed by 2
* `F2` to go to 2D mode
* `F3` to go to 3D mode
* `Escape` to leave a menu or the program itself, depending on if you have a menu open or not
 
CultSim supports what some may find overly customizable keybindings, and it can be an issue to set-up custom ones. Though once the Keybinding UI is made sense of, you will find it to be very customizable.

The Keybinding file can be found on Windows in:
`C:\Users\Your_User_Name\AppData\Roaming\g107\cultsim`

And on Linux:
`~/.local/share/cultsim`

## 3rd party libraries used in CultSim

[OpenGL 4.5](https://www.opengl.org/)
* Graphics API used for visualizing the simulations in CultSim in an efficient manner

[Robin Hood Hashing](https://github.com/martinus/robin-hood-hashing)
* Used as a replacement to std::unordered_map as this implementation of a hash map is a lot faster, and the difference could not be neglected when measuring

[SDL](https://www.libsdl.org/)
* Used to create windows, and processing events from the operating system

[Lua](https://www.lua.org/)
* Our scripting language of choice

[Google's Benchmark Libary](https://github.com/google/benchmark)
* Used internally for benchmarking Lua vs Python and other micro-benchmarks underway

[Spdlog](https://github.com/gabime/spdlog)
* Flexible logging library used for error messages and general logging

[Sol](https://github.com/ThePhD/sol2)
* Used to create the Lua bindings for CultSim

[Dear ImGui](https://github.com/ocornut/imgui)
* Immediate mode GUI library that is fast and easy to use. Used for our UI

[ImPlot](https://github.com/epezent/implot)
* Plotting extension library for Dear ImGui

[EnTT](https://github.com/skypjack/entt)
* Used as the entity-component-system for the project due to high performance and ease of use

[PhysFs](https://icculus.org/physfs/)
* Virtual file system library to make our paths platform-agnostic and restrict write access

[PCG Random](https://www.pcg-random.org/)
* A faster, smaller, and less predictable random number generator

[CPP Taskflow](https://github.com/cpp-taskflow/cpp-taskflow)
* A task-oriented multithreading scheduling library to help us data-parallelize certain tasks in CultSim

[GLM](https://glm.g-truc.net/0.9.9/index.html)
* OpenGL Mathematics library for vector and matrix computations

[doctest](https://github.com/onqtam/doctest)
* Fast and lightweight testing framework with a good CMake integration