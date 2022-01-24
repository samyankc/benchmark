# benchmark

Usage Example

#include <benchmark.h>

#include <iostream>
#include <vector>

struct BigS
{
int data[999];
};

int main()
{
    using std::cout, std::vector;
    BenchmarkResult Result1, Result2;

    for( auto _ : Benchmark >> Result1 )
    {
        auto v = vector<BigS>{};
        for( int i{ 0 }; i < 2000; ++i ) v.push_back( BigS{} );
    }
    
    for( auto _ : Benchmark >> Result2 )
    {
        auto v = vector<BigS>{};
        v.reserve(2000);
        for( int i{ 0 }; i < 2000; ++i ) v.push_back( BigS{} );
    }
    

    cout << "\n\nResult 1" << Result1  //
         << "\n\nResult 2" << Result2;
}
