#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>
#include <vector>
#include <X11/extensions/XTest.h>
#include "./Config.hpp"

struct Modifiers{
    bool Ctrl = false;
    bool Alt = false;
    bool Shift = false;
};

class UsrKeyBuffer{
    public:
        std::vector<std::string> keyBuff;

        Modifiers mod;

        void setMod(char * option){
            if (option == "Alt_L"){}
        }

        void remove(std::string key){
            for (std::vector<std::string>::iterator i = keyBuff.begin(); i != keyBuff.end(); i++){
                if (i->data() == key){
                    keyBuff.erase(i);
                }
            }
        }

        bool checkRemove(std::string key){
            for (std::vector<std::string>::iterator i = keyBuff.begin(); i != keyBuff.end(); i++){
                if (i->data() == key){
                    keyBuff.erase(i);
                    return true;
                }
            }
            return false;
        }

        void add(std::string key){
            keyBuff.push_back(key);
        }

        void delSeq(Display* disp, std::string seq){
            KeySym backspace = XK_BackSpace;
            KeyCode cbackspace = XKeysymToKeycode(disp, backspace);
            for (int i = 0; i < seq.size(); i++){
                UsendKey(disp, backspace);
            }
        }

        void UsendKey(Display *disp, KeyCode code){
            XTestFakeKeyEvent(disp, code, True, 0);
            XFlush(disp);
        
            XTestFakeKeyEvent(disp, code, False, 0);
            XFlush(disp);
        
        }
};