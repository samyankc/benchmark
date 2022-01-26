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
    for( auto _ : Benchmark( "W/O Reserve" ) )
    {
        auto v = std::vector<BigS>{};
        for( int i{ 0 }; i < 200; ++i ) v.push_back( BigS{} );
    }

    for( auto _ : Benchmark( "W/  Reserve" ) )
    {
        auto v = std::vector<BigS>{};
        v.reserve( 200 );
        for( int i{ 0 }; i < 200; ++i ) v.push_back( BigS{} );
    }

    for( auto _ : Benchmark( "Random Stuff" ) ) int a = 0;
    for( auto _ : Benchmark( "Some Other Random Stuff" ) ) int k = 0;
    for( auto _ : Benchmark( "What if I have an extremely long title ?" ) ) {}
    for( auto _ : Benchmark( "Will the extremely long title mess up display formatting ?" ) ) {}
}
````

Possible Output
````
Benchmark Summary
                                                                       Latency          Throughput
--------------------------------------------------------------------------------------------------
W/O Reserve                                                             319187                3132
W/  Reserve                                                              93876               10652
Random Stuff                                                               146             6804643
Some Other Random Stuff                                                    146             6817368
What if I have an extremely long title ?                                   144             6897678
Will the extremely long title mess up display formatting ?                 144             6912302
--------------------------------------------------------------------------------------------------

[ End of Program ]
````
