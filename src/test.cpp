#include <clocale>
#include <pthread.h>
#include <typeinfo>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <vector>
#include <glib.h>

#include "db_core.hpp"
#include "serializer.hpp"
#include "devices.hpp"
#include "socket.hpp"
#include "protocol.hpp"
#include "server.hpp"

void linear_serializer_test()
{ 
  int n = 10;
  char p[grone::serializer::dev_id_max()] = {0};
  char str[grone::serializer::dev_id_max()];
  float f = 3.141592;
      
  grone::serializer is(4), os(4);
  
  for(int i = 0; i < n; i++)
  {
    sprintf(p, "variable length header: #%d",i);
    for(int j = 0; j < i; j++) { strcat(p, "__"); }
    sprintf(str, "cstring: #%d", i<<2);
    for(int j = 0; j < i; j++) { strcat(str, "__"); }
    is.sign_block(p);
    is.serialize<int>(grone::serializer::dev_id_max() + i);
    is.serialize<char>('a' + i);
    is.serialize(str);
    is.serialize<char>('A' + i);
    is.serialize<float>(f + i);
    is.finalize_block();
  }
  
  gchar* strEncoded = g_base64_encode((const guchar*)is.get_buffer(), is.length());
  is.reset();

//   std::cout << "input buffer length: " << is.length() << std::endl;  
  
  is.dump();
  
  gsize lenDecoded;
  guchar* strDecoded = g_base64_decode(strEncoded, &lenDecoded);
  
  os.update_buffer(strDecoded, lenDecoded);  
  g_free(strEncoded);
  g_free(strDecoded);
  
  int ii = 0;
  char cc = 0;
  char c2 = 0;
  char pp[64];
  float ff = 0;
  
  memset(pp, 0x00, sizeof(pp));
  memset(p, 0x00, sizeof(p));
  
  for(int i = 0; i < n; i++)
  {
    os.read_block(p);
    os.deserialize<int>(&ii);
    os.deserialize<char>(&cc);
    os.deserialize(pp);
    os.deserialize<char>(&c2);
    os.deserialize<float>(&ff);
    
    std::cout << "id: " << p << std::endl;
    std::cout << ii << std::endl;
    std::cout << cc << std::endl;
    std::cout << pp << std::endl;
    std::cout << c2 << std::endl;
    std::cout << ff << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    memset(pp, 0x00, sizeof(pp));
  }
}



void linear_std_stream_test(int n)
{ 
  char h[16];
  strcpy(h, "h4bt");
  char c = 'a';
  char p[64];
  strcpy(p, "test_string");
  float f = 3.141592;

  
  std::stringstream os;
   
  for(int i = 0; i < n; i++)
  {
    os << h << " "
       << 0xffff + i << " " 
       << (char)(c + i) << " "
       << p << " "
       << f << " ";
       
//        std::cout << h << "\t";
//        std::cout << i + 0xffff << "\t";
//        std::cout << (char)(c + i) << "\t";
//        std::cout << p << "\t";
//        std::cout << f << std::endl;
  }
  
  
  std::cout << "std stream length: " << os.str().length() << std::endl;
  
 
  std::string hh;
  int ii = 0;
  char cc = 0;
  std::string pp;
  float ff = 0;
  
  
  for(int i = 0; i < n; i ++)
  {
    pp.clear();
    os >> hh
       >> ii 
       >> cc
       >> pp 
       >> ff;
       
//        std::cout << hh << "\t";
//        std::cout << ii << "\t";
//        std::cout << cc << "\t";
//        std::cout << pp << "\t";
//        std::cout << ff << std::endl;
  } 
}


void test_std_queue()
{
  int lim = 1000;
  std::queue<std::string> qu;
  
  std::string p = "test string";
  for(int n = 0; n < lim; n++)
  {
    p[3] = 0x30 + n;
    qu.push(p);
  }
 
    
  for(int n = 0; n < lim; n++)
  {
    std::string pp;
    pp = qu.front();
    qu.pop();
//     std::cout << "node: " << pp << std::endl;
  }
}


int counter = 0;

void fill_device1(grone::test_device1::data& d1_unit)
{
  char buf[1024] = {0};
  d1_unit.x_ = counter;
  d1_unit.y_ = counter + 1;
  d1_unit.sp_ = ((double)counter)/10000;
  strcpy(buf, "description variable length:");
  memset(buf + strlen(buf), '_', counter);
  d1_unit.descr_ = std::string(buf);
  d1_unit.mode_ = counter;
}

void fill_device2(grone::test_device2::data& d2_unit)
{
  char buf[1024] = {0};
  d2_unit.x_ = counter;
  d2_unit.temp_ = 3.14 + ((double)counter)/10000;
  strcpy(buf, "some_description");
  memset(buf + strlen(buf), '_', counter);
  d2_unit.descr_ = std::string(buf);
}



void fill_dev_test(grone::core& c)
{
  grone::test_device1::data d1_unit;
  grone::test_device2::data d2_unit;

  grone::device* pDev = 0;

  for(int i = 0; i < 3; i++)
  {
    pDev = c.factory_->create("device1");
    /* serialize when data is availble, no copying here */
    fill_device1(d1_unit);
    pDev->serialize(reinterpret_cast<unsigned char*>(&d1_unit));
    counter++;
    delete pDev;
  }
  c.cacheIt();
  
  for(int i = 0; i < 3; i++)
  {
    pDev = c.factory_->create("device2");
    /* serialize when data is availble, no copying here */
    fill_device2(d2_unit);
    pDev->serialize(reinterpret_cast<unsigned char*>(&d2_unit));
    counter++;
    delete pDev;
  }

  c.cacheIt();

  for(int i = 0; i < 3; i++)
  {
    pDev = c.factory_->create("device1");
    /* serialize when data is availble, no copying here */
    fill_device1(d1_unit);
    pDev->serialize(reinterpret_cast<unsigned char*>(&d1_unit));
    counter++;
    delete pDev;
  }

//  c.cacheIt();

  for(int i = 0; i < 3; i++)
  {
    pDev = c.factory_->create("device2");
    /* serialize when data is available, no copying here */
    fill_device2(d2_unit);
    pDev->serialize(reinterpret_cast<unsigned char*>(&d2_unit));
    counter++;
    delete pDev;
  }
}
  
  
  
  

/* 
 * TODOs
 * - api
 * - time to std::time
 * - device_id length must be limited somehow, serializer read_block and get_block must explicitly know max size  in runtime
 */
  
int main()
{
  plog::init(plog::error, "grone.log");
  LOG_NONE << "---------------------- grone start ----------------------";
  
  
//  get_block_test();
//  test_device3();
  
  
  //   linear_serializer_test();
  //   linear_std_stream_test(4000000);  
  // grone_test();
  // devices_test1();
//  core_factory_test();

  // get_block_test();
  
  
//  tcp_server_test();
  
  grone::core c;
  c.init();
  fill_dev_test(c);
  c.dm_walk();
  
  grone::server server(c, 2, 3);
  server.run();
  

  return 0;
}
