#ifndef SSC_SYS_H
#define SSC_SYS_H

namespace ssc {

#if NDEBUG
#define DBG_PANIC(err)
#else
#define DBG_PANIC(err) panic(err)
#endif

void panic(const char* err, char exit_code=1);
}

#endif
