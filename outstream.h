//===---------------------------------------------------------===
//
// Outstream is an efficient generic output stream for
// characters.
//
// The stream supports formatting with % being the most basic
// character for format arguments. It behaves similarly to how
// C's printf works.
//
//===---------------------------------------------------------===
#ifndef SSC_OUTSTREAM_H
#define SSC_OUTSTREAM_H

#include <utility> // std::forward
#include <tuple>

#include "core_types.h"
#include "characters.h"
#include "sys.h"

namespace ssc {

class OutStream {
public:
    
    void write(const char* buf);
    void write(char* buf) { write((const char*) buf); }
    void write(u64 v, ulen radix=10, bool lowercase=false, bool separators=false);
    inline void write(u8  v) { write((u64) v); }
    inline void write(u16 v) { write((u64) v); }
    inline void write(u32 v) { write((u64) v); }
    void write(i64 v, bool include_plus=false, bool separators=false);
    // -- no i8 write since that would conflict
    //    with character writing
    inline void write(i16 v) { write((i64) v);  }
    inline void write(i32 v) { write((i64) v);  }
    inline void write(char c) {
        write_buffer(&c, 1);
    }
    void write(bool b);
    
      // Writing pointers
    template<typename T>
    inline void write(T* ptr) {
        if constexpr (sizeof(uintptr_t) == 8)
            write("%016X", (uintptr_t) ptr);
        else
            write("%08X", (uintptr_t) ptr);
    }

      // Try to write using .write member function of type.
    template<typename T>
    inline void write(T&& arg) {
        arg.write(*this);
    }
    
    template<typename T>
    void write(const char* fmt, T&& arg) {
        while (*fmt != '\0') {
            if (*fmt == '%') {
                write_arg(fmt, std::forward<T>(arg));
                continue;
            }
            write(*fmt); // non-format character
            ++fmt;
        }
    }

    template<typename T, typename... TArgs>
    void write(const char* fmt, T&& arg, TArgs&&... args) {
        for (; *fmt != '\0'; ++fmt) {
            if (*fmt == '%') {
                write_arg(fmt, std::forward<T>(arg));
                write(fmt, std::forward<TArgs>(args)...);
                break;
            }
            write(*fmt); // non-format character
        }
    }
    
    void writeln() {
        write_buffer("\n", 1);
    }

    template<typename T>
    void writeln(T&& value) {
        write(std::forward<T>(value));
        write_buffer("\n", 1);
    }

    template<typename... TArgs>
    void writeln(const char* fmt, TArgs&&... args) {
        write(fmt, std::forward<TArgs>(args)...);
        write_buffer("\n", 1);
    }
   
private:

    void parse_digit_info(const char*& fmt,
                          bool& lower_hex, bool& expects_int, ulen& radix) {
                                   // a b c d  e f g h i j k l m n o p q r s t u v w x
        static const char rdx_tab[] { 0,2,0,10,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,16 };
        switch (*fmt) {
        case 'd': case 'x': case 'o': case 'b':
                  case 'X':
            lower_hex = *fmt != 'X';
            radix = rdx_tab[(*fmt-97u)%32];
            ++fmt;
            expects_int = true;
            break;
        case 's':
            ++fmt;
            break;
        default:
            DBG_PANIC("Invalid format expected type info");
            break;
        }
    }

    template<typename T>
    void write_arg(const char*& fmt, T&& arg) {
    using NoRT=std::remove_reference_t<T>;

    #define RD_PAD(cc, d)            \
        fmt_info.d ## pad_char = cc; \
        while (is_digit(*fmt)) {     \
            fmt_info.d ## pad *= 10; \
            fmt_info.d ## pad += (*fmt-'0'), ++fmt; }
        
        ++fmt; // eat the %
        
        // check for escape character
        if (*fmt == '%') {
            ++fmt;
            write_buffer("%", 1);
            return;
        }

        bool lower_hex=true, expects_int=false;
        ulen radix=10;
        
        bool flags[]{0,0,0,0}; // format-flags
        enum { fmin=0, fplus, fcom, fzero };
        while (*fmt == '-' || *fmt == '+' ||
               *fmt == ',' || *fmt == '0') {
            ulen f=0;
            switch (*fmt){
            case '-': f=fmin;  break;
            case '+': f=fplus; break;
            case ',': f=fcom;  break;
            case '0': f=fzero; break;
            }
            if (flags[f]) DBG_PANIC("Duplicate format flag");
            else flags[f] = true, ++fmt;
        }
        
        if (flags[fmin] && flags[fzero])
            DBG_PANIC("Incompatible format flags `-` and `0`");
        if (flags[fplus] | flags[fcom] | flags[fzero])
            expects_int = true;

        // write out old format information if it exist.
        if (fmt_info.lpad)
            write_pad(fmt_info.lpad, fmt_info.lpad_char, 0);
        else if (fmt_info.rpad)
            write_pad(fmt_info.rpad, fmt_info.rpad_char, 0);

        switch (*fmt) {
        case 'd': case 'x': case 'o': case 'b':
                  case 'X':
            if (flags[fmin])
                DBG_PANIC("Missing format width with `-` flag");
            parse_digit_info(fmt, lower_hex, expects_int, radix);
            break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8':
        case '9':
            if (flags[fmin]) {
                RD_PAD(' ', r)
            } else if (flags[fzero]) {
                RD_PAD('0', l)
            } else {
                RD_PAD(' ', l)
            }
            parse_digit_info(fmt, lower_hex, expects_int, radix);
            break;
        case 's':
            ++fmt;
            break;
        default:
            DBG_PANIC("Invalid format expected type info");
            break;
        }

        if (flags[fplus] && radix != 10)
            DBG_PANIC("Format flag `+` expects decimal formatting");
        if (flags[fcom] && radix != 10)
            DBG_PANIC("Format flag `,` expects decimal formatting");

        if constexpr (!std::is_integral_v<NoRT>)
            if (expects_int)
                DBG_PANIC("Formatting expects integer argument");

        if constexpr (std::is_integral_v<NoRT>) {          
            if constexpr (std::is_signed_v<NoRT>)
                if (radix == 10)
                    write((i64)arg, flags[fplus], flags[fcom]);
                else
                    write((u64) arg, radix, lower_hex, flags[fcom]);
            else
                write((u64) arg, radix, lower_hex, flags[fcom]);
        } else
            write(std::forward<T>(arg));
    #undef RD_PAD
    }
 
    struct FmtInfo {
        // left pad  '   foo'
        ulen lpad=0;
        char lpad_char;
        // right pad 'foo    '
        ulen rpad=0;
        char rpad_char;
    } fmt_info;

    void write_pad(ulen& pad, char pad_char, ulen size);
    void write_buffer(const char* buf, ulen size);

protected:

    virtual void flush_buffer(const char* buf, ulen size) = 0;

};

}

#endif
