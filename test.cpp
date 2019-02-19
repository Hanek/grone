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

void linear_serializer_test()
{ 
  int n = 10;
  char p[tmdb::serializer::dev_id_max()] = {0};
  char str[tmdb::serializer::dev_id_max()];
  float f = 3.141592;
      
  tmdb::serializer is(4), os(4);
  
  for(int i = 0; i < n; i++)
  {
    sprintf(p, "variable length header: #%d",i);
    for(int j = 0; j < i; j++) { strcat(p, "__"); }
    sprintf(str, "cstring: #%d", i<<2);
    for(int j = 0; j < i; j++) { strcat(str, "__"); }
    is.sign_block(p);
    is.serialize<int>(tmdb::serializer::dev_id_max() + i);
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
// 100000000
// 132000000 serializer
// 139031070 std_stream  



/* used by all devices, no concurrency allowed */
tmdb::serializer srl(16);


/*
 * "serial_device"
 * int x
 * int y
 * float m
 * cstring type
 */
void serialize_1(std::string& dev_id, int x, int y, float m, std::string& type) 
{
  srl.sign_block(dev_id.c_str());
  srl.serialize<int>(x);
  srl.serialize<int>(y);
  srl.serialize<float>(m);
  srl.serialize(type.c_str());
  srl.finalize_block();
}

void deserialize_1(char* p, int& x, int& y, float& m, std::string& type) 
{
  char buf[1024] = {0};
  tmdb::serializer s(p);
  s.deserialize<int>(&x);
  s.deserialize<int>(&y);
  s.deserialize<float>(&m);
  s.deserialize(buf);
  type = std::string(buf);
}

/*
 * "custom device"
 * cstring name
 * double u1
 * double u2
 * double u3
 */
 
void tmdb_test()
{
  tmdb::core c;
  tmdb::serializer is(16);
  
  is.sign_block("first device");
  is.serialize<int>(4096);
  is.serialize<char>('a');
  is.serialize("some cstring here");
  is.serialize<float>(3.141592);
  is.finalize_block();
  
  is.sign_block("second device variable length");
  is.serialize<int>(1024);
  is.serialize<char>('z');
  is.serialize("...");
  is.serialize<float>(666.666);
  is.finalize_block();
  
  is.sign_block("first device");
  is.serialize<int>(8096);
  is.serialize<char>('x');
  is.serialize("some cstring here");
  is.serialize<float>(3.141592);
  is.finalize_block();
  
  
  
  c.cacheIt();
  
  is.sign_block("first device");
  is.serialize<int>(40962);
  is.serialize<char>('b');
  is.serialize("some cstring here");
  is.serialize<float>(3.141592);
  is.finalize_block();
  
  is.sign_block("second device variable length");
  is.serialize<int>(10242);
  is.serialize<char>('y');
  is.serialize("...");
  is.serialize<float>(666.666);
  is.finalize_block();
  
  is.sign_block("first device");
  is.serialize<int>(80962);
  is.serialize<char>('w');
  is.serialize("some cstring here");
  is.serialize<float>(3.141592);
  is.finalize_block();
  
  
  c.cacheIt();
  
  c.bm_walk();  
  //c.dm_walk();
}

int counter = 0;

void fill_device1(tmdb::test_device1::data& d1_unit)
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

void fill_device2(tmdb::test_device2::data& d2_unit)
{
  char buf[1024] = {0};
  d2_unit.x_ = counter;
  d2_unit.temp_ = 3.14 + ((double)counter)/10000;
  strcpy(buf, "some_description");
  memset(buf + strlen(buf), '_', counter);
  d2_unit.descr_ = std::string(buf);
}

void devices_test1()
{
  tmdb::core c;
  tmdb::test_device1::data d1_unit;
  tmdb::test_device2::data d2_unit;
 
  tmdb::device* pDev = 0; 
  tmdb::device_factory f;
  f.register_device<tmdb::test_device1>("device1");
  f.register_device<tmdb::test_device2>("device2");

  for(int i = 0; i < 3; i++)
  {
    pDev = f.create("device1");
    /* serialize when data is availble, no copying here */
    fill_device1(d1_unit);
    pDev->serialize(&d1_unit);
    counter++;
    delete pDev;
  }

  c.cacheIt();
  
  
  for(int i = 0; i < 3; i++)
  {
    pDev = f.create("device2");
    /* serialize when data is availble, no copying here */
    fill_device2(d2_unit);
    pDev->serialize(&d2_unit);
    counter++;
    delete pDev;
  }

  c.cacheIt();
  
  for(int i = 0; i < 3; i++)
  {
    pDev = f.create("device1");
    /* serialize when data is availble, no copying here */
    fill_device1(d1_unit);
    pDev->serialize(&d1_unit);
    counter++;
    delete pDev;
  }

  c.cacheIt();
  
  //  c.bm_walk();  
  //c.dm_walk(f);
}



void devices_test2()
{
  tmdb::core c;
  tmdb::test_device1::data d1_unit;
  tmdb::test_device2::data d2_unit;
  tmdb::test_device1 td1();
  
  tmdb::device_factory f2;
  f2.register_device_sync<tmdb::test_device1>("device1");
  f2.register_device_sync<tmdb::test_device2>("device2");

  tmdb::device* pDev = f2.create_sync("device1", &d1_unit);
  pDev->serialize_sync();
  delete pDev;

  for(int i = 0; i < 2; i++)
  {
    d2_unit.x_ = i;
    d2_unit.temp_ = i/10000;
    d2_unit.descr_ = std::string("device2 description");
    pDev = f2.create_sync("device2", &d2_unit);
    /* serialize can be done at any time, device owns the data */
    pDev->serialize_sync();
    delete pDev;
  }

  c.cacheIt();

  c.bm_walk();  
  //c.dm_walk(f);

}

void core_factory_test()
{
  tmdb::core c;
  tmdb::test_device1::data d1_unit;
  tmdb::test_device2::data d2_unit;

  tmdb::device* pDev = 0;

  for(int i = 0; i < 3; i++)
  {
    pDev = c.factory_->create("device1");
    /* serialize when data is availble, no copying here */
    fill_device1(d1_unit);
    pDev->serialize(&d1_unit);
    counter++;
    delete pDev;
  }
  c.cacheIt();
  
  for(int i = 0; i < 3; i++)
  {
    pDev = c.factory_->create("device2");
    /* serialize when data is availble, no copying here */
    fill_device2(d2_unit);
    pDev->serialize(&d2_unit);
    counter++;
    delete pDev;
  }

  c.cacheIt();

  for(int i = 0; i < 3; i++)
  {
    pDev = c.factory_->create("device1");
    /* serialize when data is availble, no copying here */
    fill_device1(d1_unit);
    pDev->serialize(&d1_unit);
    counter++;
    delete pDev;
  }

//  c.cacheIt();

  for(int i = 0; i < 3; i++)
  {
    pDev = c.factory_->create("device2");
    /* serialize when data is available, no copying here */
    fill_device2(d2_unit);
    pDev->serialize(&d2_unit);
    counter++;
    delete pDev;
  }

  c.cacheIt();
  std::string resp1;
  c.get("device1", resp1);
  std::string resp2;
  c.get("device2", resp2);
  std::cout << "==== get device1 ====\n";
  std::cout << resp1 << std::endl;
  std::cout << "=====================\n";
  std::cout << "==== get device2 ====\n";
  std::cout << resp2 << std::endl;
  std::cout << "=====================\n";

  //c.bm_walk();  
  //c.dm_walk();
  //c.fetch_device_data(std::string("device1"));
  c.dm_walk();
  c.uncacheIt();
  c.dm_walk();
}


  void get_block_test()
{
  tmdb::core c;
  tmdb::test_device2::data d2_unit;

  tmdb::device* pDev = 0;

  pDev = c.factory_->create("device2");
  /* serialize when data is availble, no copying here */
  //fill_device2(d2_unit);
  pDev->serialize(&d2_unit);
  delete pDev;

  tmdb::device::ins->dump();
  c.cacheIt();

  c.uncacheIt();

}
  
  void test_device3()
{
  tmdb::core c;
  c.factory_->register_device<tmdb::test_device3>("device3");
  tmdb::test_device3::data d3_unit;

  tmdb::device* pDev = 0;

  pDev = c.factory_->create("device3");
  d3_unit.descr_ = std::string("device3 description");
  /* serialize when data is availble, no copying here */
  pDev->serialize(&d3_unit);
  delete pDev;

  c.cacheIt();
  c.uncacheIt();
}
  
  
  void tcp_server_test()
  {
    tmdb::listener listener(8080);
    while(true)
    {
        tmdb::provider socket  = listener.accept();
        tmdb::protocol server(socket);
        
        std::string message;
        server.recvMessage(message);
        std::cout << message << "\n";
        server.sendMessage("", "OK");
    }
      
  }


/* 
 * TODOs
 * - api
 * - performance testing
 * - device_id length must be limited somehow, serializer read_block and get_block must explicitly know max size  in runtime
 */
  
int main()
{
  plog::init(plog::error, "tmdb.log");
  LOG_NONE << "---------------------- tmdb start ----------------------";
  
  
//  get_block_test();
//  test_device3();
  
  
  //   linear_serializer_test();
  //   linear_std_stream_test(4000000);  
  // tmdb_test();
  // devices_test1();
//  core_factory_test();

  // get_block_test();
  
  
  tcp_server_test();
  

  return 0;
}
