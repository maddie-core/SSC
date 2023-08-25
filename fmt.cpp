#include "fmt.h"

#ifdef _WIN32
#include <Windows.h>
#endif

// Satisfying linkage
//
namespace ssc {
PrintStream stdout_stream { PrintStream::Catagory::Stdout };
PrintStream stderr_stream { PrintStream::Catagory::Stderr };
}

void ssc::set_terminal_color(TermColor color) {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD) color);
#endif
}

void ssc::PrintStream::flush_buffer(const char* buf, ulen size) {
#ifdef _WIN32
        HANDLE hdl=GetStdHandle(catagory == Catagory::Stdout
                                ? STD_OUTPUT_HANDLE
                                : STD_ERROR_HANDLE
                               );
        DWORD written;
        WriteFile(hdl, buf, (DWORD) size, &written, nullptr);
#endif
}

