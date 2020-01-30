
namespace cs
{
float get_current_time()
{
    auto chrono_now = std::chrono::steady_clock::now();

    chrono_now = std::chrono::time_point_cast<std::chrono::seconds>(chrono_now);

    auto epoch = chrono_now.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);

    float duration = value.count();

    return duration;
}
} // namespace cs
