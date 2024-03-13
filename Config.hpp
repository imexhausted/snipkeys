#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <array>

namespace Config{

struct Pair{
    std::string key;
    std::string value;
    long unsigned int triggerSize;
    long unsigned int valueSize;
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
        int triggerMaxSize = 0;

        Config(std::string confPath){
            parse(confPath);
        };

        void parse(std::string path){
            file.open(path);
            if (file.is_open()){
                while (getline(file, configStr)){
                    _subsConf();
                }

            } else {
                std::cout << "Error" << std::endl;
                perror("File not open");
            }
            file.close();
        };

        Pair getValue(std::string key){
            Pair ret = {"Not found", "Not found", 0, 0};
            for (int i = 0; i < values.size(); i++){
                if (values[i].key == key){
                    return values[i];
                    //break;
                }
            }
            return ret;
        };

        bool exist(std::string key){
            for (int i = 0; i < values.size(); i++){
                if (values[i].key == key){
                    return true;
                }
            }
            return false;            
        }
        
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
            }
        }

};

}