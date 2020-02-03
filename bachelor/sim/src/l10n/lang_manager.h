#include "filesystem.h"

#include <string>
#include <unordered_map>

#include <spdlog/spdlog.h>

namespace cs
{
namespace lang
{
class LangManager
{
private:
    std::unordered_map<std::string, std::string> langs;

public:
    LangManager(/* args */);
    std::string available_lang()
    {
        return fs::read_file("l10n/en.lua");
    }
    ~LangManager();
};
} // namespace lang
} // namespace cs
