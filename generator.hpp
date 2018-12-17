#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#ifndef _GENERATOR_H
#define _GENERATOR_H

class generator
{
private:
    /* row device template as read from file */
    std::vector<std::string> device_template_;
    /* processed device container */
    std::map<std::string,std::tuple<std::string,std::string> > device_map_; 
    
    bool is_discarded(const std::string& str);
    bool is_device(const std::string& str, std::string& device_name);
    bool is_member(const std::string& str);
    bool is_closing(const std::string& str);
    bool is_opening(const std::string& str);
public:
    generator(std::string devices);
    void print_device_map();
    void parse();


};




#endif


