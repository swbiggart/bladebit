#include "Log.h"


FILE* Log::_outStream = nullptr;
FILE* Log::_errStream = nullptr;

bool Log::_verbose = false;

#if DBG_LOG_ENABLE
    std::atomic<int> _dbglock = 0;
#endif

//-----------------------------------------------------------
inline FILE* Log::GetOutStream()
{
    if( _outStream == nullptr )
    {
        _outStream = stdout;
        //setvbuf( stdout, NULL, _IONBF, 0 );
    }

    return _outStream;
}

//-----------------------------------------------------------
inline FILE* Log::GetErrStream()
{
    if( _errStream == nullptr )
    {
        _errStream = stderr;
        //setvbuf( stderr, NULL, _IONBF, 0 );
    }

    return _errStream;
}

//-----------------------------------------------------------
void Log::Write( const char* msg, ... )
{
    va_list args;
    va_start( args, msg );
    
    Write( msg, args );

    va_end( args );
}

//-----------------------------------------------------------
void Log::WriteLine( const char* msg, ... )
{
    va_list args;
    va_start( args, msg );
    
    WriteLine( msg, args );

    va_end( args );
}

//-----------------------------------------------------------
void Log::Line( const char* msg, ... )
{
    va_list args;
    va_start( args, msg );
    
    WriteLine( msg, args );

    va_end( args );
}

//-----------------------------------------------------------
void Log::Write( const char* msg, va_list args )
{
    vfprintf( GetOutStream(), msg, args );
}

//-----------------------------------------------------------
void Log::WriteLine( const char* msg, va_list args )
{
    FILE* stream = GetOutStream();
    vfprintf( stream, msg, args );
    fputc( '\n', stream );
}

//-----------------------------------------------------------
void Log::Error( const char* msg, ... )
{
    va_list args;
    va_start( args, msg );
    
    Error( msg, args );

    va_end( args );
}

//-----------------------------------------------------------
void Log::WriteError( const char* msg, ... )
{
    va_list args;
    va_start( args, msg );
    
    WriteError( msg, args );

    va_end( args );
}

//-----------------------------------------------------------
void Log::Error( const char* msg, va_list args )
{
    WriteError( msg, args );
    fputc( '\n', GetErrStream() );
}

//-----------------------------------------------------------
void Log::WriteError( const char* msg, va_list args )
{
    vfprintf( GetErrStream(), msg, args );
}

//-----------------------------------------------------------
void Log::Verbose( const char* msg, ...  )
{
    if( !_verbose )
        return;
    
    va_list args;
    va_start( args, msg );
    
    FILE* stream = GetErrStream();
    vfprintf( stream, msg, args );
    fputc( '\n', stream );

    va_end( args );
}

//-----------------------------------------------------------
void Log::VerboseWrite( const char* msg, ...  )
{
    if( !_verbose )
        return;

    va_list args;
    va_start( args, msg );
    
    vfprintf( GetErrStream(), msg, args );

    va_end( args );
}


//-----------------------------------------------------------
void Log::Flush()
{
    fflush( GetOutStream() );
}


//-----------------------------------------------------------
void Log::FlushError()
{
    fflush( GetErrStream() );
}


#if DBG_LOG_ENABLE

//-----------------------------------------------------------
void Log::Debug( const char* msg, ... )
{
    va_list args;
    va_start( args, msg );

    const size_t BUF_SIZE = 1024;
    char buffer[BUF_SIZE];

    int count = vsnprintf( buffer, BUF_SIZE, msg, args );
    va_end( args );

    ASSERT( count >= 0 );
    count = std::min( count, (int)BUF_SIZE-1 );

    buffer[count] = '\n';

    // Lock
    int lock = 0;
    while( !_dbglock.compare_exchange_weak( lock, 1 ) )
        lock = 0;
    
    fwrite( buffer, 1, (size_t)count+1, stderr );
    fflush( stderr );

    // Unlock
    _dbglock.store( 0, std::memory_order_release );
}

#endif
