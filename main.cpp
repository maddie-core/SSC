#include "fmt.h"
#include "sys.h"

#include <string>

struct Person {
    std::string name;
    int age;

    void write(ssc::OutStream& os) {
        os.write("{ %s, age=%s }", name.c_str(), age);
    }
};

int main() {
    int  a;
    int* b=&a;
    bool c=false;
    Person maddie{ "maddie", 27 };

    ssc::println("%+d", 341);

    ssc::println("%-15X%s", 44, 22);
    ssc::println("okayyyy\n");
    ssc::println(44);
    ssc::println("%08X %s %s %s %s %s \n", 5234124, "heyo!!", 451, b, maddie, c);
    ssc::println("%15s%7d %-4s%s", "yo!", 33, "ok!", "ok!");
    ssc::println("%,d", 37541241);
    ssc::println("%%%s", 50);
}
