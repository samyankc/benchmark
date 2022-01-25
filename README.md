# benchmark

Usage Example
````C++
#include <benchmark.h>
#include <iostream>
#include <vector>

struct BigS
{
int data[999];
};

int main()
{
    using std::vector;

    for( auto _ : Benchmark("W/O Reserve") )
    {
        auto v = vector<BigS>{};
        for( int i{ 0 }; i < 200; ++i ) v.push_back( BigS{} );
    }
    
    for( auto _ : Benchmark("W/  Reserve") )
    {
        auto v = vector<BigS>{};
        v.reserve(200);
        for( int i{ 0 }; i < 200; ++i ) v.push_back( BigS{} );
    }

}
````

Possible Output
````
                Latency         Throughput
W/O Reserve     327777          3050
W/  Reserve     93533           10691

[ End of Program ]
````
