#include "serializer.hpp"

namespace tmdb
{

serializer::serializer(size_t size): size_(size), hlen_(0), external_(false)
  {  
    buf_  = (char*)malloc(size_);
    if(!buf_)
    { 
      malloc_failed_stderr(__func__, errno);
      return;
    }
    memset(buf_, 0x00, size_);
    message_len_ = 0;
    pos_         = buf_ + hlen_; 
    beg_         = buf_;
    block_len_   = 0;
  }


/* used for deserializing external buffers */
serializer::serializer(char* buf): size_(32), hlen_(0), external_(true)
  {  
    buf_  = buf;
    message_len_ = 0;
    pos_         = buf_ + hlen_; 
    beg_         = buf_;
    block_len_   = 0;
  }


serializer::serializer(char* buf, int len): size_(len), hlen_(0), external_(true)
  { 
    std::cout << len << std::endl; 
    buf_  = buf;
    message_len_ = len;
    pos_         = buf_ + hlen_; 
    beg_         = buf_;
    block_len_   = 0;
  }



bool serializer::out_of_mem()
  {
    /* realloc preserving old data */  
    size_ *= 2;
    size_t shift1 = pos_ - buf_;
    size_t shift2 = beg_ - buf_;
    char* buf_new;
    buf_new = (char*)malloc(size_);
    if(!buf_new)
    {
      malloc_failed_stderr(__func__, errno);
      return false;
    }
    memset(buf_new, 0x00, size_);
    memcpy(buf_new, buf_, size_/2);
    free(buf_);
    buf_ = buf_new;
    pos_ = buf_ + shift1;
    beg_ = buf_ + shift2;
            
    std::cout << "out_of_mem: " << size_ << std::endl;
    return true;
  }
 

void serializer::malloc_failed_stderr(const char* func, int err)
  {
    char buf[128] = {0};
    sprintf(buf, "[%s] malloc failed: %s\n", func, strerror(err)); 
    fwrite(buf, 1, strlen(buf), stderr);
  }


void serializer::update_buffer(void* bufin, size_t sizein)
  {
    while(size_ <= sizein)
    { out_of_mem(); }
    memcpy(buf_, bufin, sizein);
    message_len_ = (int)sizein;
  }


/* paired with detach_buffer, allocation */
void serializer::shrink_to_fit()
{
  void* mem = malloc(size_);
  if(!mem)
  {
    malloc_failed_stderr(__func__, errno);
  }

  memcpy(mem, buf_, size_);
  buf_copy_ = buf_;
  buf_ = (char*)mem;
}

/* return internal buffer, take a copy made with shrink_to_fit, no allocation */
void* serializer::detach_buffer()
 {
    void* mem = (void*)buf_;
    buf_ = buf_copy_;
    buf_copy_ = NULL;
    std::cout << __PRETTY_FUNCTION__ << ": message_len_: " << message_len_ << std::endl; 
  
    clear();
    
    return mem;
  }
 

/* copy buffer of message_len_ into new memory chunk, allocation */
void* serializer::copy_buffer()
{
  void* mem = malloc(message_len_);
  if(!mem)
  {
    malloc_failed_stderr(__func__, errno);
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
      { out_of_mem(); }
    }
    else
    {
      hlen_ = strlen(id) + 1;
      while(pos_ - buf_ + hlen_ + sizeof(hlen_) >= size_)
      { out_of_mem(); }
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
    std::cout << __PRETTY_FUNCTION__ << ": " << block_len_ << ":" << message_len_ << std::endl;
  }
  
  
  
  /* 
   * read block into id and set pointer to data,
   * must be followed by deserialize() methods, no allocation
   */
  bool serializer::read_block(char* id)
  {    
    strcpy(id, pos_);
    hlen_ = strlen(id);
    
    while(pos_ - buf_ + hlen_ + sizeof(hlen_) >= size_)
    { out_of_mem(); }
    
    if(message_len_ <= pos_ - buf_)
    { return false; }
        
    pos_ = beg_ + hlen_ + sizeof(hlen_) + 1;
    memcpy(&block_len_, beg_ + hlen_ + 1, sizeof(block_len_));
    beg_ += block_len_ + hlen_ + sizeof(block_len_) + 1; 

    return true;
  }
  
  /* iterate over dev_id, no allocation */
  void* serializer::get_block(char* id)
  {    
    strcpy(id, pos_);
    hlen_ = strlen(id);

    while(pos_ - buf_ + hlen_ + sizeof(hlen_) >= size_)
    { /* TODO unit test */
      if(!external_) 
      { out_of_mem(); } 
      else
      { return NULL; }
    }
    
     
    if(message_len_ <= pos_ - buf_)
    { return NULL; }
        
    memcpy(&block_len_, beg_ + hlen_ + 1, sizeof(block_len_));
    pos_ = beg_ + hlen_ + sizeof(hlen_) + 1 + block_len_;
    beg_ += block_len_ + hlen_ + sizeof(block_len_) + 1; 
    
    /* return pointer to data behind current dev_id */
    return pos_ - block_len_;
  }

}
