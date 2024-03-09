#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <array>

enum ParseFlag {CONFIG, KEYCODES};

struct Pair{
    std::string key;
    std::string value;
    long unsigned int triggerSize;
    long unsigned int vlueSize;
};

struct Key{
    int code;
    std::string key;
};

class Config {
    public:
        std::vector<Pair> values;
        std::string configStr;
        std::ifstream file;
        std::string keyboard;
        std::vector<Key> keys;
        int triggerMaxSize = 0;

        Config(std::string confPath, std::string keysPath){
            parse(confPath, CONFIG);
            parse(keysPath, KEYCODES);
            
        };

        void parse(std::string path, ParseFlag flag){
            file.open(path);
            if (file.is_open()){
                while (getline(file, configStr)){
                    if (flag == CONFIG){
                        _subsConf();
                        
                    }
                    if (flag == KEYCODES){
                        _subsKey();
                    }
                }

            } else {
                std::cout << "Error" << std::endl;
                perror("File not open");
            }
            file.close();
        };

        std::string getKey(int keyCode){
            std::string ret = "";
            for (int i = 0; i < keys.size(); i++){
                if (keys[i].code == keyCode){
                    ret = keys[i].key;
                    break;
                }
            }
            return ret;
        }

        Pair getValue(std::string key){
            Pair ret = {"Not found", "Not found", 0, 0};
            for (int i = 0; i < values.size(); i++){
                if (values[i].key == key){
                    ret = values[i];
                    break;
                }
            }
            return ret;
        };
        
    private:
        void _subsConf()
        {
            std::string trigger = configStr.substr(0, configStr.find("="));
            std::string value = configStr.substr(configStr.find("=")+1, configStr.find("\n"));
            if (configStr[0] == '/'){
                if (trigger.size() > triggerMaxSize){
                    triggerMaxSize = trigger.size();
                }
                Pair obj = {trigger, value, trigger.size(), value.size()};
                values.push_back(obj);    
            } else if (configStr[0] == '#' && trigger == "#keyboard"){
                keyboard = value;
            }
        }

        void _subsKey(){
            std::string code = configStr.substr(0, configStr.find("="));
            std::string value = configStr.substr(configStr.find("=")+1, configStr.find("\n"));
            int codeint = std::stoi(code);
            Key obj = {codeint, value};
            keys.push_back(obj);
        }

};