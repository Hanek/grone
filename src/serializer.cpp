#include "serializer.hpp"
#include  <plog/Log.h>

namespace grone
{
  int serializer::dev_id_max_ = 4096;

  /***********************************************/
  /*                 public scope                */
  /***********************************************/

  serializer::serializer(size_t size): size_(size), hlen_(0), external_(false)
  {
    buf_  = new(std::nothrow)unsigned char[size_];
    if (!buf_)
    {
      LOG_FATAL << "buffer allocation failed: " << strerror(errno);
      return;
    }
    memset(buf_, 0x00, size_);
    message_len_ = 0;
    pos_         = buf_ + hlen_;
    beg_         = buf_;
    block_len_   = 0;
  }


  serializer::serializer(unsigned char* buf): size_(32), hlen_(0), external_(true)
  {
    buf_  = buf;
    message_len_ = 0;
    pos_         = buf_ + hlen_;
    beg_         = buf_;
    block_len_   = 0;
  }


  serializer::serializer(unsigned char* buf, int len): size_(len), hlen_(0), external_(true)
  {
    buf_  = buf;
    message_len_ = len;
    pos_         = buf_ + hlen_;
    beg_         = buf_;
    block_len_   = 0;
  }


  void serializer::serialize(const std::string& str) 
  {
      serialize(str.c_str());
  }
  

  void serializer::serialize(const char* str)
  {
    /* invalid/empty cstring coded as "NULL" */
    if (!str)
    {
      size_t  nlen = strlen("NULL");
      while (nlen >= size_ - (pos_ - buf_) - 1)
      { reallocate(); }

      memcpy(pos_, "NULL", nlen);
      pos_ += nlen;
      *pos_ = 0x00;
      pos_ += 1;
      return;
    }

    while (strlen(str) >= size_ - (pos_ - buf_) - 1)
    { reallocate(); }

    memcpy(pos_, str, strlen(str));
    pos_ += strlen(str);
    *pos_ = 0x00;
    pos_ += 1;
  }


  void serializer::deserialize(char* str)
  {
    memcpy((void*)str, reinterpret_cast<const char*>(pos_), strlen(reinterpret_cast<const char*>(pos_)));
    pos_ += strlen(reinterpret_cast<const char*>(pos_)) + 1;
  }


  void serializer::deserialize(std::string & str)
  {
    str = std::string(reinterpret_cast<const char*>(pos_));
    pos_ += strlen(reinterpret_cast<const char*>(pos_)) + 1;
  }

  void serializer::dump()
  {
    std::ofstream file("buf.bin", std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char*>(buf_), size_);
    file.close();
  }

  /***********************************************/
  /*                 private scope               */
  /***********************************************/

  bool serializer::reallocate()
  {
    /* realloc preserving old data */
    size_ *= 2;
    size_t shift1 = pos_ - buf_;
    size_t shift2 = beg_ - buf_;
    unsigned char* buf_new;
    buf_new = new(std::nothrow)unsigned char[size_];
    if (!buf_new)
    {
      LOG_FATAL << "buffer allocation failed: " << strerror(errno);
      return false;
    }
    memset(buf_new, 0x00, size_);
    memcpy(buf_new, buf_, size_ / 2);
    delete[] buf_;
    buf_ = buf_new;
    pos_ = buf_ + shift1;
    beg_ = buf_ + shift2;

    LOG_DEBUG << "realloc to " << size_ << " bytes";
    return true;
  }
 


void serializer::update_buffer(unsigned char* bufin, size_t sizein)
  {
    while(size_ <= sizein)
    { reallocate(); }
    memcpy(buf_, bufin, sizein);
    message_len_ = (int)sizein;
  }


/* paired with detach_buffer, allocation */
void serializer::shrink_to_fit()
{
  unsigned char* mem = new(std::nothrow)unsigned char[size_];
  if(!mem)
  {
    LOG_FATAL << "buffer allocation failed: " << strerror(errno);
    return;
  }

  memcpy(mem, buf_, size_);
  buf_copy_ = buf_;
  buf_ = mem;
}

/* return internal buffer, take a copy made with shrink_to_fit, no allocation */
unsigned char* serializer::detach_buffer()
 {
    unsigned char* mem = buf_;
    buf_ = buf_copy_;
    buf_copy_ = NULL;
    clear();
    
    return mem;
  }
 

/* copy buffer of message_len_ into new memory chunk, allocation */
unsigned char* serializer::copy_buffer()
{
  unsigned char* mem = new(std::nothrow)unsigned char[message_len_];
  if(!mem)
  {
    LOG_FATAL << "buffer allocation failed: " << strerror(errno);
    return NULL;
  }
  memcpy(mem, buf_, message_len_);

  return mem;
}

void serializer::clear()
  {
    reset();
    memset(buf_, 0x00, size_); 
    message_len_ = 0;
  }
  
  void serializer::reset()
  {
    hlen_      = 0;
    pos_       = buf_ + hlen_; 
    beg_       = buf_;
    block_len_ = 0;
  }
  
  /* called prior to device serialization, no allocation */ 
  void serializer::sign_block(const char* id)
  {
    if(!id)
    {
      hlen_ = 1;
      *beg_ = 0x00;
      while(pos_ - buf_ + hlen_ + sizeof(hlen_) >= size_)
      { reallocate(); }
    }
    else
    {
      hlen_ = strlen(id) + 1;
      while(pos_ - buf_ + hlen_ + sizeof(hlen_) >= size_)
      { reallocate(); }
      memcpy(beg_, id, hlen_);
      *(beg_ + hlen_) = 0x00;
    }
    
    pos_ += hlen_ + sizeof(hlen_);
  }
  
  /* called when device serialization is done, no allocation */
  void serializer::finalize_block()
  {
    block_len_ = (pos_ - beg_) - hlen_ - sizeof(block_len_);
    message_len_ += (pos_ - beg_);
    memcpy(beg_ + hlen_, &block_len_, sizeof(block_len_));
    beg_ = pos_;
  }
  
  
  
  /* 
   * read block into id and set pointer to data,
   * must be followed by deserialize() methods, no allocation
   */
  bool serializer::read_block(char* id)
  {    
    strcpy(id, reinterpret_cast<const char*>(pos_));
    hlen_ = strlen(id);
    
    while(pos_ - buf_ + hlen_ + sizeof(hlen_) >= size_)
    { reallocate(); }
    
    if(message_len_ <= pos_ - buf_)
    { return false; }
        
    pos_ = beg_ + hlen_ + sizeof(hlen_) + 1;
    memcpy(&block_len_, beg_ + hlen_ + 1, sizeof(block_len_));
    beg_ += block_len_ + hlen_ + sizeof(block_len_) + 1; 

    return true;
  }
  
  /* iterate over dev_id, no allocation */
  unsigned char* serializer::get_block(char* id)
  {
    /* don't know dev_id length in runtime */
    strcpy(id, reinterpret_cast<const char*>(pos_));
    hlen_ = strlen(id);

    while(pos_ - buf_ + hlen_ + sizeof(hlen_) >= size_)
    { /* TODO unit test */
      if(!external_) 
      { reallocate(); } 
      else
      { return NULL; }
    }
    
    
    if(message_len_ <= pos_ - buf_)
    { return NULL; }
     
    /* advance current position */
    memcpy(&block_len_, beg_ + hlen_ + 1, sizeof(block_len_));
    pos_ = beg_ + hlen_ + sizeof(hlen_) + 1 + block_len_;
    beg_ += block_len_ + hlen_ + sizeof(block_len_) + 1; 
    
    /* return pointer to data behind current dev_id */
    return pos_ - block_len_;
  }

}
