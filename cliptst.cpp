#include "clip/clip.h"
#include <iostream>
#include <string.h>

int main(int argc, char * argv[])
{
    std::string value = "";
    for (int i = 1; i < argc; i++){
        value = argv[i];
    }
    
    clip::set_text(value);
    
    //std::cout << value << '\n';
    return 0;
}