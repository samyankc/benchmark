#ifndef BENCHMARK_H
#define BENCHMARK_H
#define CBENCHMARK_H

//#include <immintrin.h>  //__rdtsc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MaxIteration 7890
#define MaxCycle     ( 1 * CLOCKS_PER_SEC )

#define Finally( fn )    __attribute__( ( cleanup( fn ) ) )
#define AutoRelease( T ) Finally( T##_IndirectRelease ) T

    void print( const char* str, size_t MinWidth = 0, bool AlignLeft = false )
    {
        size_t StringWidth  = strlen( str );
        size_t PaddingWidth = ( MinWidth > StringWidth ) * ( MinWidth - StringWidth );
        for( size_t i = 0; ! AlignLeft && i < PaddingWidth; ++i ) putchar( ' ' );
        for( ; *str; ++str ) putchar( *str );
        for( size_t i = 0; AlignLeft && i < PaddingWidth; ++i ) putchar( ' ' );
    }

    void print_num( long long n, size_t MinWidth = 0 )
    {
        char Buff[ 22 ];
        itoa( n, Buff, 10 );
        print( Buff, MinWidth );
    }

    size_t digit_width( long long n )
    {
        if( n < 0 ) return 1 + digit_width( -n );
        if( n < 10 ) return 1;
        if( n < 100 ) return 2;
        if( n < 1000 ) return 3;
        if( n < 10000 ) return 4;
        if( n < 100000 ) return 5;
        if( n < 1000000 ) return 6;
        if( n < 10000000 ) return 7;
        if( n < 100000000 ) return 8;
        if( n < 1000000000 ) return 9;
        if( n < 10000000000 ) return 10;
        if( n < 100000000000 ) return 11;
        return 22;
    }

    ////////////////////////////// BenchmarkResultNode [Class] //////////////////////////////
    ////////////////////////////// BenchmarkResultNode [Fields] //////////////////////////////
    typedef struct BenchmarkResultNodeTag
    {
        const char* Title;
        clock_t TotalCycle;
        size_t TotalIteration;
        BenchmarkResultNodeTag* Next;
    } BenchmarkResultNode, *LP_BRN;

    ////////////////////////////// BenchmarkResultNode [Methods] //////////////////////////////
    size_t BRN_Latency( LP_BRN self )
    {
        return CLOCKS_PER_SEC * self->TotalCycle / self->TotalIteration;
    }
    size_t BRN_Throughput( LP_BRN self )
    {
        return self->TotalCycle ? 1000 * self->TotalIteration / self->TotalCycle : 0;
    }
    bool BRN_LessThan( LP_BRN self, LP_BRN RHS )
    {
        return strlen( self->Title ) < strlen( RHS->Title );
    }

    LP_BRN BRN_Alloc() { return (LP_BRN)malloc( sizeof( BenchmarkResultNode ) ); }

    LP_BRN BRN_Init( LP_BRN self, const char* Title, clock_t TotalCycle, size_t TotalIteration,
                     LP_BRN Next )
    {
        if( self != NULL ) *self = { Title, TotalCycle, TotalIteration, Next };
        return self;
    }

    LP_BRN BRN_New( const char* Title, clock_t TotalCycle, size_t TotalIteration,
                    LP_BRN Next = NULL )
    {
        return BRN_Init( BRN_Alloc(), Title, TotalCycle, TotalIteration, Next );
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

    ////////////////////////////// BenchmarkAnalyser [Static Object] //////////////////////////////
    static BenchmarkAnalyser* BenchmarkResults = NULL;

    ////////////////////////////// BenchmarkAnalyser [Methods] //////////////////////////////
    LP_BA BA_Init( LP_BA self )
    {
        BenchmarkResults = NULL;
        /*      */ *self = { NULL,
                             NULL,
                             0,
                             strlen( " /_____________________/   " ),
                             strlen( "     Latency" ),
                             strlen( "     Throughput" ) };
        return self;
    }

    LP_BA BA_Alloc() { return (LP_BA)malloc( sizeof( BenchmarkAnalyser ) ); }

    void BA_PushBack( LP_BA self, LP_BRN NewNode )
    {
        if( self->ListHead == NULL )  // new container
            self->ListTail = self->ListHead = NewNode;
        else
            self->ListTail = self->ListTail->Next = NewNode;
        ++self->ListSize;

        if( self->TitleWidth < strlen( NewNode->Title ) )
            self->TitleWidth = strlen( NewNode->Title );
        if( self->LatencyWidth < digit_width( BRN_Latency( NewNode ) ) )
            self->LatencyWidth = digit_width( BRN_Latency( NewNode ) );
        if( self->ThroughputWidth < digit_width( BRN_Throughput( NewNode ) ) )
            self->ThroughputWidth = digit_width( BRN_Throughput( NewNode ) );
    }

    void BA_PrintHorizontalLine( LP_BA self )
    {
        putchar( '\n' );
        size_t LineWidth = self->TitleWidth + self->LatencyWidth + self->ThroughputWidth;
        for( int i = 0; i < LineWidth; ++i ) putchar( '_' );
        putchar( '\n' );
    }

    void BA_PrintSummaryHeader( LP_BA self )
    {
        print(
        "\n    ______________________"
        "\n   /                     /"
        "\n  /  Benchmark Summary  /"
        "\n /_____________________/   " );
        print( "Latency", self->LatencyWidth );
        print( "Throughput", self->ThroughputWidth );
        BA_PrintHorizontalLine( self );
    }

    void BA_PrintSummaryLine( LP_BA self, LP_BRN BRNode )
    {
        putchar( '\n' );
        print( " " );
        print( BRNode->Title, self->TitleWidth - 1, true );
        print_num( BRN_Latency( BRNode ), self->LatencyWidth );
        print_num( BRN_Throughput( BRNode ), self->ThroughputWidth );
    }

    void BA_Release( LP_BA self )
    {
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

    void BenchmarkAnalyser_IndirectRelease( LP_BA* IndirectPtr )
    {
        return BA_Release( *IndirectPtr );
    }
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

    ////////////////////////////// BenchmarkModulator [Methods] //////////////////////////////
    LP_BM BM_New( const char* Title = "Unspecified" )
    {
        print( "\nBenchmarking... " );
        print( Title );
        putchar( '\n' );
        LP_BM self = (LP_BM)malloc( sizeof( BenchmarkModulator ) );
        *self      = { Title, clock(), 0, 0 };
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

    void BenchmarkModulator_IndirectRelease( LP_BM* IndirectPtr )
    {
        return BM_Release( *IndirectPtr );
    }

    ////////////////////////////// BenchmarkModulator [End] //////////////////////////////
#define EnableBenchmark() \
    AutoRelease( BenchmarkAnalyser )* BenchmarkResults_ = BenchmarkResults = BA_Init( BA_Alloc() )
#define Benchmark( Title ) \
    for( AutoRelease( BenchmarkModulator )* Modulator = BM_New( Title ); BM_Alive( Modulator ); )

#ifdef __cplusplus
}
#endif

#endif /* BENCHMARK_H */

#define RUN_TEST
#ifdef RUN_TEST

int main()
{
    EnableBenchmark();

    Benchmark( "puts" )
    {  //
        puts( "This is a test string." );
    }

    Benchmark( "print" )
    {  //
        print( "This is a test string.\n" );
    }

    Benchmark( "no-op" )
    {
        for( volatile int i = 0; i < 2000; ++i ) { ++i; }
    }
    return 0;
}

#endif