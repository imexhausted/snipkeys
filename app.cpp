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

#include "Config.hpp"
#include "KeyBuffer.hpp"
#include "./clip/clip.h"

int main(int argc, char * argv[])
{
    std::string confPath = "./settings.config";
    for (int i = 1; i < argc; i++){
        if (argv[i] == "config"){
            if (argc < i+1){
                std::cout << "No config path has been provided" << '\n';
                exit(1);
            }
            confPath = argv[i + 1];
        }
        
    }

    const char * hostname    = ":0";
    // Set up X
    Display * disp = XOpenDisplay(hostname);
    if (NULL == disp)
    {
        std::cerr << "Cannot open X display: " << hostname << std::endl;
        exit(1);
    }

    // parse config file 
    Config::Config config("./settings.config");

    // init keybuffer
    UsrKeyBuffer keyBuff(&config, disp);

    // Register events
    Window root = DefaultRootWindow(disp);
    
    // set mask to chek for only needed events
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
                cookie->type == GenericEvent) {

                    if (cookie->evtype == XI_RawKeyRelease) continue; 
                    XIRawEvent *ev = (XIRawEvent*)cookie->data;
 
                    // Ask X what it calls that key
                    KeySym s = XkbKeycodeToKeysym(disp, ev->detail, 0, 0);

                    if (s == XK_slash) keyBuff.listen(true);

                    if (NoSymbol == s) continue;

                    if (!keyBuff.listening) continue;
                    
                    char *str = XKeysymToString(s);
                    if (NULL == str) continue;
                    
                    if (!keyBuff.kvlBuff(str)) continue;

                    std::string pasteValue = config.getValue(keyBuff.keyValBuff).value;
                    keyBuff.delSeq();
                    clip::set_text(pasteValue);
                    keyBuff.paste();
                    
                    keyBuff.clearBuff();     
        }
    }
}
