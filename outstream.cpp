#include "outstream.h"

#include <cstring>   // for strlen
#include <algorithm>
#include <assert.h>

void ssc::OutStream::write(const char* buf) {
    write_buffer(buf, strlen(buf));
}

void ssc::OutStream::write(u64 v, ulen radix, bool lowercase, bool separators) {
    assert(radix>=2 && radix<=36 && "invalid radix");
    if (!v) {
        write_buffer("0", 1);
        return;
    }
    const ulen BUF_LEN=64; // base-2 generates the largest number of
                           // digits which will at most generate 64.
    static const char DGT_SET[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char buf[BUF_LEN];//, *ptr=buf+BUF_LEN-1;
    ulen idx=BUF_LEN-1,count=1;
    while (v) {
        char c=DGT_SET[v%radix];
        c = (lowercase && c>'9') ? c+32 : c;
        buf[idx] = c, --idx;
        v /= radix;     
        if (separators && v && count%3 == 0)
            buf[idx] = ',', --idx;
        ++count;
    }
    write_buffer(&buf[idx+1], BUF_LEN-idx-1);
}

void ssc::OutStream::write(i64 v, bool include_plus, bool separators) {
    if (v<0) {
        write_buffer("-1", 2);
        v=-v;
    } else if (include_plus)
        write_buffer("+", 1);
    write((u64) v, 10, false, separators);
}

void ssc::OutStream::write(bool b) {
    if (b)
        write_buffer("true", 4);
    else
        write_buffer("false", 5);
}

void ssc::OutStream::write_pad(ulen& pad, char pad_char, ulen size) {
    if (pad>0) {
        if (pad>size)
            for (ulen i=0; i<pad-size; i++)
                flush_buffer(&pad_char, 1);
        pad = 0;
    }
}

void ssc::OutStream::write_buffer(const char* buf, ulen size) {
    write_pad(fmt_info.lpad, fmt_info.lpad_char, size);
    flush_buffer(buf, size);
    write_pad(fmt_info.rpad, fmt_info.rpad_char, size);
}

