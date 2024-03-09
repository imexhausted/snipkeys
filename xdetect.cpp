#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include <X11/XKBlib.h>
#include <X11/extensions/XTest.h>

void UsendKey(Display *disp, KeyCode code){
    XTestFakeKeyEvent(disp, code, True, 0);
    XFlush(disp);
    
    XTestFakeKeyEvent(disp, code, False, 0);
    XFlush(disp);
    
}

// usage for -help flag when executing file
int printUsage(std::string application_name) 
{
    std::cout << "USAGE: " << application_name << " [-display ] [-enumerate] [-help]" << std::endl;
    std::cout << "display      target X display                   (default :0)" << std::endl;
    std::cout << "enumerate    enumerate all X11 displays" << std::endl;
    std::cout << "help         print this information and exit" << std::endl;
 
    exit(0);
}


//iterates through files in /tmp/.X11-unix directory and enumerates all displays based on filenames
std::vector<std::string> EnumerateDisplay()
{
    // declare return variable
    std::vector<std::string> displays;
    
    //iterate through /tmp/.X11-unix
    for (auto &p : std::filesystem::directory_iterator("/tmp/.X11-unix"))
    {
        
        std::string path = p.path().filename().string(); // convert filepath to string
        std::string display_name = ":";
        
        if (path[0] != 'X') continue; // skip non-X files at the beginning if the name
        
        path.erase(0, 1); // delete X at the start
        display_name.append(path); // modify path file name
        
        Display *disp = XOpenDisplay(display_name.c_str()); // then open it with this function from X11 lib
        if (disp != NULL) 
        {
            int count = XScreenCount(disp); // count screens
            printf("Display %s has %d screens\n", //print screens
                display_name.c_str(), count);
 
            int i;
            for (i=0; i<count; i++)
                printf(" %d: %dx%d\n",
                    i, XDisplayWidth(disp, i), XDisplayHeight(disp, i)); // print display resolution
 
            XCloseDisplay(disp); //close display 
            
            displays.push_back(display_name); //add display name to list
        }
    }
    
    return displays;
}
 
int main(int argc, char * argv[])
{
    const char * hostname    = ":0";
 
    // Get arguments/flags when executong app e.g. ./app -help -display etc.
    for (int i = 1; i < argc; i++)
    {
        if      (!strcmp(argv[i], "-help"))
            printUsage(argv[0]);
        else if (!strcmp(argv[i], "-display"))  
            hostname    = argv[++i];
        else if (!strcmp(argv[i], "-enumerate"))
        {
            EnumerateDisplay();
            return 0;
        }
        else
        { 
            std::cerr << "Unknown argument: " << argv[i] << std::endl;
            printUsage(argv[0]); 
        }
    }
 
    // Set up X
    Display * disp = XOpenDisplay(hostname);
    if (NULL == disp)
    {
        std::cerr << "Cannot open X display: " << hostname << std::endl;
        exit(1);
    }
 
    // Test for XInput 2 extension
    int xiOpcode, queryEvent, queryError;
    if (! XQueryExtension(disp, "XInputExtension", &xiOpcode, &queryEvent, &queryError)) 
    {
        std::cerr << "X Input extension not available" << std::endl;
        exit(2);
    }
    { // Request XInput 2.0, guarding against changes in future versions
        int major = 2, minor = 0;
        int queryResult = XIQueryVersion(disp, &major, &minor);
        if (queryResult == BadRequest) 
        {
            std::cerr << "Need XI 2.0 support (got " << major << "." << minor << std::endl;
            exit(3);
        }
        else if (queryResult != Success) 
        {
            std::cerr << "Internal error" << std::endl;
            exit(4);
        }
    }
 
    // Register events
    Window root = DefaultRootWindow(disp);
    
    XIEventMask m;
    m.deviceid = XIAllMasterDevices;
    m.mask_len = XIMaskLen(XI_LASTEVENT);
    m.mask = (unsigned char*)calloc(m.mask_len, sizeof(char));
    XISetMask(m.mask, XI_RawKeyPress);
    XISetMask(m.mask, XI_RawKeyRelease);
    
    XISelectEvents(disp, root, &m, 1);
    XSync(disp, false);
    free(m.mask);
 
    while (true) 
    {
        XEvent event;
        XGenericEventCookie *cookie = (XGenericEventCookie*)&event.xcookie;
        XNextEvent(disp, &event);
 
        if (XGetEventData(disp, cookie) &&
                cookie->type == GenericEvent &&
                cookie->extension == xiOpcode) 
        {
            switch (cookie->evtype)
            {
                case XI_RawKeyRelease:
                case XI_RawKeyPress: 
                {
                    XIRawEvent *ev = (XIRawEvent*)cookie->data;
 
                    // Ask X what it calls that key
                    KeySym s = XkbKeycodeToKeysym(disp, ev->detail, 0, 0);
                    if (NoSymbol == s) continue;
                    char *str = XKeysymToString(s);
                    if (NULL == str) continue;
 
                    std::cout << (cookie->evtype == XI_RawKeyPress ? "+" : "-") << str << " " << std::flush;
                    //UsendKey(disp, ev->detail);
                    break;
                }
            }
        }
    }
}
