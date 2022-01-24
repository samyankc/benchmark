// usage:
// for( auto _ : Benchmark ) ...
// for( auto _ : Benchmark >> External_Result_Holder ) ...

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <immintrin.h>  //__rdtsc

#include <chrono>
#include <iostream>

namespace {

struct BenchmarkResult
{
    std::size_t TotalCycle;
    std::size_t Rep;

    friend auto& operator<<( auto& out, const BenchmarkResult& Result )
    {
        auto Latency    = Result.TotalCycle / Result.Rep;
        auto Throughput = 1000000000 * Result.Rep / Result.TotalCycle;
        return out << "\n    Latency : " << Latency << " Cycles"            //
                   << "\n Throughput : " << Throughput << " / Giga Cycles"  //
                   << std::endl;
    }
};

struct BenchmarkContainer
{
    using clock         = std::chrono::steady_clock;
    using time_point    = std::chrono::time_point<clock>;
    using time_interval = std::chrono::milliseconds;

    constexpr static time_interval MaxDuration{ 2000 };
    constexpr static std::size_t MaxRep{ 50000 };

    inline static BenchmarkResult SharedResult{};

    BenchmarkResult& Result{ SharedResult };

    struct Sentinel
    {};

    template <typename BaseRange>
    struct Iterator
    {
        BaseRange& Base;
        time_point EndTime;
        std::size_t RemainRep;
        std::size_t StartCycle;

        auto operator*() { return 0; }
        auto operator++() { --RemainRep; }
        auto operator!=( Sentinel ) { return RemainRep > 0 && clock::now() < EndTime; }

        Iterator( BaseRange& Base )
            : Base{ Base },                                      //
              EndTime{ clock::now() + BaseRange::MaxDuration },  //
              RemainRep{ BaseRange::MaxRep },                    //
              StartCycle{ __rdtsc() }
        {}

        ~Iterator()
        {
            Base.Result.TotalCycle = __rdtsc() - StartCycle;
            Base.Result.Rep        = BaseRange::MaxRep - RemainRep;
            if( &Base.Result == &BaseRange::SharedResult ) std::cout << Base.Result;
        }
    };

    auto begin() { return Iterator{ *this }; }
    auto end() { return Sentinel{}; }

    auto operator>>( BenchmarkResult& RedirectedResult )
    {
        return std::decay_t<decltype( *this )>{ RedirectedResult };
    }
};

};  // namespace

static auto Benchmark = BenchmarkContainer{};

#endif /* BENCHMARK_H */