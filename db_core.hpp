#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <map>

#ifndef _DB_CORE_H
#define _DB_CORE_H

/* 
 * tmdb used to store time series as blobs nearly equal size
 * time series defined as pod data with id
 * 
 * each blob name holds index and time bounds of data
 * local cache keeps "rt" data of most recent time series,
 * partially dumping outdated branch as a new blob.
 * 
 * -------------------------------------------------------------------------
 * |       | id1 | len | data | id2 | len | data |       | id | len | data |
 * | time1 |------------------|------------------| time2 |-----------------|
 * |       |                block1               |       |      block2     |
 * |------------------------------------------------------------------------
 * |                                blob0001                               |
 * -------------------------------------------------------------------------
 *  
 * 
 * - cache is a key-value in-memory storage, 
 *   whith time as keys and block as values 
 * 
 * - id is user-generated metric linked to data  
 * - len is data length in bytes
 * 
 * 
 * const char id1 = "device1";
 * int  a     = 5;
 * int  b     = 2324;
 * char c     = 'a';
 * double f   = 3.1415926535;
 * unsigned u = 0xff;
 * 
 * - serialize
 * - generate/read time 
 * - call cacheIt() to submit data to core 
 *
 * 
 */




namespace tmdb
{
  class time;
  class serializer;
  
  
  class core 
  {
  public:
    core();
    ~core();
    static void cacheIt(serializer& s);
  private:
    static bool instance_;
    static std::map<time,void*> bmap_;
    static std::map<std::string,std::map<time,void*> > dmap_;
  };
  
}





#endif


