// usage:
// for( auto _ : Benchmark ) ...
// for( auto _ : Benchmark >> External_Result_Holder ) ...

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <immintrin.h>  //__rdtsc

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct BenchmarkResult
{
    std::string Title;
    std::size_t TotalCycle;
    std::size_t TotalIteration;

    friend auto& operator<<( auto& out, const BenchmarkResult& Result )
    {
        auto Latency    = Result.TotalCycle / Result.TotalIteration;
        auto Throughput = 1000000000 * Result.TotalIteration / Result.TotalCycle;
        return out << "\n    Latency : " << Latency << " Cycles"            //
                   << "\n Throughput : " << Throughput << " / Giga Cycles"  //
                   << std::endl;
    }
};

static auto BenchmarkResults = std::vector<BenchmarkResult>{};

struct BenchmarkAnalyzerType
{
    BenchmarkAnalyzer() { BenchmarkResults.reserve( 10 ); }
    ~BenchmarkAnalyzerType()
    {
        std::cout << "\t\tLatency\t\tThroughput\n";
        for( auto&& Result : BenchmarkResults )
        {
            auto Latency    = Result.TotalCycle / Result.TotalIteration;
            auto Throughput = 1000000000 * Result.TotalIteration / Result.TotalCycle;
            std::cout << Result.Title << "\t"  //
                      << Latency << "\t\t"     //
                      << Throughput << "\n";
        }
    }
};

static auto BenchmarkAnalyzer = BenchmarkAnalyzerType{};

struct BenchmarkContainer
{
    using clock      = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;

    constexpr static auto MaxDuration  = std::chrono::milliseconds{ 2000 };
    constexpr static auto MaxIteration = std::size_t{ 50000 };

    BenchmarkResult& Result;

    struct Sentinel
    {};

    template <typename BaseRange>
    struct Iterator
    {
        BaseRange& Base;
        time_point EndTime;
        std::size_t RemainIteration;
        std::size_t StartCycle;

        auto operator*() { return 0; }
        auto operator++() { --RemainIteration; }
        auto operator!=( Sentinel ) { return RemainIteration > 0 && clock::now() < EndTime; }

        Iterator( BaseRange& Base )
            : Base{ Base },                                      //
              EndTime{ clock::now() + BaseRange::MaxDuration },  //
              RemainIteration{ BaseRange::MaxIteration },        //
              StartCycle{ __rdtsc() }
        {}

        ~Iterator()
        {
            Base.Result.TotalCycle     = __rdtsc() - StartCycle;
            Base.Result.TotalIteration = BaseRange::MaxIteration - RemainIteration;
        }
    };

    auto begin() { return Iterator{ *this }; }
    auto end() { return Sentinel{}; }

    auto operator>>( BenchmarkResult& RedirectedResult )
    {
        return std::decay_t<decltype( *this )>{ RedirectedResult };
    }
};

auto Benchmark( std::string&& BenchmarkTitle )
{
    BenchmarkResults.emplace_back( std::move( BenchmarkTitle ), 0, 0 );
    return BenchmarkContainer{ BenchmarkResults.back() };
}

};  // namespace

#endif /* BENCHMARK_H */