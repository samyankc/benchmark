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
}
````

Possible Output
````
Benchmark Summary
                        Latency          Throughput
---------------------------------------------------
W/O Reserve              317235                3152
W/  Reserve               91795               10893
---------------------------------------------------

[ End of Program ]
````
