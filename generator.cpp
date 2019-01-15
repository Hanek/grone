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

std::string newline(int n)
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




void generator::dump_source(const std::string& str)
{
    std::ofstream devices;
    devices.open(str);
    
    devices << "/* this file is auto-generated with devgen */\n";
    devices << "#include \"devices.hpp\"\n";
    devices << "#include \"serializer.hpp\"\n";
    devices << newline(4);
    devices << "namespace tmdb\n";
    devices << "{\n";
    devices << intend(1) << "serializer* device::ins = new serializer(1024);\n";
    
    for(auto&& it : device_map_)
    {
        devices << newline(1) << intend(1);
        devices << "/**************************************   "
                << it.first 
                << "   **************************************/\n";
        devices << newline(2);

        
        /*****************    print_data   *****************/
        
        devices << intend(1) << "void " << it.first << "::print_data(std::ostream& os)\n";
        devices << intend(1) << "{\n";
        devices << intend(1) << "os << device_id_ << \" : \"\n";
        for(auto& i : it.second.members_)
        {
            devices << intend(2) << "<< data_unit_." << std::get<1>(i) << " << \" : \"\n";
        }
        devices << intend(2) << ";\n";
        devices << intend(1) << "}\n";
        devices << intend(1) << newline(1);

    

        /*****************    serialize_sync   *****************/
        
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
        devices << newline(1);
        
        /****************   deserialize_sync   ****************/
        
        devices << intend(1) << "void " << it.first << "::deserialize_sync(void* block)\n";
        devices << intend(1) << "{\n";
        devices << intend(2) << "serializer exs(static_cast<char*>(block));\n";
        
        for(auto& i : it.second.members_)
        {
            if(std::string("std::string") == std::get<0>(i))
            {
                devices << intend(2) << "exs.deserialize"  
                        << "(data_unit_."<< std::get<1>(i) << ");\n";
                continue;
            }
            
            devices << intend(2) << "exs.deserialize<" << std::get<0>(i) 
                    << ">(&data_unit_."<< std::get<1>(i) << ");\n";
        }
        devices << intend(1) << "}\n";
        devices << newline(1);
        
        /*****************    serialize   *****************/
        
        devices << intend(1) << "void " << it.first << "::serialize(void* mem)\n";
        devices << intend(1) << "{\n";
        devices << intend(2) << "data& data_unit = *(static_cast<data*>(mem));\n";
        devices << intend(2) << "ins->sign_block(device_id_.c_str());\n";
        
        for(auto& i : it.second.members_)
        {
            if(std::string("std::string") == std::get<0>(i))
            {
                devices << intend(2) << "ins->serialize"  
                        << "(data_unit."<< std::get<1>(i) << ");\n";
                continue;
            }
            
            devices << intend(2) << "ins->serialize<" << std::get<0>(i) 
                    << ">(data_unit."<< std::get<1>(i) << ");\n";
        }
        devices << intend(2) << "ins->finalize_block();\n";
        devices << intend(1) << "}\n";
        devices << newline(1);
        
        /****************   deserialize   ****************/
        
        devices << intend(1) << "void " << it.first << "::deserialize(void* block, void* mem)\n";
        devices << intend(1) << "{\n";
        devices << intend(2) << "data& data_unit = *(static_cast<data*>(mem));\n";
        devices << intend(2) << "serializer exs(static_cast<char*>(block));\n";
        
        for(auto& i : it.second.members_)
        {
            if(std::string("std::string") == std::get<0>(i))
            {
                devices << intend(2) << "exs.deserialize"  
                        << "(data_unit."<< std::get<1>(i) << ");\n";
                continue;
            }
            
            devices << intend(2) << "exs.deserialize<" << std::get<0>(i) 
                    << ">(&data_unit."<< std::get<1>(i) << ");\n";
        }
        devices << intend(1) << "}\n";
        devices << newline(1);
        
    }
    
    devices << "}\n";
    devices.close();
}


