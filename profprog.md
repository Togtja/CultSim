# Professional Programming - G107

These are the group members of Group 107:

| Name            | NTNU Name |
| --------------- | --------- |
| Tomas Berger    | tomashb   |
| Leon Cinquemani | leonnc    |
| Carl Findahl    | carlhf    |

These are all the repositories within our group namespace. The only one that is relevant is the Main Repo as the rest are just supporting tools we created for ourselves.

| Repo                 | Link                                                                            |
| -------------------- | ------------------------------------------------------------------------------- |
| Main Repo            | https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor                    |
| Lua Python Benchmark | https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/lua-and-python-benchmarking |
| Room Booker          | https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/s413                        |
| Room Booking Backend | https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/s413-backend                |


## Group Discussion

### Language Discussion

#### Lua
We decided to use Lua as our scripting language, to allow end-users to communicate with our framework. Lua allows us to expose C++ code to the end-users, and allow them to create their own data structures in Lua to use in their simulations. We struggled between Lua and Python as the scripting language but ended on Lua due to its better performance over Python. We knew Lua would perform better, just not by how much. A good chunk of our thesis is about the decision to use Lua over Python, and if you want to get a better overview as to why we decided this, we recommend reading "3.3 Scripting" page 57 in our Bachelor thesis, found [here](https://drive.google.com/open?id=1SDrRfxnVoPVqjJFiC55-w0xvkP7yrCkr) 

Lua is very lightweight, only under 100kb, meaning to include the whole language in our project comes with nearly no overhead. On the downside, this means that everything we want to be done, we most likely have to do ourselves. Especially considering the limited amount of 3rd party libraries for tools such as data visualization, and AI. Lua is also supposed to live on top of a primary programming language, this typically comes with the benefit of ease of communication between host language and Lua, in our case C++ and Lua. This worked very well for us, we found it easy to bind Lua with C++ and vice-versa.

Seeing as Lua's only true data structure is tables, it can make the language both easy and hard to use. For newcomers it makes is much easier to get started, as you only need to think about one data structure. For the more seasoned programmer you might find the need for additional data structures, but you will have to implement them yourself. Still, the tables in Lua are extremely powerful, especially when you add metatables into the equation. With metatables, you can (as an advanced programmer) create linked lists and read-only data types, and also fake an object-oriented approach to programming in Lua. However, there are libraries that introduce other data structures into Lua, as an example the object-oriented libary we included, found [here](https://github.com/kikito/middleclass), or  [this one for Linked lists](https://gist.github.com/LouiseBC/692bfd907d9e0527b471947d540dee6c).

In the gaming industry, Lua is a very common scripting language to come across, while we are making a simulation framework, this has several parallels to games, meaning Lua very much has a place here as well. It is an industry-standard, which means that there is a lot of help to get online and in books.

We also needed something that did not need to compile for every time we made a change to the script, meaning we needed a scripting language that was dynamically typed. Based on research into this, we found that Lua was one of the fastest dynamically typed scripting languages. [source](https://lukas-boersma.com/en/blog/2016-10-01-choosing-scripting-language)

We had a big issue with debugging Lua code, as we had no linter or pre-compiler to tell us if the Lua code would even run before starting up CultSim to then let CultSim's Lua interpreter check it. This meant we spent a lot of time writing Lua, running CultSim, get the first of potentially many errors from CultSim, go back, fix the error, and repeat. One of the reasons for this is that we did not use [luac](https://www.lua.org/manual/5.1/luac.html), which is a compiler/pre-compiler for Lua. Instead we were just using Cultsim's Lua interpreter, as we did not include the compiler into CultSim.


#### C++

The main language used in our project is C++, as that is used for the core framework for CultSim. As a high-level overview, we chose to work with this language because it allowed us to take full control of our computer at a relatively low level, while still being capable of creating higher-level abstractions that made it easier for our developers to manage complex tasks. At the same time, we achieved very good performance out of the box, while also being able to fine-tune and micro-optimize hot parts of the code based on measurements to squeeze out as much as possible from the underlying hardware. Additionally, C++ has great support for OpenGL and other graphics APIs which was central to the development of our project.

For a beginner, C++ can be a daunting language that is often portrayed as scary and hard to learn. We understand where that prejudice comes from, as it is a big language that puts a lot of responsibility on the users. However, the base language itself is not necessarily that hard to learn. By that, we mean general concepts such as functions, variables, and classes. If that is all you use, learning C++ is not going to be much harder than what you need to do with C. You could call it C with classes. That being said, C++ is a lot more than that, but only when you start adding the other "parts" of the language. At the core, you have the part that is C with classes. Here you use the core ideas from C, but you also have the concept of `private`, `protected` and `public` visibility specifiers within `struct`s and `class`es, and you can inherit from other `class`es and `struct`s. However, C++ also has a massive standard library with a lot of utilities like dynamically resizeable arrays (`vector`), hash maps (`unordered-map`), smart pointers (`unique-ptr`/`shared-ptr`) and much more. This is itself such a huge library, that learning to use it well can be almost as time-consuming as learning the base language itself. You also have templates, which is a compile-time construct to help you write generic code. The templates in C++ are almost like a whole new language as templates are Turing complete ("[A Turing Complete system means a system in which a program can be written that will find an answer](https://stackoverflow.com/questions/7284/what-is-turing-complete)"). Also, you have the complexity and different way of thinking that comes with a non-garbage collected and low-level language such as C++. You can't just throw `new`'s everywhere and assume that it is freed automatically. Likewise, you have to make sure memory layouts are efficient, so the CPU can access the data without too many cache misses if you want fast programs. The same goes for pointers, indirections, and object lifetimes. Unlike many other languages, you need to be explicit and know why and what you are doing to write maintainable and efficient code with C++. Finally, since the language is really old, finding good and up to date learning materials is not always easy, and many online sources can be outright wrong or completely outdated. All these factors together are what we believe makes C++ such a challenging language to learn and adapt for beginners. But at the core, it should be no harder than, for example, Java. Likewise, if you learn it well from the beginning, using good resources with detailed explanations you will gain a strong understanding of not only many programming paradigms but also of the computer and hardware that lies underneath.

A weakness of the language is related to the high threshold of getting started as just discussed, and later, mastering it. As C++ can be used in so many different ways (graphics, embedded, games, GUI applications, functionally, object-oriented) it is hard to master everything.  You could consider this a strength as well since the language is very diverse and can be used for more or less anything. Also, since it is built on C, it is not unusual to see C++ libraries with a C-interface be bound to many other languages as well. At the same time, we have all the different versions of C++ that are currently active (97, 03, 11, 14, 17, 20). These versions all have subtle and big differences between them, and being able to remember all of that as you move between projects can be hard. At the same time, C++ aims to keep backward compatibility, so code written in a C\++97 style should still compile with C\++20. This is great for long-living projects which are mostly being maintained, as they can opt-in to new features without breaking the functionality of their legacy code. However, it's not necessarily good for the future development of the language, as design errors and other flaws from earlier versions are much harder to remove. Instead, replacement features must be added, or a long deprecation process must be started. An example of this is [`auto_ptr`](https://en.cppreference.com/w/cpp/memory/auto_ptr) that was introduced as an early attempt at creating a smart pointer before C\++11 came out. As C\++11 came around with `unique_ptr`, the `auto_ptr` was deprecated and deemed completely useless. This was because C\++11 introduced the concept of move-only types, which was the only way to properly define the ownership model required by a `unique_ptr`. It has since been removed in C\++17 (6 years after deprecation).

C++ is a language that evolves through a committee that is responsible for discussing possible additions or improvements to the language, submitted by active users, companies, or others in the community. They hold meetings several times per year where they discuss and give feedback on such papers, and eventually add or reject them from the next language version. When a new version of C++ is ready, it becomes an [ISO  standard](https://www.iso.org/standard/68564.html). This approach means the language takes long to evolve, and iteration time is slow, but the quality of the specification should be much better once released. For businesses, this can be good since they will know what to expect and know that as long as they use a certain standard, that is the way it will be.  Compilers are also implemented in terms of the standard.

When it comes to features, C++ can be used as a functional language through concepts such as lambdas and pass-by-value. It can also be object-oriented with inheritance, and access specifiers in `class`es and `struct`s, or it can be generic through the use of templates. It is a very imperative language, in that you need to tell it exactly how to do its tasks, although you can create some declarative styles with clever use of macros, constructors/destructors, and templates. It has a huge standard library with many tools and utilities to help you stay productive on new projects. However, since it is very generic, many users will implement their own versions of some concepts, such as strings or vectors, that manage memory more optimized for their applications. Games are a prime example, as memory allocations can be very expensive if used carelessly. Also, some features in the STL can be quite verbose compared to  other languages:

```cpp
// C++
std::sort(vector.begin(), vector.end());
```

```python
# Python
list.sort()
```

Templates in C++ can help you write very generic code, and not repeat yourself as much as without them. However, extensive template usage will greatly increase compile times of your project as the compiler is the one that reads the templates and stamps out code for specific specializations of that template. The more variations of a template, and the more complex the template, the bigger the binary, and the longer the compile times become. However, the fact that this is created at compile time also lets you use templates to gain big performance boosts by doing some computations at compile time. This got much better in C\++11 with the concept of `constexpr` functions. These functions must adhere to certain rules, but in return, they could be evaluated at compile-time, saving runtime performance! In our project, we used templates quite extensively through our dependencies, and also created some of our own to avoid writing the same class or function more than once for every possible specialization:

```cpp
// From auto_timer.h, line 79
/**
 * Measure the execution time of a single callable
 *
 * @param func The callable to measure
 * @param args The arguments to pass to the callable
 * @tparam Callable The type of the function
 * @tparam Args The argument types for the function
 * @return The execution time in milliseconds
 */
template<typename Callable, typename... Args, typename = std::enable_if_t<std::is_invocable_v<Callable, Args...>>>
double measure_callable(Callable&& func, Args&&... args)
{
    const auto before = std::chrono::steady_clock::now();
    std::invoke(func, std::forward<Args>(args)...);
    const auto after = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(after - before).count();
}
```

For example, the function above is used to measure the execution time of any callable object. Using templates here has been great since we can use this function on *any* callable object, whether they are functions, classes, or lambdas, and regardless of function signatures. If we did not use a template here, we would have to create a new function for every type of function that we wanted to measure, effectively removing the purpose of the function as we would end up duplicating code regardless. We also block the use of this function if the callable you passed can not be invoked (called) with the parameters you tried to give it (`std::is_invocable_t`), yielding a compile-time error. In the end, templates can be powerful but are also hard to implement correctly for all use cases, so it is smart to restrict them where possible as in the example above, using [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae).

### Code review

Our code review process was largely unstructured, despite the original planning of the project calling for a day specifically for refactoring and going through code once per week. Instead, we ended up doing code review internally via Pair programming, which meant that code was written more correctly the first time around, rather than us having to come back and change it later down the line. In a way, it acted as a live code review, with one person pointing out improvements or flaws under way from behind your shoulder.

Additionally, throughout the development process we used the change tracker of GitLab's merge requests to check new code being implemented for any obvious errors and mistakes. While this relied on us having some comprehension of what the code was actually trying to achieve and was usually done relatively quickly to simply familiarize ourselves roughly with the changes made, it still helped to find formatting and spelling errors.

In addition to internal code review we also had help from our professor Simon McCallum, who over the course of the semester looked at the code of our project, and gave valuable feedback on our filesystem and input manager, both of which were critical systems for the program. Here Simon went through our code and looked at each line, checking for any errors in logic, or simply poor code. During the reviews, we had a team member, typically the main author of the code, discuss the feedback with the reviewer, and another one taking notes. The notes indicated what line the issue was on, and exactly what the issue was, for example ([note document](https://hackmd.io/@ArmPitPerson/SJR-bG_ML)):

 > Line 57, the debug message doesn't add valuable information to the user. Unclear what, or if anything went wrong

Once the notes had been taken and the review session had ended, the team gathered to review each possible issue, point by point. We discussed internally if it was worth fixing, and if it was, who should deal with it and what should be done. Once the fix was implemented we used the review strategy discussed in the Process Control section to ensure that everything had been resolved. Our routines for double checking this was a bit lacking as we had a greater focus on new features due to the scope of our project. If we had more team members or a smaller scope we would have spent more effort on conducting self-evaluation and review underway.


### Library Usage

All the libraries and dependencies that we needed for the development of CultSim were managed using [CMake](https://cmake.org/) and its [`FetchContent`](https://cmake.org/cmake/help/v3.15/module/FetchContent.html) module. This module allowed us to manage our dependencies universally on all platforms regardless of operating systems and compilers as long as the underlying libraries or dependencies supported that platform. [`FetchContent`](https://cmake.org/cmake/help/v3.15/module/FetchContent.html) is a relatively new addition to CMake (3.11+, March 2018), but a very valuable one for us. We set up template `.cmake` files for managing our dependencies that look like this:

```cmake
# Set up LIB by fetching it from a Git repo

# Declare where to find LIB and what version to use
FetchContent_Declare(
    LIB_external
    GIT_REPOSITORY https://github.com/link_to_lib_repo/LIB.git
    GIT_TAG v1.5.0
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(LIB_external)
```

What this does is declare an external library dependency using Git as the external source. We enter the repository of the library, and the tag or commit hash that we want to use. Using the `MakeAvailable` command later then prompts CMake to clone the repository and set up a build environment for that library. It is also possible to have CMake download libraries that are only distributed in the `.zip` or `.tar.gz` file formats, in which case it will download the archive, unzip it and then assume that the files within are the root of a source hierarchy. We did this with the SDL library:

```cmake
# Declare where to find SDL2 and what version to use
FetchContent_Declare(
    sdl2_external
    URL https://www.libsdl.org/release/SDL2-2.0.10.tar.gz
    URL_HASH SHA256=b4656c13a1f0d0023ae2f4a9cf08ec92fffb464e0f24238337784159b8b91d57
)
```

By specifying a hash we let CMake know not to re-download the archive unless the hash has changed, and if the downloaded file was tampered with, CMake will know since we provided a hash.

A great advantage of managing dependencies this way is that as long as you have a recent version of CMake, the project can be built on Windows, Mac, and Linux equally, and the dependency management process becomes the same on all platforms. You don't have to install packages on Linux or try to manually point to the libraries on Windows. You also do not need to use a custom package manager on Windows. The effort spent on dependencies is put on the developer of the software, rather than the user who just wants to build it. Even if you're a new developer, dependencies will be managed for you. Another benefit of this approach is that everyone will be using the same versions of the libraries and dependencies since it is specified with the archive hash or Git tag what exact version to use. For example, on Linux, users of different distros could have slightly different versions of SDL which could cause subtle bugs.

A downside is that building the project for the first time becomes more time consuming since this approach means users have to build all of these libraries from the source code to use them. Especially if you are a Linux user where you are used to just installing a package and then it is good to go for linking with, this can be a hassle. For Windows users it is a godsend in our opinion, as managing library dependencies on Windows can be quite a pain. In recent times, with tools such as [vcpkg](https://github.com/microsoft/vcpkg), it is easier to manage CMake dependencies on Windows as well, but it does depend on the package being available in the tool. Another downside is that this way of managing dependencies becomes a lot harder when a dependency does not use CMake as a build system. Since `FetchContent` will try to build with CMake by default, it can become a problem. It is possible to set it up working with other build systems, but the cross platform-ness of this approach becomes harder to guarantee at that point. Especially if a library uses `make`, which is not universally available on Windows. However, since most major C++ libraries use CMake, or offer CMake as a build system today, we were able to manage almost all of our libraries with the template mentioned before. Only a few libraries had to be manually created into a CMake library.

Overall, we are very pleased with using CMake for managing our dependencies. Since the group has Windows and Linux users, finding a good way to manage dependencies on both systems was hugely important to us. We think that this approach was very helpful in trying to achieve this as the build process is the same on all platforms now. All you need to do is to run CMake using your favorite interface (GUI / Terminal) on your platform, and it should build. On Windows, you still have to copy some `.dll` files from the library directories to the executable directory. We could not find an automatic solution for this, but overall it is a small price to pay for having dependencies managed this easily across OS boundaries.

### Process control

#### Extreme Scrum
For our project, we decided to use a combination of Scrum and Extreme Programming, creating a mixed development method we dubbed "Extreme Scrum".
This method borrowed the Pair programming, Continuous Integration and Smaller Release size from Extreme Programming, and the Sprints and structured Sprint meetings from scrum.

We wanted the flexibility, feeling of ownership, pair-programming and constant refactoring from Extreme Programming to be able to produce higher-quality code as well as make sure that all team members were up to date on the technologies used and code written.

Due to the Projects length and need for structure, it was decided that we would support this development model with some elements from Scrum to encourage a more structured and clear development cycle, such as sprints, clear roles for the team members, and frequent meetings to keep everyone up to date. 

Combined, these ensured a clear sense of progression, as well as clean, simple code that the entire team can understand due to their close involvement in the development process of each others code. The combined usage of the three Scrum meetings also helped strengthen the Communication and Feedback principles, which are important in Extreme Programming.

#### Gitlab Issues
To further structure out workflow, we utilized Scrums sprint and project backlog, represented via the "boards" feature in gitlabs Issue tracking. This allowed us to create individual Issues with their own prerequisites, estimated workhours, and associated categories. These were placed in the project backlog, with new issues being created at the end of each sprint to fill up the backlog when needed, as new Issues arose throughout the development process. Then, at the start of each new sprint, the sprint backlog was filled with Issues from the project backlog, and the team would do their best to complete all Issues for that sprint.

At the end of a sprint all Issues which had remained uncompleted where returned into the project backlog, and the team then chose their next sprint backlog from the Issues in the project backlog once more. Work on an issue was time tracked to see if we completed our issue in the allotted amount of time. This enabled us to increase the accuracy of future estimations, which aided in choosing the correct amount of Issues for each sprint.

Each commit was also linked to the issue that was being worked on in that commit, to enable tracking of what work went into fixing said issue, as well as who worked on the Issue at any given time.

#### CI/CD pipeline & Tests

Each Commit additionally had to make it's way through our CI-Pipeline, which attempted to build the project and run our tests to see if everything was working properly. This ensured the validity of our code being pushed before it could be merged into the master branch, stopping us from accidentally pushing broken code to master. 

Also the breaking of the tests usually indicated an improperly done refactor, which also was a great boon as it enabled us to properly update our code to work the same way before and after the refactoring. This aided us in saving time throughout the project by clearly indicating which part of the code breaks, allowing for more informed debugging in cases that would not have been caught by the compiler normally, such as vectors being ordered the wrong way around, for example.

Using the pipelines was not always a pleasant experience, however, as we experienced several issues throughout the project. For example, about a quarter way into the project we added a new library that we could not find on the OS that the pipeline used (Intel TBB). This was required *only* on Linux systems using C\++17's execution policies. It compiled fine on all our machines, since we used Windows and Arch Linux, which either did not need this library, or had the correct version. Since our pipeline used Ubuntu, it could not get a recent version of this library. As a response, we changed to Alpine, but found out that it did not support it either. Therefore, to avoid spending too much time on fixing the pipeline, we turned it off for a few sprints. This obviously removed our safety net with tests and code checks, but the teams code had so far been very functional when it came to launching and compiling, so we took that risk.

Later down the line towards the end of the project we reactivated the pipeline to aid us in the last few sprints, and used it to fix some errors that occured from some peoples compilers being more lenient on errors than others. At this point the pipeline had been refined to use more compiler warning flags, and also the dependency on the library that caused issues had been removed. The downside at this point was that the pipelines took a long time to compile due to our choice of dependency management as we discussed in [Library Usage](#Library-Usage), since we have to recompile all dependencies, and not only our project. Responding to this, the team set up a GitLab CI runner on one of their own laptops to be able to run several pipelines in parallel, and also to have more computing power available. Overall the pipeline would have probably worked better if we had more experience with creating them from before, but it still had a positive impact on the project overall, and we learned a lot about challenges that occur from using them. Especially since working with C++ and graphics was a bit of a pain to compile. But, for any new users on Ubuntu, they can now use the pipeline as documentation on how to install and run CultSim, as the commands needed to run the pipeline are in the `.gitlab-ci.yml` file from a brand new installation of Ubuntu!

#### Toggl

The final tool used for tracking our process was Toggl, an online time tracker which allows users to time themselves on different tasks, and stores the time spent. The Team used Toggl as a means to "clock in" during work hours, keeping track of how much time they spent working throughout the development process, and how close the team had gotten to its 90 work-hours per week goal. The tool also allows users to see the time spent working each day of the week, and across several weeks or even months.

Additionally, it allowed us to get an overview of total time spent working on the project, and if all users had followed the same naming scheme, it could also allow us to track time spent on certain tasks. We collectively made sure to use Toggl as much as possible throughout the project, both to train ourselves to keep watch of our work-hours, and to ensure that each group member spent the proper amount of time actually working. Sometimes, when people forgot to turn the timer on, the Time Watcher ensured that they logged their time after the fact and that they got the correct times entered, even if it was not as granular as it could have been. Overall, the time tracking was a nice boon to the team to keep track of what we did and see the stats evolving over time.

At the end of each sprint, the team would go through Toggl's report for the week, check which days they met their goals, and how much time had been spent on the project in total, which aided in decision making about work-hours, as well as increased motivation by seeing how much time had gone into the project collectively.

![toggl](https://i.imgur.com/6quUDoU.png)


#### Doxygen

To generate the documentation of CultSim we used a combination of Doxygen, Sphinx, and Breathe. Doxygen alone would be great, but does not create aesthetically pleasing documentation, and does not give enough control on the final organization as we would like. Therefore, we also used a combination of Sphinx and Breathe to give life to our documentation using the ReadTheDocs theme:

![cultsimdocs](https://i.imgur.com/GJDTwdf.png)

Sphinx uses a file format known as `.rst` (Re-Structured Text) that we use to write our documentation. The file lies in `docs/index.rst` when navigating from the root folder of our Main Repository. Here we can mix, match, and decide the layout of our documentation on our own. The process of generating the documentation has been completely automated using CMake, and can be enabled or disabled with a custom flag: `CULTSIM_BUILD_DOCS` that does the following:

```cmake
# Build Documentation if enabled
if(CULTSIM_BUILD_DOCS)
    add_subdirectory(docs)
endif()
```

Using this combination of automated and manual documentation we hope to provide readable and easily accessible documentation for CultSim. Technically, the documentation from Doxygen is also manual as the developers need to follow a strict commenting style, but the automation comes from laying it all out consistently and nicely in the final documentation. Having this level of control is great for the development process, as we can write tutorial-style sections without mixing documentation and code, and also ensure that the final docs are readable and logically laid out!

### Communication Channels
#### Discord
The main communication channel for the team was our own discord server, which was used as a hub for posting resources, screenshots, and messages, as well as our online chat tool of choice during the COVID-19 quarantine. Discord has flexible enough chat features to allow us to share our screens for impromptu pair programming, as well as an easy way for us to send files between group members. It was also used as a tool for communication with supervisors and outside sources on rare occasions, such as online user testing.


#### Zoom
Zoom was the online communication tool of choice for our supervisors throughout the quarantine and was used largely for meetings between developers and supervisors. It offered much of the same functionality as Discord, and was used much in the same way, but in an environment the supervisors felt more comfortable with. An argument can be made that Zoom is a more professional tool than Discord, but they offer much of the same functionality overall, so there is not much difference beyond the scope of how they are marketed.


### Code Style
* We used `clang-format` to format our code automatically for most cases, the config file can be found [here](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/.clang-format).
* Our code style found [here](https://drive.google.com/open?id=1eAfsVek-4wuHVmg_DCU-ajl8lKYHo0Nk).
   

## Individual Discussion

### Carl
       
#### Good Code

When picking a good piece of code that I wrote for this thesis I wanted to find something that helped me solve a task more efficiently and make that task more readable. Since I was working a lot with graphics I am going to discuss an abstraction of the creation of Vertex Array Objects (VAO) that I created. The idea of a vertex array is to hold the layout and format of the underlying vertex data so the vertex processor can interpret the meaning of the various vertex attributes when it enters the graphics pipeline. However, creating this array involves a lot of functions that repeat themselves, and if you need a lot of variations it could quickly become a lot of boilerplate. To solve this I thought about the builder pattern, and how that could be used to "build" a vertex array in a more intuitive and streamlined way that is also documented in the source, rather than in the OpenGL docs. The code in question can be found in the [header file](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/gfx/vao_builder.h) and [source file](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/gfx/vao_builder.cpp) of our main repository.

While it is not the most complex code, I think it enhances readability, and also reduces the chance for mistakes. If you forget to call `build`, for example, the builder object will delete the OpenGL object for you when it goes out of scope. However, when `build` has been called, the responsibility of the handle is given to the developer. Here is a simple example of creating a VAO with and without the builder:

```cpp
// ------------------
// With the builder
// ------------------
 m_vao = VaoBuilder()
  .attribute(0, 0, 3, GL_FLOAT, offsetof(SpriteVertex, pos))
  .attribute(1, 0, 2, GL_FLOAT, offsetof(SpriteVertex, tex_coord))
  .attribute(2, 1, 3, GL_FLOAT, offsetof(SpriteInstanceVertex, offset))
  .attribute(3, 1, 3, GL_FLOAT, offsetof(SpriteInstanceVertex, color))
  .iattribute(4, 1, 1, GL_UNSIGNED_INT, offsetof(SpriteInstanceVertex, texture))
  .divisor(1, 1)
  .ebo(m_vbo)
  .vbo(m_vbo, 0, size_bytes(indices), sizeof(SpriteVertex))
  .vbo(m_ivbo, 1, 0, sizeof(SpriteInstanceVertex))
  .bind()
  .build();

// ------------------
// Without the builder
// ------------------

/** Create VAO */
glCreateVertexArrays(1, &m_vao);

/** Format VAO*/
glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(SpriteVertex, pos));
glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(SpriteVertex, tex_coord));
glVertexArrayAttribFormat(m_vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(SpriteInstanceVertex, offset));
glVertexArrayAttribFormat(m_vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof(SpriteInstanceVertex, color));
glVertexArrayAttribIFormat(m_vao, 4, 1, GL_UNSIGNED_INT, offsetof(SpriteInstanceVertex, texture));

/** Binding VAO*/
glVertexArrayAttribBinding(m_vao, 0, 0);
glVertexArrayAttribBinding(m_vao, 1, 0);
glVertexArrayAttribBinding(m_vao, 2, 1);
glVertexArrayAttribBinding(m_vao, 3, 1);
glVertexArrayAttribBinding(m_vao, 4, 1);

glVertexArrayBindingDivisor(m_vao, 1, 1);

glEnableVertexArrayAttrib(m_vao, 0);
glEnableVertexArrayAttrib(m_vao, 1);
glEnableVertexArrayAttrib(m_vao, 2);
glEnableVertexArrayAttrib(m_vao, 3);
glEnableVertexArrayAttrib(m_vao, 4);

glVertexArrayVertexBuffer(m_vao, 0, m_vbo, size_bytes(indices), sizeof(SpriteVertex));
glVertexArrayVertexBuffer(m_vao, 1, m_ivbo, 0, sizeof(SpriteInstanceVertex));

glVertexArrayElementBuffer(m_vao, m_vbo);
glBindVertexArray(m_vao);
```
I find the builder version more succinct and faster to understand. It merges many of the calls you normally do into single function calls without the loss of control. Especially with OpenGL 4.5 where using the old `glVertexAttribPointer` functions is not recommended. Also, by using the builder, if there are any bugs or updates to the API in a hypothetical OpenGL 4.7 I would only have to update the code in the builder to ensure the VAO creation remained correct throughout the project.

The class interface is also well documented to make the adaption of the class more accessible. Every parameter is detailed, and the class it self describes some general information that is good to know. As for the implementation itself, there is relatively little code, and most just delegate work to one or more OpenGL functions. The only thing to highlight is the use of the destructor to clean up an unused handle and returning the builder by reference to self to allow chaining of method calls, although that shows good use of the builder pattern more than anything else. The "good-ness" of this code comes from its use rather than a fancy implementation.

#### Bad Code

When considering bad code from the project, one file comes to mind almost right away, [this one](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/scenes/scenario_scene_lua_bindings.cpp). At a high level, this is a single file that contains the implementation of a single function that is about 5-600 lines long. The function is simply a shell where we bind functions to the Lua API. However, every single function that is bound is a lambda, and there are a *lot* of them in there. A brief comment accompanies most of the lambdas to say what it is for, but it is not the case for all of them.

This function was so bad that it was moved to a separate file to declutter the file it originally lived in. The reason it is so long and unmanageable is that we added new Lua function bindings here as we needed them just to be able to get them working. Since we had little experience integrating Lua upfront, we did not know how to effectively bind functions in a readable and maintainable way. This code is not maintainable since it is such a huge mess of functions, and finding the one you want is quite hard since they are all lambdas. Also, the individual Lua functions lack documentation so we would have to remember every function manually. There is also a lot of boilerplate in there, for example on lines 344-365 and 96-250. Those lines, in particular, do not scale well since every time we add a new component, these functions must manually receive a new `case` statement for that component.

The code is also really hard to debug as breaking the debugger inside a lambda does not always work. After all, the function is called once, binding the lambdas to Lua. But when they are called, it is never from inside this function, and later in the program. Many debuggers struggle to realize that they are executing code inside a lambda from this function.

To improve this file, the individual lambdas could be extracted as functions, but then they would require more parameters, as it currently relies on some member variables from the `ScenarioScene` class. The extracted functions could then be wrapped in function objects or lambdas, to be usable from Lua. This would require a lot more code to set up, but would potentially be better in the future. Another approach could be to have every Lua callable use the Command pattern and have function objects for every API call, keeping them around for as long as Lua could potentially need them. That way they could be constructed with all dependencies, and called with ease. A weakness here would be that all functions would share the same `execute` function signature, but could be resolved by passing parameters as Lua tables, or even a map structure.

#### Refactored Code

The code I want to highlight as a refactor is parts of the sprite renderer. This is the file  [before](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/dec204dfb87bb8dab80e54e301e285790326c799/bachelor/sim/src/gfx/sprite_renderer.cpp) and [after](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/10a0e1bd00c8b4e50256d46ae38e60c88c60c1e1/bachelor/sim/src/gfx/sprite_renderer.cpp). It is not a major refactor, but it took the sprite renderer from a hacky solution that just needed to work and show something on the screen to a somewhat more organized file.

Before, everything needed to initialize the renderer happened in the constructor, and it was hard to see where one initialization started and another ended. The shader, VAO, VBO, and all textures are all created in one huge chunk. Also, the resources are never cleaned up in this version. While the constructor should be responsible for these things, the structure is currently not so good. In the after file, the resources that have a logical connection have `private` helper functions to assist in the initialization. This cleans up the constructor a lot and makes it more readable to see at a glance what resources are initialized, or what the constructor does at a high level. If one is interested in the particulars of a particular resource, one can look at the specific functions instead. In the [latest version](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/gfx/sprite_renderer.cpp) of the file, the initialization pattern has been kept as more resources have been needed. Additionally, a destructor is introduced to clean up any resources when the renderer goes out of scope, which the older versions did not do.

In the end, the file is a lot more readable and understandable for first-time readers, and also more maintainable since the functions have more precise tasks to accomplish. If more initializations are needed, it is only a matter of adding a new function. Currently, the destructor just uses the raw OpenGL API calls since the cleanup is rather simple. In the future, these could become matching `deinit_*` functions to keep the naming and usage consistent, and in case the complexity increases.

#### Personal Reflection
    
Professionalism in programming is very important to consider when starting or entering any project. It can be related to your ethics, a good working environment, efficient workflows, and clear communication with your fellow programmers and the end-users. It can also be something to investigate before taking on a job. For example, would you work with military software that could kill people? Or slightly less intimidating: do you want to work with Scrum or Waterfall. Perhaps you strongly object to participating with any Waterfall related project because your workflow is very flexible by nature, and you want the project you work on to be in tune with that. 

To me, professionalism in programming means that you and your team need to have an aligned goal and vision of what you're creating, and everyone involved should understand the framework in which you're working. In addition, all team members should aim to work in a way that can be "universally" understood by the rest of the team. For example, no one should go off on a wild one-man ride using a different naming convention and commenting style just because it suits them better. In that case, they are not being professional as the rest of the developers on the team might struggle with understanding and reading this person's code. Not because they can't read different styles, but because the expectations they have in the given context are broken, and the amount of brain capacity the have to expend to understand it will needlessly be increased.

If you manage your process efficiently and communicate clearly with your team members, not only with spoken words but also in the way you write your code, a team can be considered professional and efficient. Since code is often read more than it is written, having a consistent documentation style is essential for new team members to jump in and understand something. But relying entirely on comments is not enough, as the code should aim to be understandable even without them. By that, I mean that inline comments that show up in the middle of a chunk of code should try to explain why something is happening rather than what is happening in case users don't see the need for something. The code itself should be enough to show what is happening. Still, it doesn't always have to be that strict if explaining "the what" makes the overall readability better. It is more helpful to new developers to read "the what", but in my opinion, you should find that in the documentation of each function if you are curious. Any comments that document the function or class as a whole should be specific on the what.

For our Bachelor, we have tried to remain professional by having a set coding and commenting style, use a streamlined build system that will work for all members, and have daily Scrum meetings to ensure issues and concerns are taken care of, and also to know what the team members are up to. Notes were taken at all the meetings so we can go back at any time to see who was there and what was said at a high level. We also used pair programming to conduct live code reviews, ensure that everyone is up to date on technologies, and to share knowledge within the group. Also, we had a "team-building" night once per week where we would be playing video games to increase team morale and relieve some of the stress of working closely together over a long time. We also used issues and branching thoroughly, and blocked *all* pushes directly to the master branch. Everything had to go through merge requests to end up in `master`. We were able to follow these procedures well throughout the project, with some slack off now and then. Overall, I think the high focus that was put on keeping each other accountable helped the project come as far as it did without making us mortal enemies at the end.

### Leon

#### Good Code

Finding a single file or singular piece of code which I wrote, that I would say is good code is somewhat hard. I would say that good code is code which completes it's tasks as expected, is understandable, and efficient. In that sense, I believe the best code I have written in this project is the new, refactored decision making systems, the [goal-](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/entity/systems/goal.cpp), [goal oriented behavior](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/entity/systems/gob.cpp), and [goal oriented behavior-based action-systems](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/entity/systems/gobbas.cpp).

Functioning as a trio of steps, controlling the process of making a decision, from choosing of what needs to be done, to how to do it most efficiently, to finally completing the action, the goal-based decision making systems are short, concise, and do exactly what they are supposed to in an easily understandable and, most importantly, easily debuggable way.

Each system is completely modular, relying on each other only in the form of input provided, which can be easily mimicked by other systems. This enables users to create their own systems to take the place of any of the three steps in this process, so long as their input and output match the provided systems, where applicable.

While we are on the topic of user scriptability, all three systems heavily rely on variants, which allow the functions that the systems run to be either Lua implemented or a C++ default implementation. This is true for almost all functions in the data structures that the systems rely on, and allows for great flexibility in the tasks the systems can tackle, as the user can decide how goals are weighted, how much impact the passage of time has on a goal, how actions impact goals, and what an action does when run, among other things. These three systems offer an unrivaled amount of customizability to the user, making them the systems that best fit with our vision for a highly flexible and customizable framework.

The only gripes with the systems I have right now are that the check for whether something is a Lua implemented function or c++ function is somewhat ugly, and that the Goal-Oriented Behaviour system is computationally intensive, for as of yet unknown reasons. My intuition is that the high amount of calls to the member functions of objects inside a vector might be the worst offender here, but we will not know for certain until further benchmarks are made.

#### Bad Code
On the opposite side, we have bad code. Here again there isn't a single file I can immediately point to. Instead I would here like to point towards the uglier cousin of the Goal-based decision-making systems, the utility-based decision-making systems, or simply, the need-based systems. These make up the old decision-making process before we realized they were, in fact, less than ideal, and decided to make a new set of decision-making systems instead. These systems are the [Need-](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/entity/systems/need.cpp), [Mitigation-](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/entity/systems/mitigation.cpp), [action-](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/entity/systems/action.cpp), and [requirement-](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/entity/systems/requirement.cpp)systems, which effectively are a more verbose, less customizable, and more error-prone version of their sister systems in the goal-based systems.

Many of the systems read and write from the same components, which leads to a more complex dataflow. This naturally causes issues when trying to debug the system for errors, as it is not necessarily clear which system last touched the now corrupted data. The reasoning behind certain snippets of code is also less than clear, for example the constant removal of components in the mitigation system.

Additionally, Need-based systems severely struggle in terms of flexibility and customizability. The entire prospect of the requirement-system is the worst offender for this, seeing as we can not support custom requirements due to the way they are implemented in the action system. This means we are limited to a group of generic requirements, which themselves are confusing to work with due to the way they are instantiated in the action system, which needs their variables to be present in the strategy the action system is working on. This again causes bloat in the strategy and leads to further confusion as to where data is coming from, and where the process of creating and verifying this data is failing.

When compared to the very direct linking between the goal systems and the components they work on, the Need system is a horrid mess where connections are unorganized, and frankly often illogical. Their low compliance with the goals set by the project, hard to comprehend code, and their aversion towards being debugged is why I think them to be bad code.

As a closing statement, after all of the above, I have to say it annoys me to no end that out of the two decision making systems, the need-based one is the more performant, by a factor of near 50% on small simulations. The fact that this hulk of junk-code is faster than the sleek, streamlined Goal-based systems is incomprehensible and infuriating to me. I choose to believe that this is due to the goal system having a faulty algorithm somewhere, rather than it being less optimized than the need-based systems.

#### Refactored code
A major chunk of development time was spent going through every single system, refactoring them to be as concise and easily understandable as possible. This was done to streamline the cumbersome debugging process which resulted from many systems having a lot of points of contact with other systems, as well as the difficulty of moving data between systems easily.

A good example of this is the [Memory system](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/entity/systems/memory.cpp), which was [originally](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/f775ee914d9e9a959a3bae4eae5a32d21ad6d329/bachelor/sim/src/entity/systems/memory.cpp) a cumbersome hulk of a system, which struggled with the deletion of entities that used to have memories, the addition of new memories was partially handled outside of the system itself, outsourcing parts of its functionality into the creation of actions, in a completely different file.

This was deemed completely unacceptable, seeing as it was not sustainable when users wanted to create their own actions in Lua. So all code for adding memories in response to completing actions was instead moved to an event that would be sent by another system to let the memory system know a new memory was created. This is also less than ideal, but still a massive improvement to the previous implementation, and it keeps all memory code inside the memory system itself, making debugging easier.

Additionally, the system had a bad case of code repetition between throughout, which was eventually moved into separate functions, to increase readability. Finally, the system was also moved to a multithreaded approach, which significantly improved performance when many agents were trying to access their memories on the same frame.

#### Professionalism in programming

Professional, coming from the Latin word `professus`(meaning to have professed, or vowed) stems from the specialists(or professionals) vow to perform their trade to the highest standard. Thus, to be a professional means to have a reputation to uphold in terms of the quality of your work.

In Programming, as in any field, this means that one has to adhere to the standards set in the environment that one is working in, be that a company, a development team, or any collection of people working on the same project. You can not deviate from the standards set by the team and go off to do your own thing. This includes both documentation, code style, and anything else that has been decided in the standard. It involves both the way you work and what you produce, as following standards for security, work ethics, and similar distinctions which vary from workplace to workplace, is essential to the success of a project.

On the coding side, both the workflow and the end product need to be considered. As a professional programmer, adhering to the work ethic agreed upon by the developers and employers is important, as it aids in building trust in your competency in your field with your peers, and assures them that you will perform your work to an acceptable standard. For example, if it has been agreed upon that developers should spend at least 1/8th of their time writing documentation, it should be expected that everyone is following that agreement. Those that do not follow the standards are subject to distrust from their peers, as their inability to follow the agreed-upon rules casts doubt upon their ability to work alongside their team members.

Likewise, the code produced should follow the guidelines set by both employers and the development team. Agreed upon styles need to be followed, whether they are naming conventions, commenting styles, or even something as small as whitespace conventions. This makes the project look uniform, rather than a cluttered mess, and ensures that everyone's vision of what things should look like, and how well things should be documented, align.

In order to follow agreed-upon standards, the team first needs to create these standards of course. This is likely a multi-tiered process, where the product owners, the company that the developers belong to, and the development team themselves all have ideas, demands and personal preferences as to how things should best be handled. Here it is important to have good communication, to ensure all involved leave with a clear vision of what guidelines they are expected to follow, as well as what a breach of these entails.

Communication in general is an important part of these standards as well, as it helps ensure everyone involved knows where the project stands. This is needed for considerations on which work to prioritize, which programmers will be best suited to which tasks, and what can reasonably be achieved. This also includes the administrative forces; the company involved and the product owner, who will need to inform the team about the requirements of the project, the budget, timeframe and the exact expectations of what the end product should be, as well as how they are developing throughout the project.

Ideally, this should create a situation where every member of the development process, from top to bottom can confidently say they know the current state of the project, and how things are looking going forward. This ensures no-one has to spend additional time trying to figure out how things are going, and doing parts of someone else's work. 

### Tomas

#### Good code
The code I wrote, that I was proudest of is the [Input Handler](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/tree/master/bachelor/sim/src/input). What I believe makes it good, is its design. I was tasked with creating an input handler that could handle a binding to an action that works for a certain context. What this means is best explained as an example. In Video games you might be able to walk forward using the "W" key, however, if you step into a car, pressing "W" will not move your character, rather it will move the car. This means that while the action "Walk Forward" is bound to the "W" key, it means different things depending on what/where you are, which we refer to as a "Context". 

What the input manager does very much assembles this, to give a real scenario in the program, the fact of pressing escape while in the Pause Menu takes you out of the Pause Menu, while clicking "Escape" in the main menu exits the program. Escape does different things depending on the context. A context may potentially hold a lot of bindings, such as "S" for backward, and "A" for left, etc..

Modeling this I created 2 classes a "Context Handler" class, what would be the class we would use, and call its functions from. Moreover, there was also an "Action Handler class" that originally held a mapping from Key to its function, i.e move forward could be that function. However, this was later changed to hold a mapping from Key to an "Action" Enum, and another from "Action" to function, this was to introduced to keep the "functionality" static, and allow for easy switching from Key to Action without affecting its functionality. Meaning if you wanted 2 keys to be bound to the same function, in the old system you would need to write the function twice, the new way allowed to easily bind two keys to one action, and that action is always bound to the same function. 

So the Context handler then holds a mapping of all the context to an Action Handler class, and the Action handler holds that context's Key -> Action -> Function binding. The Context Handler also keeps track of what context you currently are in and the switching between them. 

I believe the way this is modeled is one of the better ways to create a customizable key binding, as the end-user can decide what button does what action in any given context. However there is a downside to this, one is that it can be a bit confusing, and maybe a bit hard to keep track of. It could also be a bit over-engineered, in the sense that many users might not even need this functionality. However, I believed it to be a very good system based on the requirements we made for the Input Handler.

The code itself has good documentation on what the functions do. However, the documentation inside the function is minimal, because it's not too hard to understand what the code does. When we asked for a code review by Simon, he stated that it was not necessary to write a bunch of comments of what each if or else statement did, as it was understood based on the context, and variable names.



#### Bad code

While I worked on other things than the "Input Handler" it also has a part of it that has some bad code. Rather a related section to it, the logic for its [UI](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/blob/master/bachelor/sim/src/scenes/preference_scene.cpp). While I believe the main logic for **handling** the Input is solid, the logic for displaying the UI and transferring new keybindings to the mapping is rather lackluster and confusing. As a sign that it's pretty bad is the fact I do not know where to start. 

Here as well there are not many comments inside the different functions, but compared to the input handler, these functions are larger. Even worse, it used to be one big function, but during a large common refactor of all the files, we split it up a tiny bit. One reason for its size is just how the UI gets called, and that's not too much that we can do about it. However, that is not the worst part about it.

The design philosophy I came up with was to create a copy of the mapping, that the user could change around, and when done with his/her changes I would take the copy of the mappings, and overwrite the old one. I do not think this idea in itself is a horrible one, however, how it turned out was. The fact that we are using unordered maps for the mapping, means that I do not retrieve all the bindings in the same order if I add or remove a binding, the UI will change the order in which it displays the key binding. This can be pretty confusing for the user when suddenly his bindings are displayed to him in a different order then what he created them in. There is of course an easy fix to this, it's just to make it an ordered map. However that would mean changing the Input Handler as well, which was a job I decided I would do if I had spare time, which I knew we would not get.

In the code section, what truly makes it worse, is the edge cases. To add a new key binding, you click the new keybinding button in the context you want it (red arrow created to show the button):
![Keybindings UI](https://i.imgur.com/bqa57Fu.png)
You then get a selection of all possible actions, then you choose an action, after that you get to set a key to that action in that context. It's here the real mess begins. After pressing a key, the code needs to figure out if:
1. The key you pressed is a Mouse press or a Keyboard press (Due to hoe SDL works)
2. If the binding is a new binding or a replacement
3. Then need to know if the new key you pressed already has a binding if it does we need to flip it
    * Only relevant if changing an existing key such as A -> "Does something" and B -> "Does other", if you change B to do "Does something" then A will do "Does other"
5. If we are going to flip it, we need to know if the old key was a Mouse button or a Keyboard key.

All of these steps are nested, meaning the two branches of the first item, has all the other items. I.e. Both the mouse press, keyboard press brach need to know if the binding is new or a replacement. At first glance the fix seems to just create a function for it, to avoid this duplication. However, at closer inspection you will notice that how it is handled is different for a mouse press, and a keypress.

To fix this bad code, I believe a redesign is in order while trying to make the similar branches into functions to avoid copying code will make it look nicer, it will not fix the main issue of it being poorly designed. If I were to do it again, I would spend more time designing something that combined keyboard and mouse presses, so that both the handling of input and creating of new key bindings did not need to brach as often. I would also try to learn a bit more about UI design to make it easier for the user to understand how to change, and add key bindings. To top it all of, maybe even write a wrapper for some of the ImGui (the UI) code to use it without cluttering the code. Something I did for PhysFS, that become known as out file manager. 


#### Refactored Code

While we did a good chunk of refactoring, it was mostly "Write documentation for this", "Fix spelling of variable".. etc, no too much of "re-write this entire chunk", with the exceptions of the early AI need-system that got refactored nearing the end, however that was mostly Leon's work. We asked for a code review by Simon, twice, both times we looked through systems I was the primary designer, and implementer([merge after reviewing the FileSystem](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/merge_requests/21/diffs) and [merge after reviewing the InputHandler](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/merge_requests/57/diffs)). Most of the changes were also minimal renames of variables, and fixing some debug messages to be clearer. Except for a part in the file system where Simon caught a false positive exit, that I then later fixed.

I never personally did any _major_ refactors, it was not because our code was soo good, rather, once a part of the program was working, we moved on to the next system that needed to be implemented. Due to the large number of features that needed to be implemented, we rarely had time to look back on old code and used the merge request for each feature to review each other's code. We did do a [large refactor](https://git.gvk.idi.ntnu.no/bachelor/2020/group-107/bachelor/-/merge_requests/105) at the end, going through every single file fixing minor and major issues.

Refactoring code was an important part of our development process, and while we did not have time to always re-write certain systems, refactoring helped us, and our code look more professional and error-free.



#### Professionalism in programming

Soon after the Bachelor is done, a lot of programmers are going to start working professionally, myself included. There it will be important to follow company coding standards, and their best practices. Companies normally set these standards so that their code is as consistent, and maintainable as possible, as programmer comes and leaves, but the company still stands. However, there is still a lot of ways you can write bad code, even following typical company standards. So it is important that you as well have a standard where the company does not. Typically this has a lot to do with how you structure your code, and how well you comment it. The company might have a preferred comment style, but only you decided how to phrase your comment, and explain it. For many newcomers explaining their code is hard and confusing, typically since it common for them to not need to comment their code, as they have not worked on a large project that spans several months or sometimes years. It is why it is important to write comments, and code that you can understand when reading it again some month or maybe a year later.

Professional programming to me is not only about writing good and readable code. It is also the manner in which I conduct myself when discussing and representing the programming profession. In this age of artificial intelligence, there is a lot of ethical and moral question that keeps coming up. Such as, what should self-driving cars do when they are about to hit an old lady crossing the crosswalk, and the car cannot stop in time. Should it drive over the old lady, as she has lived out most of her life, and save the younger driver. Or save the old lady by ramming the car into the side, as according to most laws it is the driver's fault when hitting a pedestrian crossing. Even worse when you create something that other people can misuse, such as an algorithm with several weights, to figure out who in the E.R. gets treatment first. And those who get the algorithm misuses the weighting system by allowing people with red hair come before, those with blond. Such questions are not something programmers _should_ be responsible for, but since it is their creations, they are. Knowing that you are responsible for your code, and how it is being used, to me, is a very important part of being a professional programmer, to even go as far as to scratch your work, and risk your livelihood, if you believe what you are creating and be misused.




