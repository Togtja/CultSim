#include "functions.h"

#include <cmath>
#include <vector>
#include <memory>

#include <benchmark/benchmark.h>
#include <entt/meta/factory.hpp>

/** Arbitrary system with some pointers to give it a size */
class ISystem
{
protected:
    float* a = nullptr;
    float* b = nullptr;
    float* c = nullptr;

public:
    virtual void update(float dt) = 0;
};

class ActionSystem : public ISystem
{
public:
    void update(float dt) override;
};

void ActionSystem::update(float dt)
{
    /** Do arbitrary math */
    float x = 10 / 10.f;
    for (int i = 0; i < 100'000; ++i)
    {
        x += std::sqrt(static_cast<float>(i)) * dt;
        if (a == nullptr)
        {
            b = &x;
        }
    }

    float y = 5.f / x;
    b       = nullptr;
    benchmark::DoNotOptimize(y);
}

class ActionNonVirtalSystem
{
private:
    float* a = nullptr;
    float* b = nullptr;
    float* c = nullptr;

public:
    void update(float dt)
    {
        /** Do arbitrary math */
        float x = 10 / 10.f;
        for (int i = 0; i < 100'000; ++i)
        {
            x += std::sqrt(static_cast<float>(i)) * dt;
            if (a == nullptr)
            {
                b = &x;
            }
        }

        float y = 5.f / x;
        b       = nullptr;
        benchmark::DoNotOptimize(y);
    }
};

static void bm_virtual_update(benchmark::State& state)
{
    /** Fill vector with N systems */
    std::vector<std::unique_ptr<ISystem>> vec{};
    for (int i = 0; i < 6; ++i)
    {
        vec.emplace_back(new ActionSystem());
    }

    /** Measure this */
    for (auto _ : state)
    {
        for (auto&& system : vec)
        {
            system->update(1.f / 60.f);
        }
    }
}

static void bm_meta_update(benchmark::State& state)
{
    /** Reflect type */
    entt::meta<ActionNonVirtalSystem>()
        .type(entt::hashed_string("ActionSystem"))
        .ctor<>()
        .func<&ActionNonVirtalSystem::update>(entt::hashed_string("update"));

    /** Fill vector with N systems */
    std::vector<entt::meta_any> vec{};
    for (int i = 0; i < state.range(0); ++i)
    {
        auto type = entt::resolve(entt::hashed_string("ActionSystem"));
        vec.push_back(type.construct());
    }

    /** Measure this */
    for (auto _ : state)
    {
        for (auto&& system : vec)
        {
            system.type().func(entt::hashed_string("update")).invoke(system, 1.f / 60.f);
        }
    }
}

static void bm_0_arg_func(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = func_0_vars();
        benchmark::DoNotOptimize(result);
    }
}

static void bm_1_arg_func(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = func_1_vars(state.range(0));
        benchmark::DoNotOptimize(result);
    }
}

static void bm_2_arg_func(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto result = func_2_vars(state.range(0), state.range(1));
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(bm_0_arg_func);
BENCHMARK(bm_1_arg_func)->Ranges({{-8, 33}});
BENCHMARK(bm_2_arg_func)->Ranges({{-8, 33}});
BENCHMARK_MAIN();
