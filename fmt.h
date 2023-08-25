//===---------------------------------------------------------===
//
// Functions for writing to standard output and standard error
// may be found here.
//
// The functions allow for formatted output by using the %
// character in the format string to specify an argument. The
// functions behave similarly to that of C's printf.
//
//===---------------------------------------------------------===
#ifndef SSC_FMT_H
#define SSC_FMT_H

#include "outstream.h"

namespace ssc {

class PrintStream : public OutStream {
public:
    enum class Catagory {
        Stdout,
        Stderr
    } catagory;

    PrintStream(Catagory catagory) :
        catagory(catagory)
    {}

protected:
    void flush_buffer(const char* buf, ulen size) override;
}
extern stdout_stream,
       stderr_stream;

enum class TermColor {
    Default        = 0x7,
    Black          = 0x0,
    Dark_blue      = 0x1,
    Dark_green     = 0x2,
    Bright_blue    = 0x3,
    Red            = 0x4,
    Magenta        = 0x5,
    Yellow         = 0x6, // mustard yellow
    Dark_gray      = 0x8,
    Blue           = 0x9,
    Bright_green   = 0xA,
    Cyan           = 0xB,
    Bright_red     = 0xC,
    Bright_magenta = 0xD,
    Bright_yellow  = 0xE,
    White          = 0xF,
};

void set_terminal_color(TermColor color);

// ===------------------------------------------------------
// Functions for writing to standard output


/// Prints the value to standard output stream.
///
template<typename T>
inline void print(T&& value) {
    stdout_stream.write(std::forward<T>(value));
}

/// Prints a formatted string with arguments to standard output
/// stream.
///
template<typename... TArgs>
inline void print(const char* fmt, TArgs&&... args) {
    stdout_stream.write(fmt, std::forward<TArgs>(args)...);
}

/// Prints a value to standard output stream with a new line
/// at the end.
///
template<typename T>
inline void println(T&& value) {
    stdout_stream.writeln(std::forward<T>(value));
}

/// Prints a formatted string with arguments to standard output
/// stream with a new line at the end.
///
template<typename... TArgs>
inline void println(const char* fmt, TArgs&&... args) {
    stdout_stream.writeln(fmt, std::forward<TArgs>(args)...);
}

// Prints a new line to the standard error stream.
//
inline void println() {
    stdout_stream.writeln();
}

// ===------------------------------------------------------
// Functions for writing to standard error


/// Prints the value to standard error stream.
///
template<typename T>
inline void eprint(T&& value) {
    stderr_stream.write(std::forward<T>(value));
}

/// Prints a formatted string with arguments to standard error
/// stream.
///
template<typename... TArgs>
inline void eprint(const char* fmt, TArgs&&... args) {
    stderr_stream.write(fmt, std::forward<TArgs>(args)...);
}

/// Prints a value to standard error stream with a new line
/// at the end.
///
template<typename T>
inline void eprintln(T&& value) {
    stderr_stream.writeln(std::forward<T>(value));
}

/// Prints a formatted string with arguments to standard error
/// stream with a new line at the end.
///
template<typename... TArgs>
inline void eprintln(const char* fmt, TArgs&&... args) {
    stderr_stream.writeln(fmt, std::forward<TArgs>(args)...);
}

// Prints a new line to the standard error stream.
//
inline void eprintln() {
    stderr_stream.writeln();
}

}

#endif