void generator::dump_header(const std::string& str)
{
    std::ofstream devices;
    devices.open(str);
    
    devices << "/* this file is auto-generated with devgen */\n";
    devices << "#include <cstring>\n";
    devices << "#include <cstdlib>\n";
    devices << "#include <cerrno>\n";
    devices << "#include <iostream>\n";
    devices << "#include <fstream>\n";
    devices << "#include <string.h>\n";
    devices << "#include <map>\n";
    devices << newline(1);
    devices << "#ifndef _DEVICES_H\n";
    devices << "#define _DEVICES_H\n";
    devices << newline(1);
    devices << "namespace tmdb\n";
    devices << "{\n";
    devices << intend(1) << "class serializer;\n";
    devices << newline(1);
    devices << intend(1) << "class device\n";
    devices << intend(1) << "{\n";
    devices << intend(1) << "public:\n";
    devices << intend(2) << "static serializer* ins;\n";
    devices << intend(2) << "std::string device_id_;\n";
    devices << intend(2) << "device(const char* id): device_id_(id) {}\n";
    devices << intend(2) << "virtual ~device() {}\n";
    devices << newline(1);
    devices << intend(2) << "virtual void* get_data()                                = 0;\n";
    devices << intend(2) << "virtual void print_data(std::ostream& os = std::cout)   = 0;\n";
    devices << intend(2) << "virtual void serialize_sync()                           = 0;\n";
    devices << intend(2) << "virtual void deserialize_sync(void*)                    = 0;\n";
    devices << intend(2) << "virtual void serialize(void*)                           = 0;\n";
    devices << intend(2) << "virtual void deserialize(void*, void*)                  = 0;\n";
    devices << intend(1) << "};\n";

    for(auto&& it : device_map_)
    {
        devices << newline(1);
        devices << intend(1) << "/************************************   ";
        devices << it.first;
        devices << "   ************************************/\n";
        devices << newline(1);
        devices << intend(1) << "class " << it.first << ": public device\n";
        devices << intend(1) << "{\n";
        devices << intend(1) << "public:\n";
        devices << intend(2) << "struct data\n";
        devices << intend(2) << "{\n";
        
        std::string ctor = "data(): "; 
        for(auto& i : it.second.members_)
        {
            devices << intend(3) << std::get<0>(i) << " " << std::get<1>(i) << ";\n";
            if(std::string("std::string") != std::get<0>(i))
            {
                ctor += std::get<1>(i);
                ctor += "(0), ";
            }
        }
        ctor.pop_back();
        ctor.pop_back();
        ctor += " {}\n";
        devices << intend(3) << ctor;
        devices << intend(2) << "};";
        devices << newline(1);
        devices << intend(2) << "data data_unit_;\n";
        devices << intend(2) << it.first << "(const char* id): device(id) {}\n";
        devices << intend(2) << it.first << "(const char* id, void* mem): device(id) { if(mem) {} }\n";
        devices << newline(1);
        devices << intend(2) << "void* get_data() { return &data_unit_; }\n";
        devices << intend(2) << "void print_data(std::ostream& os = std::cout);\n";
        devices << intend(2) << "void serialize_sync();\n";
        devices << intend(2) << "void deserialize_sync(void*);\n";
        devices << intend(2) << "void serialize(void*);\n";
        devices << intend(2) << "void deserialize(void*, void*);\n";
        devices << intend(1) << "};\n";
    }

    devices << newline(1);
    devices << intend(1) << "/************************************   ";
    devices << "factory";
    devices << "   ************************************/\n";
    devices << newline(1);
    devices << intend(1) << "class device_factory\n";
    devices << intend(1) << "{\n";
    devices << intend(1) << "public:\n";
    devices << intend(2) << "/* create empty device, supply data later.. */\n";
    devices << intend(2) << "device* create(const char* device_id)\n";
    devices << intend(2) << "{\n";
    devices << intend(3) << "std::map<std::string,pCreate>::iterator it;\n";
    devices << intend(3) << "it = mapCreate_.find(std::string(device_id));\n";
    devices << intend(3) << "if(it != mapCreate_.end())\n";
    devices << intend(3) << "{ return it->second(device_id); }\n";
    devices << intend(3) << "return 0;\n";
    devices << intend(2) << "}\n";
    devices << newline(1);
    devices << intend(2) << "/* create device with data */\n";
    devices << intend(2) << "device* create_sync(const char* device_id, void* data)\n";
    devices << intend(2) << "{\n";
    devices << intend(3) << "std::map<std::string,pCreate_sync>::iterator it;\n";
    devices << intend(3) << "it = mapCreate_sync_.find(std::string(device_id));\n";
    devices << intend(3) << "if(it != mapCreate_sync_.end())\n";
    devices << intend(3) << "{ return it->second(device_id, data); }\n";
    devices << intend(3) << "return 0;\n";
    devices << intend(2) << "}\n";
    devices << newline(1);
    devices << intend(2) << "template <typename T>\n";
    devices << intend(3) << "void register_device(const char* device_id)\n";
    devices << intend(3) << "{ mapCreate_[device_id] = &instantiate<T>; }\n";
    devices << newline(1);
    devices << intend(2) << "template <typename T>\n";
    devices << intend(3) << "void register_device_sync(const char* device_id)\n";
    devices << intend(3) << "{ mapCreate_sync_[device_id] = &instantiate_sync<T>; }\n";
    devices << newline(2);
    devices << intend(1) << "private:\n";
    devices << intend(2) << "template <typename T>\n";
    devices << intend(3) << "static device* instantiate(const char* id)\n";
    devices << intend(3) << "{ return new T(id); }\n";
    devices << newline(1);
    devices << intend(2) << "template <typename T>\n";
    devices << intend(3) << "static device* instantiate_sync(const char* id,void* val)\n";
    devices << intend(3) << "{ return new T(id, val); }\n";
    devices << newline(2);
    devices << intend(2) << "typedef device* (*pCreate)(const char*);\n";
    devices << intend(2) << "std::map<std::string,pCreate> mapCreate_;\n";
    devices << intend(2) << "typedef device* (*pCreate_sync)(const char*, void*);\n";
    devices << intend(2) << "std::map<std::string,pCreate_sync> mapCreate_sync_;\n";
    devices << intend(1) << "};\n";
    devices << "}\n";
    devices << "#endif\n";
}




int main()
{
    generator gen("devices.in");
    gen.parse();
    gen.print_device_map();
    gen.dump_header("devices.hpp");
    gen.dump_source("devices.cpp");
    
    return 0;
}
