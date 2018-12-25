/*
 * pod serializer requirements:
 * - named blocks with pod types defined by user;
 * - varibale header length 
 */ 

// g++ -Wall -Wextra `pkg-config --cflags glib-2.0` test.cpp -o test `pkg-config --libs glib-2.0`

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <map>
#include <vector>
#include <typeinfo>
#include <fstream>
#include  <plog/Log.h>

#ifndef _SERIALIZER_H
#define _SERIALIZER_H  


namespace tmdb
{

/*
 * - no easy way to handle multiple devices
 * - each would require callbacks
 * - what if data sources change
 * -------------------------------------------------------------
 * - totaly idependent serialization that satisfy tlv stadanard 
 *   could be a possible solution
 * 
 */ 
class serializer
{
/*  
 *                        block structure
 * 
 *    |_|_|_|_|0|_|_|_|_|_|_|_|_|_|_|_|_|_|_|0|_|_|_|_|_|_|_|_|
 *    | dev_id  |  len  |       |             |               |
 *    | cstring | size_t|  int  |   cstring   |     float     |
 *    |s i g n a t u r e|          b l o c k   b o d y        |
 *
 *    serializer used to accumulate pod data as blocks
 *    each block starts with sign_block() with dev_id of variable length, 
 *    ends up with finalize()   
 */

private:
  /* whole message length */
  int    message_len_;
  /* dynamically allocated buffer */
  char*  buf_;
  char*  buf_copy_;

  /* current position */
  char*  pos_;
  /* begininng of current block */
  char*  beg_;
  /* current block length, calculated on finalize() */
  int block_len_;
    
  /* size of buffer */
  size_t size_;

  /* length of dev_id header, calculated as (dev_id) */
  int hlen_;
  
  /* max length of dev_id header */  
  static int dev_id_max_;

  /* on if constructed with external buffer */
  bool external_;  

  std::map<std::string, std::vector<std::string> > device_map_;

  bool out_of_mem();
   
public:
  serializer(size_t size);  
  serializer(char* buf);
  serializer(char* buf, int len);
  ~serializer() { if(!external_){ delete[] buf_; } }
  bool empty() { return (0 == size_) ? 1 : 0; }
  size_t get_size() { return size_; }
  size_t length() { return message_len_; }
  size_t get_hlen() { return hlen_; }
  void* get_buffer() { return buf_; }
  static int dev_id_max() { return dev_id_max_; }
  
  /* copy data to serializer */
  void update_buffer(void* bufin, size_t sizein);
  
  /* 
   * paired, used by cacheIt exclusivly 
   * - realloc to fit data to message_len_, returns pointer to old buffer
   * - detaching allows buffer reuse and avoid extra allocs
   */
  void shrink_to_fit();
  void* detach_buffer();
  
  /* allocate memory and copy */
  void* copy_buffer();
  void clear();
  void reset();
  /* called prior to device serialization */ 
  void sign_block(const char* id);
  /* called when device serialization is done */
  void finalize_block();
  /* 
   * read block into id and set pointer to data,
   * must be followed by deserialize() methods
   */
  bool read_block(char* id);
  /* iterate over dev_id */
  void* get_block(char* id);
  
  /******************** serialization methods ********************/
  
  void serialize(const std::string& str) 
  {
      serialize(str.c_str());
  }
  
  void serialize(const char* str) 
  {
   /* empty cstring coded as "NULL" */  
    if(!str) 
    {
      size_t  nlen = strlen("NULL");
      if(nlen >= size_ - (pos_ - buf_) - 1)
      { out_of_mem(); }
      
      memcpy(pos_, "NULL", nlen); 
      pos_ += nlen; 
      *pos_ = 0x00; 
      pos_ += 1;
      return;
    }
    
    if(strlen(str) >= size_ - (pos_ - buf_) - 1)
    { out_of_mem(); }
    
    memcpy(pos_, str, strlen(str)); 
    pos_ += strlen(str); 
    *pos_ = 0x00; 
    pos_ += 1;
  }
  
  
  template <class T> void serialize(T var) 
  {
    if(!std::is_trivial<T>::value)
    { 
      LOG_NONE << "skipping non_trivial type: " << typeid(T).name();
      return;
    }
    
    if(sizeof(T) >= size_ - (pos_ - buf_) - 1)
    { out_of_mem(); }
    
    memcpy(pos_, &var, sizeof(T)); 
    pos_ += sizeof(T);
  }
  
  /******************** deserialization methods ********************/
  
  void deserialize(char* str)
  {
    memcpy((void*)str, pos_, strlen(pos_)); 
    pos_ += strlen(pos_) + 1;
  }
  
  void deserialize(std::string& str)
  {
    str = std::string(pos_);
    pos_ += strlen(pos_) + 1;
  }
  
  template <class T> void deserialize(T* var)
  {
    memcpy((void*)var, pos_, sizeof(T)); 
    pos_ += sizeof(T);
  }
  
  /******************************************************************/
  
  void dump()
  {
    std::ofstream file("buf.bin", std::ios::out | std::ios::binary);
    file.write(buf_, size_);
    file.close();
  }
};

  
}
#endif

