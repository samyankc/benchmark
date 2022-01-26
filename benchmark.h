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
    auto operator<( const auto& RHS ) const { return Title.length() < RHS.Title.length(); }
};

struct BenchmarkAnalyzer : std::vector<BenchmarkResult>
{
    BenchmarkAnalyzer() : std::vector<BenchmarkResult>{} { reserve( 10 ); }

    ~BenchmarkAnalyzer()
    {
        auto DigitWidth = 20;
        auto TitleWidth = std::max_element( begin(), end() )->Title.length();

        auto cout_row = [ = ]( const auto& Title, const auto Latency, const auto Throughput,
                               const char fill = ' ' ) {
            std::cout << std::setfill( fill ) << std::left               //
                      << std::setw( TitleWidth ) << Title << std::right  //
                      << std::setw( DigitWidth ) << Latency              //
                      << std::setw( DigitWidth ) << Throughput           //
                      << std::setfill( ' ' ) << '\n';
        };

        std::cout << "\n\nBenchmark Summary\n";  //
        cout_row( "", "Latency", "Throughput" );
        cout_row( "", "", "", '-' );
        for( auto&& Result : *this )
            cout_row( Result.Title,                                           // Title
                      Result.TotalCycle / Result.TotalIteration,              // Latency
                      1000000000 * Result.TotalIteration / Result.TotalCycle  // Throughput
            );
        cout_row( "", "", "", '-' );
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
    std::cout << "Benchmarking... " << BenchmarkTitle << "\n";
    BenchmarkResults.emplace_back( std::move( BenchmarkTitle ), 0, 0 );
    return BenchmarkContainer{ BenchmarkResults.back() };
}

};  // namespace

#endif /* BENCHMARK_H */