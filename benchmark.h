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
#include <string_view>
#include <vector>

namespace {

struct BenchmarkResult
{
    std::string Title;
    std::size_t TotalCycle;
    std::size_t TotalIteration;
    auto Latency() const { return TotalCycle / TotalIteration; }
    auto Throughput() const { return 1000000000 * TotalIteration / TotalCycle; }
    auto operator<( const auto& RHS ) const { return Title.length() < RHS.Title.length(); }
};

struct BenchmarkAnalyzer : std::vector<BenchmarkResult>
{
    std::size_t BaselinePos;
    BenchmarkAnalyzer() : std::vector<BenchmarkResult>{}, BaselinePos{ 0 } { reserve( 10 ); }

    ~BenchmarkAnalyzer()
    {
        const auto DigitWidth = 20;
        const auto TitleWidth = std::max( std::max_element( begin(), end() )->Title.length(),  //
                                          24ull );
        const auto ThroughputBaseline =
        static_cast<double>( ( *this )[ BaselinePos ].Throughput() );

        auto cout_row = [ TW = std::setw( TitleWidth ), DW = std::setw( DigitWidth ) ](  //
                        std::string_view Title,                                          //
                        const auto Latency,                                              //
                        const auto Throughput,                                           //
                        const auto Relative,                                             //
                        const char fill = ' ' )                                          //
        {
            std::cout << std::setfill( fill ) << std::left                       //
                      << TW << Title << std::right                               //
                      << DW << Latency                                           //
                      << DW << Throughput                                        //
                      << DW << std::setprecision( 2 ) << std::fixed << Relative  //
                      << std::setfill( ' ' ) << '\n';
        };

        auto cout_line = [ cout_row ] {
            cout_row( "", "", "", "", '_' );
            std::cout << '\n';
        };

        std::cout << "\n    ______________________"
                     "\n   /                     /"
                     "\n  /  Benchmark Summary  /\n";
        /**/ cout_row( " /_____________________/", "Latency", "Throughput", "Relative" );
        cout_line();
        for( auto&& Result : *this )
            cout_row( Result.Title,         //
                      Result.Latency(),     //
                      Result.Throughput(),  //
                      Result.Throughput() / ThroughputBaseline );
        cout_line();
    }
};

static auto BenchmarkResults = BenchmarkAnalyzer{};

struct BenchmarkContainer
{
    using clock      = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;

    constexpr static auto MaxDuration  = std::chrono::milliseconds{ 2000 };
    constexpr static auto MaxIteration = std::size_t{ 9876 };

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
    BenchmarkResults.push_back( { " " + BenchmarkTitle, 0, 0 } );
    return BenchmarkContainer{ BenchmarkResults.back() };
}

};  // namespace

#endif /* BENCHMARK_H */

#define RUN_TEST
#ifdef RUN_TEST

int main()
{
    std::ios::sync_with_stdio( false );
    for( auto _ : Benchmark( "puts" ) )
    {  //
        puts( "This is a test string." );
    }

    for( auto _ : Benchmark( "std::cout" ) )
    {  //
        std::cout << "This is a test string." << std::endl;
    }

    for( auto _ : Benchmark( "no-op" ) )
    {
        for( volatile int i = 0; i < 2000; ++i ) { ++i; }
    }
}

#endif