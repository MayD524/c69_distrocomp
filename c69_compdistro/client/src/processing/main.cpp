#include <iostream>
#include <vector>
#include <string>
#include <map>


#ifdef _WIN32
#   include <windows.h>
#   include <conio.h>
#else
#   include <unistd.h>
#endif

#include <signal.h>
#include <cstdlib>

#include "./inc/script_lang.hpp"
#include "./inc/system.hpp"
using namespace std;

// handle sigtrap

int main( void ) {
    // handle sigtrap
    ScriptLang scriptHandler;
    vector<string> test = {
        "assign a 1",
        "out *a",
        "add *a 1",
        "out *a",
        "jmp 2"
    };
    scriptHandler.parse(test);
    scriptHandler.execute();
    //scriptHandler.displayPtrs();
    return 0;
}