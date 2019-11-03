#pragma once

#define ULFEYE_NO_COPY(classname)         \
    classname(const classname&) = delete; \
    classname& operator=(const classname&) = delete

#define ULFEYE_NO_MOVE(classname)    \
    classname(classname&&) = delete; \
    classname& operator=(classname&&) = delete

namespace ulf
{
}
