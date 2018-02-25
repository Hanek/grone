#include <time.h>
#include <iostream>

#ifndef _SVRTK_TIME_HPP_
#define _SVRTK_TIME_HPP_


namespace tmdb 
{
  
  /*   
   * wrapper for struct timespec  
   * TODO negative time handling   
   */
class time
{
public:
  static const long nsec10L9;
  static const long nsec10L8;
  static const long nsec10L7;
  static const long nsec10L6;
  
  static const std::pair<long int, long int> zero;
  static const std::pair<long int, long int> msSec10;
  static const std::pair<long int, long int> msSec20;
  static const std::pair<long int, long int> msSec30;
  static const std::pair<long int, long int> msSec40;
  static const std::pair<long int, long int> msSec50;
  
  static const std::pair<long int, long int> Sec1;
  static const std::pair<long int, long int> Sec2;
  static const std::pair<long int, long int> Sec3;
  static const std::pair<long int, long int> Sec4;
  static const std::pair<long int, long int> Sec5;

private:
  struct timespec t;

public:
  /*  
   * define display formatting
   *  false - time represented as float
   *  true  - time represented as pair of integers, seconds:nanoseconds
   */
  bool hi_res; 
 
  time();
  time(bool res);
  time(long int sec, long int nsec);
  time(std::pair<long int, long int>);
  time(double seconds);
  
  static void sleep(const time&);

  struct timespec& get() { return t; }
  const struct timespec& get() const { return t; }
  std::string get_str() const;
  double get_double() const;
  void set(long int sec, long int nsec);
  void reset();

  bool operator == (const time& rhs);
  bool operator != (const time& rhs) { return !operator == (rhs); }
  bool operator <  (const time& rhs);
  bool operator >  (const time& rhs);

  bool operator == (const time& rhs) const;
  bool operator != (const time& rhs) const { return !operator == (rhs); }
  bool operator <  (const time& rhs) const;
  bool operator >  (const time& rhs) const;
  
  time& operator = (const time& rhs);
  time operator + (const time& rhs) const;
  time operator - (const time& rhs) const;
};

std::ostream& operator << (std::ostream& os, const time& obj);
int get_time(time& time);

}

#endif