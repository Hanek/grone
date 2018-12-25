#include "time.hpp"
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <cstdio>



namespace tmdb 
{
  const long time::nsec10L9 = 1000000000;
  const long time::nsec10L8 = 100000000;
  const long time::nsec10L7 = 10000000;
  const long time::nsec10L6 = 1000000; 

  const std::pair<long int, long int> time::zero        = std::make_pair(0, 0);
  const std::pair<long int, long int> time::msSec10 = std::make_pair(0, 10000000);
  const std::pair<long int, long int> time::msSec20 = std::make_pair(0, 20000000);
  const std::pair<long int, long int> time::msSec30 = std::make_pair(0, 30000000);
  const std::pair<long int, long int> time::msSec40 = std::make_pair(0, 40000000);
  const std::pair<long int, long int> time::msSec50 = std::make_pair(0, 50000000);

  const std::pair<long int, long int> time::Sec1 = std::make_pair(1, 0);
  const std::pair<long int, long int> time::Sec2 = std::make_pair(2, 0);
  const std::pair<long int, long int> time::Sec3 = std::make_pair(3, 0);
  const std::pair<long int, long int> time::Sec4 = std::make_pair(4, 0);
  const std::pair<long int, long int> time::Sec5 = std::make_pair(5, 0);


  void time::sleep(const time& tm)
  {
    nanosleep(&tm.get() , NULL);
  }

  time::time()   
  {
    t.tv_sec     = 0;
    t.tv_nsec   = 0;
    hi_res        = false;
  }


  time::time(bool res)
  {
    t.tv_sec     = 0;
    t.tv_nsec   = 0;
    hi_res        = res;
  }

  time::time(long int sec, long int nsec)
  {
    t.tv_sec     = sec;
    t.tv_nsec   = nsec;
    hi_res        = false;
  }

  time::time(std::pair<long int, long int> timeval)
  {
    t.tv_sec     = timeval.first;
    t.tv_nsec   = timeval.second;
    hi_res        = false;
  }

  time::time(double seconds)
  {
    double fractpart, intpart;
    fractpart = modf(seconds , &intpart);
    t.tv_sec     = (long int)intpart;
    t.tv_nsec   = (long int)(fractpart*1000000000);
    hi_res        = false;
  }

  void time::set(long int sec, long int nsec)
  {
    t.tv_sec = sec;
    t.tv_nsec = nsec;
  }

  void time::reset()
  {
    t.tv_sec = 0;
    t.tv_nsec = 0;
  }

  time& time::operator = (const time& rhs)
  {
    t.tv_sec = rhs.t.tv_sec;
    t.tv_nsec = rhs.t.tv_nsec;
    return *this;
  }


  bool time::operator == (const time& rhs)
  { return (t.tv_sec == rhs.t.tv_sec && t.tv_nsec == rhs.t.tv_nsec) ? true : false; }

  bool time::operator < (const time& rhs)
  {
    if(t.tv_sec > rhs.t.tv_sec)
      return false;

    if(t.tv_sec < rhs.t.tv_sec)
      return true;

    if(t.tv_nsec >= rhs.t.tv_nsec)
      return false;

    return true;
  }

  bool time::operator >  (const time& rhs)
  {
    if(t.tv_sec > rhs.t.tv_sec)
      return true;

    if(t.tv_sec < rhs.t.tv_sec)
      return false;

    if(t.tv_nsec > rhs.t.tv_nsec)
      return true;

    return false;	
  }

  bool time::operator == (const time& rhs) const
  { return (t.tv_sec == rhs.t.tv_sec && t.tv_nsec == rhs.t.tv_nsec) ? true : false; }

  bool time::operator < (const time& rhs) const
  {
    if(t.tv_sec > rhs.t.tv_sec)
      return false;

    if(t.tv_sec < rhs.t.tv_sec)
      return true;

    if(t.tv_nsec >= rhs.t.tv_nsec)
      return false;

    return true;
  }

  bool time::operator >  (const time& rhs) const
  {
    if(t.tv_sec > rhs.t.tv_sec)
      return true;

    if(t.tv_sec < rhs.t.tv_sec)
      return false;

    if(t.tv_nsec > rhs.t.tv_nsec)
      return true;

    return false;
  }

  time time::operator + (const time& rhs) const
  {
    time sum(time::zero);
    sum.t.tv_sec = t.tv_sec + rhs.t.tv_sec;
    sum.t.tv_nsec = t.tv_nsec + rhs.t.tv_nsec;

    if(nsec10L9 <= sum.t.tv_nsec)
    {
      sum.t.tv_sec++;
      sum.t.tv_nsec = sum.t.tv_nsec - nsec10L9;
    }
    return sum;
  }

  time time::operator - (const time& rhs) const
  {
    time sub(time::zero);
    sub.t.tv_sec = t.tv_sec - rhs.t.tv_sec;

    if(t.tv_nsec >= rhs.t.tv_nsec)
    { sub.t.tv_nsec = t.tv_nsec - rhs.t.tv_nsec; }
    else
    {
      sub.t.tv_sec--;
      sub.t.tv_nsec = nsec10L9 - rhs.t.tv_nsec + t.tv_nsec;
    }
    return sub;
  }


  std::string time::get_str() const
  {  
    double time_conv = static_cast<double>(t.tv_sec) + (static_cast<double>(t.tv_nsec))/1000000000.0;
    char buffer[64] = {0};    
    snprintf(buffer, sizeof(buffer), "%f", time_conv);
    std::string res(buffer);

    return res;
  }
  
  std::string time::get_date() const
  {
    size_t len = 32;
    struct tm t_t;
    std::string res;
    char str[len] = {0};

    tzset();
    if(!localtime_r(&t.tv_sec, &t_t) || 0 == strftime(str, len, "%F %T", &t_t))
    {
        return res;
    }
     
    sprintf(str + strlen(str), ".%02ld", t.tv_nsec);
    res = str;
    return res;
}


  double time::get_double() const
  {  
    double time_double = static_cast<double>(t.tv_sec) + (static_cast<double>(t.tv_nsec))/1000000000.0;
    return time_double;
  }


  std::ostream& operator << (std::ostream& os, const time& tm)
  {
    int len = 0x32;
    char buffer[len];
    memset(buffer, '\0', len);
    int res = (tm.hi_res) ? 1 : 0;

    switch (res)
    {
      case 1:
        os << std::setw(9) << tm.get().tv_sec << ":" << std::setw(9) << tm.get().tv_nsec;
        break;

      case 0:
        double time_conv = static_cast<double>(tm.get().tv_sec) + (static_cast<double>(tm.get().tv_nsec))/1000000000.0;
        snprintf(buffer, sizeof(buffer), "%f", time_conv);
        os << buffer;
        break;
    }  
    return os;
  }


  int get_time(time& time)
  {
    if(0 != clock_gettime(CLOCK_REALTIME, &time.get()))
    { return errno; }

    return 0;
  }

}
