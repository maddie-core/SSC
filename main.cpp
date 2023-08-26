#include "fmt.h"
#include "util/List.h"

#include <string>

int main() {
    {
        ssc::ArenaAllocator allocator(1024);
        int* a=allocator.alloc<int>();
        int* b=allocator.alloc<int>();
        *a=44;
        *b=82;
        struct CC {
            int a;
            int b;
        };
        CC* cc=allocator.alloc<CC>();
        cc->a=111;
        cc->b=222;
        ssc::println("%s %s %s %s", *a, *b, cc->a, cc->b);
    }
    {
        ssc::List<int> l1 = { 4, 2, 6, 1 };
        ssc::List<int> l2 = l1;
        ssc::List<int> l3 = std::move(l1);
        ssc::List<int> l4;
        l4 = l2;

        ssc::println("l1:");
        for (int v : l1) {
            ssc::println(v);
        }
        ssc::println("l2:");
        for (int v : l2) {
            ssc::println(v);
        }
        ssc::println("l3:");
        for (int v : l3) {
            ssc::println(v);
        }
        ssc::println("l4:");
        for (int v : l4) {
            ssc::println(v);
        }
    }
    {
        ssc::println("\n\n");
        ssc::List<std::string> l1;
        l1.add("maddie");
        l1.add("john");
        l1.add("jerry");
        l1.add("sally");
        l1.add("lily");
        l1.add("bob!");
        ssc::println("contents:");
        for (const std::string& name : l1) {
            ssc::println(name);
        }
        l1.pop_back();
        ssc::println("contents:");
        for (const std::string& name : l1) {
            ssc::println(name);
        }
        l1.pop_back_n(3);
        ssc::println("contents:");
        for (const std::string& name : l1) {
            ssc::println(name);
        }
        l1.add("luna");
        l1.add("ava");
        l1.remove(l1.find("john"));
        ssc::println("contents:");
        for (const std::string& name : l1) {
            ssc::println(name);
        }
        ssc::List<std::string> l2 = { "maddie", "luna", "ava" };
        ssc::println("list equal=%s", l1 == l2);
        l1.pop_front_n(2);
        ssc::println("contents:");
        for (const std::string& name : l1) {
            ssc::println(name);
        }
    }
}
