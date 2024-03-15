#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>
#include <vector>
#include <X11/extensions/XTest.h>
#include "./Config.hpp"


struct operKeys{
    KeyCode backspace;
    KeyCode ctrl;
    KeyCode vLetter;
};

class UsrKeyBuffer{
    public:
        std::string keyValBuff = "";
        Config::Config* conf;
        bool listening = false;
        Display* disp;
        operKeys oper;

        UsrKeyBuffer(Config::Config *configuration, Display* display){
            conf = configuration;
            disp = display;
            oper.backspace = XKeysymToKeycode(disp, XK_BackSpace);
            oper.ctrl = XKeysymToKeycode(disp, XK_Control_L);
            oper.vLetter = XKeysymToKeycode(disp, XK_v);
        }

        void listen(bool isListening){
            listening = isListening;
        }

        bool kvlBuff(std::string val){
            if (val == "slash") val = "/";
            keyValBuff.append(val);
            if (keyValBuff.size() > conf->triggerMaxSize) {
                clearBuff();
                listen(false);
                return false;
                }
            if (conf->exist(keyValBuff)) {
                listen(false);
                return true;
                }
            return false;
        }

        void clearBuff(){
            keyValBuff = "";
        }

        void delSeq(){
            for (int i = 0; i < keyValBuff.size(); i++){
                UsendKey(disp, oper.backspace);
            }
        }

        void paste(){
            XTestFakeKeyEvent(disp, oper.ctrl, True, 0);
            XFlush(disp);

            XTestFakeKeyEvent(disp, oper.vLetter, True, 0);
            XFlush(disp);

            XTestFakeKeyEvent(disp, oper.vLetter, False, 0);
            XFlush(disp);  
        
            XTestFakeKeyEvent(disp, oper.ctrl, False, 0);
            XFlush(disp);            
        }
    private:
        void UsendKey(Display *disp, KeyCode code){
            XTestFakeKeyEvent(disp, code, True, 0);
            XFlush(disp);
        
            XTestFakeKeyEvent(disp, code, False, 0);
            XFlush(disp);
        
        }

};