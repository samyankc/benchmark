// usage:
// for( auto _ : Benchmark("Title") ) ...

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <immintrin.h>  //__rdtsc

#include <chrono>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct BenchmarkResult
{
    std::string Title;
    std::size_t TotalCycle;
    std::size_t TotalIteration;
};

struct BenchmarkAnalyzer : std::vector<BenchmarkResult>
{
    BenchmarkAnalyzer() : std::vector<BenchmarkResult>{} { reserve( 10 ); }

    ~BenchmarkAnalyzer()
    {
        auto DigitWidth = 20;
        auto TitleWidth = std::max_element( begin(), end(),
                                            []( auto& lhs, auto& rhs ) {
                                                return lhs.Title.length() < rhs.Title.length();
                                            } )
                          ->Title.length();
        using std::cout, std::setw, std::setfill;

        cout << "\n\nBenchmark Summary\n"  //
             << setw( TitleWidth + DigitWidth ) << "Latency" << setw( DigitWidth ) << "Throughput"
             << '\n';
        cout << setw( TitleWidth + DigitWidth * 2 + 1 ) << setfill( '-' ) << '\n' << setfill( ' ' );
        for( auto&& Result : *this )
        {
            auto Latency    = Result.TotalCycle / Result.TotalIteration;
            auto Throughput = 1000000000 * Result.TotalIteration / Result.TotalCycle;
            cout << std::left << setw( TitleWidth ) << Result.Title  //
                 << std::right << setw( DigitWidth ) << Latency << setw( DigitWidth ) << Throughput
                 << '\n';
        }
        cout << setw( TitleWidth + DigitWidth * 2 + 1 ) << setfill( '-' ) << '\n' << setfill( ' ' );
    }
};

static auto BenchmarkResults = BenchmarkAnalyzer{};

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
};

auto Benchmark( std::string&& BenchmarkTitle )
{
    BenchmarkResults.emplace_back( std::move( BenchmarkTitle ), 0, 0 );
    return BenchmarkContainer{ BenchmarkResults.back() };
}

};  // namespace

#endif /* BENCHMARK_H */