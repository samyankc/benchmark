# benchmark

Usage Example
````C++
#include <benchmark.h>
#include <iostream>
#include <vector>

struct BigS
{
    int data[ 999 ];
};

int main()
{
    for( auto _ : Benchmark( "Without Reserve" ) )
    {
        auto v = std::vector<BigS>{};
        for( int i{ 0 }; i < 20; ++i ) v.push_back( BigS{} );
    }

    for( auto _ : Benchmark( "With Reserve" ) )
    {
        auto v = std::vector<BigS>{};
        v.reserve( 20 );
        for( int i{ 0 }; i < 20; ++i ) v.push_back( BigS{} );
    }
    for( auto _ : Benchmark( "volatile increment" ) )
    {
        for( volatile int i = 0; i < 100; ++i ) { ++i; }
    }
    for( auto _ : Benchmark( "Random Stuff" ) ) int a = 0;
    for( auto _ : Benchmark( "What if I have an extremely long title ?" ) ) {}
}
````

Possible Output
````
Benchmarking... Without Reserve
Benchmarking... With Reserve
Benchmarking... volatile increment
Benchmarking... Random Stuff
Benchmarking... What if I have an extremely long title ?

    ______________________
   /                     /
  /  Benchmark Summary  /
 /_____________________/                            Latency        Throughput          Relative
_______________________________________________________________________________________________

 Without Reserve                                      62726             15942              1.00
 With Reserve                                         10035             99647              6.25
 volatile increment                                     801           1247722             78.27
 Random Stuff                                           145           6892381            432.34
 What if I have an extremely long title ?               145           6862176            430.45
_______________________________________________________________________________________________

[ End of Program ]
````
