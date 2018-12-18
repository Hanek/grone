#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>

#include  <plog/Log.h>
#include "generator.hpp"



void split(const std::string& str, std::vector<std::string>& tokens)
{
    std::stringstream ss(str);
    std::string tok;
    while(ss >> tok)
    {
        tokens.push_back(tok);
    }
}



generator::generator(std::string devices)
{
    std::ifstream input(devices);
    for(std::string line; getline(input, line);)
    {
        device_template_.push_back(line);
    }
    input.close();
}


void generator::parse()
{
    bool device        = false;
    bool open_bracket  = false;
    std::string device_name;
    generator::data_unit data;
    
    for(auto&& str: device_template_)
    {
        
        std::vector<std::string> tokens;
        if(!is_discarded(str))
        {
            continue;
        }
        
        split(str, tokens);

        
        if(!open_bracket && is_device(tokens.at(0), device_name))
        {
            if(device || tokens.size() > 1)
            {
                std::cerr << "failed to define device: " << str << std::endl;
                return;
            }
            
            std::cout << device_name << std::endl;
            device = true;
            continue;
        }
        

        if(is_opening(str))
        {
            open_bracket = true;
            continue;
        }
          
        
        if(is_closing(str))
        {
            if(!device || !open_bracket)
            {
                std::cerr << "invalid closing bracket" << std::endl;
                return; 
            }
            
            if(end(device_map_) != device_map_.find(device_name))
            {
                std::cerr << "device already exists: " << device_name << std::endl;
                return;
            }
            
            /* all done, clear data, reset flags */
            device_map_[device_name] = data;
            data.members_.clear();
            device       = false;
            open_bracket = false;
            device_name.clear();
            continue;
        }
               
        
        if(is_member(str))
        {
            std::tuple<std::string,std::string> member;

            if(tokens.size() != 2)
            {
                std::cerr << "failed to split data member: " << str << std::endl;
                return;
            }
            
            data.members_.push_back(std::make_tuple(tokens.at(0), tokens.at(1)));
            std::cout << tokens.at(0) << " " << tokens.at(1);
        }
        std::cout << std::endl;
    }
}


bool generator::is_discarded(const std::string& str)
{
    for(const char& c: str)
    {
        if(isalnum(c) || '{' == c || '}' == c)
        {
            return true;
        }
        
        if('#' == c)
        {
            break;
        }
    }
    return false;
}


bool generator::is_device(const std::string& str, std::string& device_name)
{
    for(const char& c : str)
    {
        if(isspace(c))
        {
            continue;
        }
        if(!isalnum(c) && '_' != c)
        {
            return false;
        }
        device_name.push_back(c);
    }
    return true;
}


bool generator::is_member(const std::string& str)
{
    for(const char& c : str)
    {
        if(!isblank(c) && !isalnum(c) && '_' != c && ';' != c && ':' != c)
        {
            return false;
        }
    }
    
    return true;
}


bool generator::is_opening(const std::string& str)
{
    bool found = false;
    for(const char& c : str)
    {
        if('{' == c)
        {
            found = true;
            continue;
        }
        
        if(!isblank(c))
        {
            return false;
        }
    }
    return found;
}


bool generator::is_closing(const std::string& str)
{
    bool found = false;
    for(const char& c : str)
    {
        if('}' == c)
        {
            found = true;
            continue;
        }
        
        if(!isblank(c))
        {
            return false;
        }
    }
    return found;
}



void generator::print_device_map()
{
    for(auto&& it : device_map_)
    {
        std::cout << it.first << std::endl;
        for(auto& i : it.second.members_)
        {
            std::cout << "\t" << std::get<0>(i) 
                      << ":"  << std::get<1>(i) << std::endl;
        }
    }
    
}

std::string new_line(int n)
{
    std::string line;
    for(int i = 0; i < n; i++)
    {
        line += "\n";
    }
    return line;
}


std::string intend(int n)
{
    std::string line;
    for(int i = 0; i < n; i++)
    {
        line += "    ";
    }
    return line;
}




void generator::dump_source()
{
    std::ofstream devices;
    devices.open("devices.cpp.bac");
    
    devices << "#include \"devices.hpp\"\n";
    devices << "#include \"serializer.hpp\"\n";
    devices << new_line(4);
    devices << "namespace tmdb\n";
    devices << "{\n";
    devices << intend(1) << "serializer* device::ins = new serializer(1024);\n";
    
    for(auto&& it : device_map_)
    {
        devices << new_line(1);
        devices << "/**************************************   "
                << it.first 
                << "   **************************************/\n";
        devices << new_line(2);
        
        devices << intend(1) << "void " << it.first << "::serialize_sync()\n";
        devices << intend(1) << "{\n";
        devices << intend(2) << "ins->sign_block(device_id_.c_str());\n";
        
        for(auto& i : it.second.members_)
        {
            if(std::string("std::string") == std::get<0>(i))
            {
                devices << intend(2) << "ins->serialize"  
                        << "(data_unit_."<< std::get<1>(i) << ");\n";
                continue;
            }
            
            devices << intend(2) << "ins->serialize<" << std::get<0>(i) 
                    << ">(data_unit_."<< std::get<1>(i) << ");\n";
        }
        devices << intend(2) << "ins->finalize_block();\n";
        devices << intend(1) << "}\n";
    }
    
    
    
    devices.close();
    
}



int main()
{
    generator gen("devices.in");
    gen.parse();
    gen.print_device_map();
    gen.dump_source();
    
    return 0;
}