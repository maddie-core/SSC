#include "sys.h"

#include "fmt.h"

#ifdef _WIN32
#include <Windows.h>
#include <dbghelp.h>
#endif

#include <string>
#include <algorithm>

static bool PREVENT_CIRCULAR = false;

namespace ssc {
static void try_stacktrace() {

// Enforce debug mode because otherwise symbol information will not
// even properly exist.
#if !NDEBUG
#ifdef _WIN32
    HANDLE hprocess=GetCurrentProcess();
    if (SymInitialize(hprocess, NULL, TRUE) == FALSE)
        return;
    const ulen TRACE_SIZE=10;
    void* stack[TRACE_SIZE+1];
    USHORT nframes=CaptureStackBackTrace(0, TRACE_SIZE, stack, NULL);

    eprintln("-------------------------");
    eprintln("Current process stacktrace\n");
    
    char sym_buffer[ sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) ] {0};
    SYMBOL_INFO* symbol=(SYMBOL_INFO*)sym_buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    eprintln(" | function location              | function name                  | address");
    eprintln(" .--------------------------------.--------------------------------.-----------------");
    for (ulen i=0; i<nframes; i++) {
        DWORD64 displacement1=0;
        DWORD   displacement2=0;
        IMAGEHLP_LINE64 loc_info {0};
        loc_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        if (SymFromAddr(hprocess,
                        (DWORD64)stack[i],
                        &displacement1,
                        symbol) == FALSE) {
            return;
        }
        if (SymGetLineFromAddr64(hprocess,
                                 (DWORD64)stack[i],
                                 &displacement2,
                                 &loc_info) == FALSE) {
            return;
        }
        
        const char* func_name=(const char*)symbol->Name;
        std::string file_path{ loc_info.FileName };
        std::replace(file_path.begin(), file_path.end(), '\\', '/');
        std::string file_loc = "." + file_path.substr(strlen(PROJECT_SOURCE_PATH));

        ulen addr=symbol->Address;
        eprintln(" | %-30s | %-30s | %X", file_loc.c_str(), func_name, addr);
        if (strcmp(func_name, "main") == 0) {
            break;
        }
    }
    eprintln();
#endif
#endif
}
}

void ssc::panic(const char* err, char exit_code) {
    if (PREVENT_CIRCULAR)
        exit(255);
    PREVENT_CIRCULAR = true;
    eprintln("\nPanic Termination");
    try_stacktrace();
    eprintln(">> Reason: %s", err);
    exit(exit_code);
}

