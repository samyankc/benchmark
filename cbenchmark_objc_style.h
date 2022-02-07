#ifndef BENCHMARK_H
#define BENCHMARK_H
#define CBENCHMARK_H

//#include <immintrin.h>  //__rdtsc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#ifdef __cplusplus
extern "C" {
#else
#ifndef bool
#define bool unsigned char
#endif
#endif

#define MaxIteration 5678
#define MaxCycle     ( CLOCKS_PER_SEC * 1 / 2 )

#define Finally( fn )    __attribute__( ( cleanup( fn ) ) )
#define AutoRelease( T ) Finally( T##_IndirectRelease ) T

void FatalError( const char* Message )
{
    puts( "" );
    puts( "###########################" );
    puts( "####### Fatal Error #######" );
    puts( "###########################" );
    puts( "" );
    puts( Message );
    getchar();
    exit( EXIT_FAILURE );
}

size_t Min( const size_t LHS, const size_t RHS ) { return ( LHS < RHS ) ? LHS : RHS; }
size_t Max( const size_t LHS, const size_t RHS ) { return ( LHS > RHS ) ? LHS : RHS; }

void print( const char* str )
{
    for( ; *str; ++str ) putchar( *str );
}

const char* SetWidth( size_t MinWidth, const char* str )
{
    size_t PaddingWidth = MinWidth - Min( MinWidth, strlen( str ) );
    for( size_t i = 0; i < PaddingWidth; ++i ) putchar( ' ' );
    return str;
}

const char* AlignLeft( size_t MinWidth, const char* str )
{
    size_t PaddingWidth = MinWidth - Min( MinWidth, strlen( str ) );
    putchar( ' ' );
    print( str );
    for( size_t i = 1; i < PaddingWidth; ++i ) putchar( ' ' );
    return "";
}

void print_num( long long num, int MinWidth ) { printf( "%*llu", MinWidth, num ); }

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
size_t BRN_Latency( LP_BRN self )
{
    return self->TotalCycle / self->TotalIteration;
}
size_t BRN_Throughput( LP_BRN self )
{
    return self->TotalCycle ? CLOCKS_PER_SEC * self->TotalIteration / self->TotalCycle : 0;
}

LP_BRN BRN_Alloc() { return (LP_BRN)malloc( sizeof( BenchmarkResultNode ) ); }

LP_BRN BRN_Init( LP_BRN self, const char* Title, clock_t TotalCycle, size_t TotalIteration,
                 LP_BRN Next )
{
    if( self == NULL ) FatalError( "BenchmarkResultNode Construction Failed" );
    self->Title          = Title;
    self->TotalCycle     = TotalCycle;
    self->TotalIteration = TotalIteration;
    self->Next           = Next;
    return self;
}

LP_BRN BRN_New( const char* Title, clock_t TotalCycle, size_t TotalIteration )
{
    return BRN_Init( BRN_Alloc(), Title, TotalCycle, TotalIteration, NULL );
}

void BRN_Release( LP_BRN self ) { free( self ); }

////////////////////////////// BenchmarkResultNode [End] //////////////////////////////

////////////////////////////// BenchmarkAnalyser [Class] //////////////////////////////
////////////////////////////// BenchmarkAnalyser [Fields] //////////////////////////////
typedef struct BenchmarkAnalyserTag
{
    LP_BRN ListHead, ListTail;
    size_t ListSize;
    size_t TitleWidth, LatencyWidth, ThroughputWidth;
} BenchmarkAnalyser, *LP_BA;

////////////////////////////// BenchmarkAnalyser [static] ////////////////////////////////////
static BenchmarkAnalyser* BenchmarkResults = NULL;

////////////////////////////// BenchmarkAnalyser [Methods] //////////////////////////////
LP_BA BA_Init( LP_BA self )
{
    if( self == NULL ) FatalError( "BenchmarkAnalyser Construction Failed" );
    self->ListHead        = NULL;
    self->ListTail        = NULL;
    self->ListSize        = 0;
    self->TitleWidth      = strlen( "/_____________________/" ) + 6;
    self->LatencyWidth    = strlen( "Latency" );
    self->ThroughputWidth = strlen( "Throughput" ) + 5;
    return self;
}

LP_BA BA_Alloc() { return (LP_BA)malloc( sizeof( BenchmarkAnalyser ) ); }

LP_BA BA_New() { return BA_Init( BA_Alloc() ); }

void BA_PushBack( LP_BA self, LP_BRN NewNode )
{
    if( self->ListHead == NULL )  // new container
        self->ListTail = self->ListHead = NewNode;
    else
        self->ListTail = self->ListTail->Next = NewNode;
    ++self->ListSize;

    // if( self->TitleWidth < strlen( NewNode->Title ) )
    // self->TitleWidth = strlen( NewNode->Title );
    // if( self->LatencyWidth < digit_width( BRN_Latency( NewNode ) ) )
    // self->LatencyWidth = digit_width( BRN_Latency( NewNode ) );
    // if( self->ThroughputWidth < digit_width( BRN_Throughput( NewNode ) ) )
    // self->ThroughputWidth = digit_width( BRN_Throughput( NewNode ) );

#define UpdateMaxValue( A, B )                                 \
    do {                                                       \
        size_t UpdateMaxValue_ = ( B );                        \
        if( ( A ) < UpdateMaxValue_ ) ( A ) = UpdateMaxValue_; \
    } while( 0 )

    UpdateMaxValue( self->TitleWidth, strlen( NewNode->Title ) );
    UpdateMaxValue( self->LatencyWidth, digit_width( BRN_Latency( NewNode ) ) );
    UpdateMaxValue( self->ThroughputWidth, digit_width( BRN_Throughput( NewNode ) ) );
}

void BA_PrintHorizontalLine( LP_BA self )
{
    putchar( '\n' );
    size_t LineWidth = self->TitleWidth + self->LatencyWidth + self->ThroughputWidth;
    for( size_t i = 0; i < LineWidth; ++i ) putchar( '_' );
    putchar( '\n' );
}

void BA_PrintSummaryHeader( LP_BA self )
{
    print( "\n    ______________________"
           "\n   /                     /"
           "\n  /  Benchmark Summary  /\n" );
    //       " /_____________________/"
    print( AlignLeft( self->TitleWidth, "/_____________________/" ) );
    print( SetWidth( self->LatencyWidth, "Latency" ) );
    print( SetWidth( self->ThroughputWidth, "Throughput" ) );
    BA_PrintHorizontalLine( self );
}

void BA_PrintSummaryLine( LP_BA self, LP_BRN BRNode )
{
    putchar( '\n' );
    print( AlignLeft( self->TitleWidth, BRNode->Title ) );
    print_num( BRN_Latency( BRNode ), self->LatencyWidth );
    print_num( BRN_Throughput( BRNode ), self->ThroughputWidth );
}

void BA_Release( LP_BA self )
{
    if( ! self ) return;
    ++self->TitleWidth;
    BA_PrintSummaryHeader( self );
    LP_BRN OldListHead;
    self->ListTail = NULL;
    while( self->ListHead )
    {
        OldListHead    = self->ListHead;
        self->ListHead = self->ListHead->Next;
        BA_PrintSummaryLine( self, OldListHead );
        BRN_Release( OldListHead );
    }
    BA_PrintHorizontalLine( self );
    free( self );
    BenchmarkResults = NULL;
}

void BenchmarkAnalyser_IndirectRelease( LP_BA* IndirectPtr ) { BA_Release( *IndirectPtr ); }
////////////////////////////// BenchmarkAnalyser [End] //////////////////////////////

////////////////////////////// BenchmarkModulator [Class] //////////////////////////////
////////////////////////////// BenchmarkModulator [Fields] //////////////////////////////
typedef struct BenchmarkModulatorTag
{
    const char* Title;
    clock_t StartTime;
    clock_t TotalCycle;
    size_t TotalIteration;
} BenchmarkModulator, *LP_BM;

////////////////////////////// BenchmarkModulator [Methods] /////////////////////////////////
LP_BM BM_New( const char* Title )
{
    print( "\nBenchmarking... " );
    print( Title );
    putchar( '\n' );
    LP_BM self = (LP_BM)malloc( sizeof( BenchmarkModulator ) );
    if( self == NULL ) FatalError( "BenchmarkModulator Construction Failed" );
    self->Title          = Title;
    self->StartTime      = clock();
    self->TotalCycle     = 0;
    self->TotalIteration = 0;
    return self;
}

bool BM_Alive( LP_BM self )
{
    self->TotalCycle = clock() - self->StartTime;
    ++self->TotalIteration;
    return self->TotalCycle < MaxCycle && self->TotalIteration < MaxIteration;
}

void BM_Release( LP_BM self )
{
    BA_PushBack( BenchmarkResults,
                 BRN_New( self->Title, self->TotalCycle, self->TotalIteration ) );
    free( self );
}

void BenchmarkModulator_IndirectRelease( LP_BM* IndirectPtr ) { BM_Release( *IndirectPtr ); }

////////////////////////////// BenchmarkModulator [End] //////////////////////////////

#define EnableBenchmark()                                                    \
    AutoRelease( BenchmarkAnalyser )* BenchmarkResults_ = BenchmarkResults = \
    BA_Init( BA_Alloc() )

#define UniqueVarID_( Name, ID ) Name##_##ID
#define UniqueVarID( Name, ID )  UniqueVarID_( Name, ID )
#define UniqueName               UniqueVarID( _0_, __LINE__ )

#define Benchmark( Title )                                               \
    ; /*control block skip*/                                             \
    AutoRelease( BenchmarkAnalyser )* UniqueName =                       \
    BenchmarkResults ? NULL : ( BenchmarkResults = BA_New() );           \
    for( AutoRelease( BenchmarkModulator )* Modulator = BM_New( Title ); \
         BM_Alive( Modulator ); )

#ifdef __cplusplus
}
#endif

#endif /* BENCHMARK_H */

#define DONT_RUN_TEST_
#ifndef DONT_RUN_TEST

#include <math.h>
void task( int k )
{
    printf( "Performing task . " );
    volatile double m;
    for( int i = 0; i < k; ++i )
        for( int j = 0; j < k; ++j ) m = sqrt( pow( cos( i ), sin( j ) ) );
    printf( "Complete.\n" );
}

int main()
{
    // EnableBenchmark();

    Benchmark( "task(50)" ) { task(50); }
    Benchmark( "task(100)" ) { task(100); }
    Benchmark( "task(200)" ) { task(200); }
    Benchmark( "task(600)" ) { task(600); }

    Benchmark( "volatile increment & super long title using a super long string..." )
    {
        for( volatile int i = 0; i < 100; ++i ) {}
    }

    return 0;
}

#endif