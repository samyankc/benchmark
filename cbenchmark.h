#ifndef BENCHMARK_H
#define BENCHMARK_H
#define CBENCHMARK_H

//#include <immintrin.h>  //__rdtsc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#else
#ifndef bool
#define bool unsigned char
#endif
#endif

#define MaxIteration 5678
#define MaxCycle     ( CLOCKS_PER_SEC * 3 )

size_t digit_width( long long n )
{
    if( n < 0 ) return 1 + digit_width( -n );
    for( size_t i = 1; i < 22; ++i )
        if( ( n /= 10 ) == 0 ) return i;
    return 0;
}

////////////////////////////// BenchmarkResultNode [Class] //////////////////////////////
////////////////////////////// BenchmarkResultNode [Fields] /////////////////////////////////
typedef struct BenchmarkResultNodeTag
{
    const char* Title;
    clock_t TotalCycle;
    size_t TotalIteration;
    struct BenchmarkResultNodeTag* Next;
} BenchmarkResultNode, *LP_BRN;

////////////////////////////// BenchmarkResultNode [Methods] /////////////////////////////////
size_t BRN_Latency( LP_BRN self ) { return self->TotalCycle / self->TotalIteration; }

size_t BRN_Throughput( LP_BRN self )
{
    return self->TotalCycle ? CLOCKS_PER_SEC * self->TotalIteration / self->TotalCycle : 0;
}

LP_BRN BRN_New( const char* Title, clock_t TotalCycle, size_t TotalIteration )
{
    LP_BRN self = (LP_BRN)malloc( sizeof( BenchmarkResultNode ) );
    if( self == NULL )
    {
        puts( "BenchmarkResultNode Construction Failed" );
        getchar();
        exit( EXIT_FAILURE );
    }
    self->Title          = Title;
    self->TotalCycle     = TotalCycle;
    self->TotalIteration = TotalIteration;
    self->Next           = NULL;
    return self;
}

////////////////////////////// BenchmarkResultNode [End] //////////////////////////////

////////////////////////////// BenchmarkAnalyser [Class] //////////////////////////////
////////////////////////////// BenchmarkAnalyser [Fields] //////////////////////////////
typedef struct BenchmarkAnalyserTag
{
    LP_BRN ListHead, ListTail;
    size_t TitleWidth, LatencyWidth, ThroughputWidth;
} BenchmarkAnalyser, *LP_BA;

////////////////////////////// BenchmarkAnalyser [static] ////////////////////////////////////
static BenchmarkAnalyser BenchmarkResults = { NULL, NULL,
                                              strlen( "/_____________________/" ) + 6,
                                              strlen( "Latency" ), strlen( "Throughput" ) + 5 };

////////////////////////////// BenchmarkAnalyser [Methods] //////////////////////////////
int BA_PushBack( LP_BA self, LP_BRN NewNode )
{
    if( self->ListHead == NULL )  // new container
        self->ListTail = self->ListHead = NewNode;
    else
        self->ListTail = self->ListTail->Next = NewNode;

#define UpdateMaxValue( A, B ) \
    do {                       \
        size_t B_ = ( B );     \
        if( A < B_ ) A = B_;   \
    } while( 0 )

    UpdateMaxValue( self->TitleWidth, strlen( NewNode->Title ) );
    UpdateMaxValue( self->LatencyWidth, digit_width( BRN_Latency( NewNode ) ) );
    UpdateMaxValue( self->ThroughputWidth, digit_width( BRN_Throughput( NewNode ) ) );

    return 0;
}

void BA_PrintHorizontalLine( LP_BA self )
{
    putchar( '\n' );
    size_t LineWidth = self->TitleWidth + self->LatencyWidth + self->ThroughputWidth;
    for( size_t i = 0; i <= LineWidth; ++i ) putchar( '_' );
    putchar( '\n' );
}

void BA_PrintSummaryHeader( LP_BA self )
{
    printf( "\n    ______________________"
            "\n   /                     /"
            "\n  /  Benchmark Summary  /" );
    //      "\n /_____________________/"
    printf( "\n %-*s", (int)self->TitleWidth, "/_____________________/" );
    printf( "%*s", (int)self->LatencyWidth, "Latency" );
    printf( "%*s", (int)self->ThroughputWidth, "Throughput" );
    BA_PrintHorizontalLine( self );
}

void BA_PrintSummaryLine( LP_BA self, LP_BRN BRNode )
{
    printf( "\n %-*s", (int)self->TitleWidth, BRNode->Title );
    printf( "%*lu", (int)self->LatencyWidth, BRN_Latency( BRNode ) );
    printf( "%*lu", (int)self->ThroughputWidth, BRN_Throughput( BRNode ) );
}

void BA_Release( LP_BA self )
{
    if( ! self ) return;
    BA_PrintSummaryHeader( self );
    self->ListTail = NULL;
    while( self->ListHead )
    {
        LP_BRN OldListHead = self->ListHead;
        self->ListHead     = self->ListHead->Next;
        BA_PrintSummaryLine( self, OldListHead );
        free( OldListHead );
    }
    BA_PrintHorizontalLine( self );
}

void BenchmarkAnalyser_IndirectRelease( LP_BA* IndirectPtr ) { BA_Release( *IndirectPtr ); }
////////////////////////////// BenchmarkAnalyser [End] //////////////////////////////

////////////////////////////// BenchmarkModulator [Class] //////////////////////////////
////////////////////////////// BenchmarkModulator [Fields] //////////////////////////////
typedef struct BenchmarkModulatorTag
{
    clock_t StartTime;
    clock_t TotalCycle;
    size_t TotalIteration;
} BenchmarkModulator;

////////////////////////////// BenchmarkModulator [End] //////////////////////////////

#define AutoRelease( T ) __attribute__( ( cleanup( T##_IndirectRelease ) ) ) T

#define UniqueVarID_( Name, ID ) Name##_##ID
#define UniqueVarID( Name, ID )  UniqueVarID_( Name, ID )
#define UniqueName               UniqueVarID( _0_, __LINE__ )

#define Benchmark( Title )                                                    \
    ; /*single line control block skip*/                                      \
    printf( "\nBenchmarking... %s\n", Title );                                \
    AutoRelease( BenchmarkAnalyser )* UniqueName =                            \
    BenchmarkResults.ListHead ? NULL : &BenchmarkResults;                     \
    for( BenchmarkModulator Mod = { clock(), 0, 0 };                          \
         Mod.TotalCycle < MaxCycle && Mod.TotalIteration < MaxIteration ||    \
         BA_PushBack( &BenchmarkResults,                                      \
                      BRN_New( Title, Mod.TotalCycle, Mod.TotalIteration ) ); \
         Mod.TotalCycle = clock() - Mod.StartTime, ++Mod.TotalIteration )

#ifdef __cplusplus
}
#endif

#endif /* BENCHMARK_H */

#define DONT_RUN_TEST_
#ifndef DONT_RUN_TEST

void task( int k )
{
    printf( "Performing task." );
    for( int i = 0, interval = 10; i * interval < k; ++i, Sleep( interval ) ) putchar( '.' );
    printf( "Complete\n" );
}

int main()
{
    Benchmark( "task(50)" ) { task( 50 ); }
    Benchmark( "task(100)" ) { task( 100 ); }
    Benchmark( "task(200)" ) { task( 200 ); }

    Benchmark( "volatile increment" )
    {
        for( volatile int i = 0; i < 100; ++i ) {}
    }
}

#endif