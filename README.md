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

    for( auto _ : Benchmark( "   With Reserve" ) )
    {
        auto v = std::vector<BigS>{};
        v.reserve( 20 );
        for( int i{ 0 }; i < 20; ++i ) v.push_back( BigS{} );
    }

    for( auto _ : Benchmark( "Random Stuff" ) ) int a = 0;
    for( auto _ : Benchmark( "What if I have an extremely long title ?" ) ) {}
}
````

Possible Output
````
Benchmarking... Without Reserve
Benchmarking... With    Reserve
Benchmarking... Random Stuff
Benchmarking... What if I have an extremely long title ?

    ______________________
   /                     /
  /  Benchmark Summary  /
 /_____________________/                             Latency          Throughput
________________________________________________________________________________

Without Reserve                                        33008               30295
With    Reserve                                         9983              100161
Random Stuff                                             149             6684999
What if I have an extremely long title ?                 149             6685514
________________________________________________________________________________


[ End of Program ]
````
