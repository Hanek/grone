#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
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
    class data_unit
    {
    public:
        std::vector<std::tuple<std::string,std::string> > members_;
    };
    /* processed device container */
    std::map<std::string,data_unit> device_map_; 
    
    bool is_discarded(const std::string& str);
    bool is_device(const std::string& str, std::string& device_name);
    bool is_member(const std::string& str);
    bool is_closing(const std::string& str);
    bool is_opening(const std::string& str);
public:
    generator(std::string devices);
    void print_device_map();
    void dump_header(const std::string& str);
    void dump_source(const std::string& str);
    void parse();


};




#endif


