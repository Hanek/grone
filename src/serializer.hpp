#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <map>
#include <vector>
#include <typeinfo>
#include <fstream>
#include <plog/Log.h>

#ifndef _SERIALIZER_H
#define _SERIALIZER_H  


namespace grone
{
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
    int             message_len_;

    /* internal buffer */
    unsigned char*  buf_;
    unsigned char*  buf_copy_;

    /* current position */
    unsigned char*  pos_;
    /* begininng of the current block */
    unsigned char*  beg_;
    /* current block length, calculated on finalize() */
    int block_len_;

    /* size of the buffer */
    size_t size_;

    /* length of dev_id header, calculated as (dev_id) */
    int hlen_;

    /* max length of dev_id header */
    static int dev_id_max_;

    /* set when constructed with external buffer */
    bool external_;

    bool reallocate();


  
  public:
    /* two basic modes offered:
     * external - when user holds responsibility for suppling and maintaining of external memory
     * internal - when buffer is allocated and managed by serializer */
    
    /* internal mode */
    serializer(size_t size);
    /* external mode */
    serializer(unsigned char* buf);
    /* external mode */
    serializer(unsigned char* buf, int len);
    ~serializer() { if (!external_) { delete[] buf_; } }

    bool   empty()    const { return (size_) ? true : false; }
    size_t get_size() const { return size_; }
    size_t get_hlen() const { return hlen_; }
    size_t length()   const { return message_len_; }
    unsigned char* get_buffer() const { return buf_; }
    
    static int dev_id_max() { return dev_id_max_; }

    /* called prior to device serialization */ 
    void sign_block(const char* id);
    /* called when device serialization is done */
    void finalize_block();

    

  /* copy data to serializer */
  void update_buffer(unsigned char* bufin, size_t sizein);
  
  /* 
   * paired, used by cacheIt exclusivly 
   * - realloc to fit data to message_len_, returns pointer to old buffer
   * - detaching allows buffer reuse and avoid extra allocs
   */
  void shrink_to_fit();
  unsigned char* detach_buffer();
  
  /* allocate memory and copy */
  unsigned char* copy_buffer();
  void clear();
  void reset();
  /* 
   * read block into id and set pointer to data,
   * must be followed by deserialize() methods
   */
  bool read_block(char* id);
  /* iterate over dev_id */
  unsigned char* get_block(char* id);
  
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
      { reallocate(); }
      
      memcpy(pos_, "NULL", nlen); 
      pos_ += nlen; 
      *pos_ = 0x00; 
      pos_ += 1;
      return;
    }
    
    if(strlen(str) >= size_ - (pos_ - buf_) - 1)
    { reallocate(); }
    
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
    { reallocate(); }
    
    memcpy(pos_, &var, sizeof(T)); 
    pos_ += sizeof(T);
  }
  
  /******************** deserialization methods ********************/
  
  void deserialize(char* str)
  {
    memcpy((void*)str, reinterpret_cast<const char*>(pos_), strlen(reinterpret_cast<const char*>(pos_))); 
    pos_ += strlen(reinterpret_cast<const char*>(pos_)) + 1;
  }
  
  void deserialize(std::string& str)
  {
    str = std::string(reinterpret_cast<const char*>(pos_));
    pos_ += strlen(reinterpret_cast<const char*>(pos_)) + 1;
  }
  
  template <class T> void deserialize(T* var)
  {
    memcpy((void*)var, pos_, sizeof(T)); 
    pos_ += sizeof(T);
  }
  
 /**** get member size, same as deserialize but making no copies ****/
  
  size_t size()
  {
    size_t size = strlen(reinterpret_cast<const char*>(pos_)) + 1;
    pos_ += size;
    return size;
  }
  
  template <class T> size_t size()
  { 
    pos_ += sizeof(T);
    return sizeof(T);
  }
  
  /******************************************************************/
  
  void dump()
  {
    std::ofstream file("buf.bin", std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char*>(buf_), size_);
    file.close();
  }
};

  
}
#endif

