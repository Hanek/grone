#include <iostream>
#include <sstream>
#include <iterator>
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
    
    for(auto&& str: device_template_)
    {
        std::string device_name;
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
            /* all done, reset flags */
            device       = false;
            open_bracket = false;
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
            std::make_tuple(tokens.at(0), tokens.at(1));
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
    
}



int main()
{
    generator gen("devices.in");
    gen.parse();
    
    return 0;
}