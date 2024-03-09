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

int main(int argc, char * argv[])
{
    const char * hostname    = ":0";
 

    // Set up X
    Display * disp = XOpenDisplay(hostname);
    if (NULL == disp)
    {
        std::cerr << "Cannot open X display: " << hostname << std::endl;
        exit(1);
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
                cookie->type == GenericEvent) 
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
