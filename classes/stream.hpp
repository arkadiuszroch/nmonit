#ifndef NM_STREAM
#define	NM_STREAM
#include <pthread.h>
#include "../functions/global_h.hpp"
#include "exception.hpp"

enum NM_stream_type
{
    NM_stream_cout = 1,
    NM_stream_file = 2
};

class NM_stream : public std::ostream
{
  private:
    NM_stream_type stream_type;
    std::string file_name;
    std::fstream file;
    pthread_mutex_t mutex;
    std::ostream * out;

  public:
    NM_stream();
    NM_stream(const NM_stream &);
    NM_stream & operator =(const NM_stream &);
    ~NM_stream();
    void set_stream(NM_stream_type /*stream_type*/, std::string = "" /*file_name*/, bool = true /*truncate*/);
    NM_stream_type get_stream_type();
    std::string get_file_name();
    
    std::ostream & operator <<(bool /*val*/);
    std::ostream & operator <<(short /*val*/);
    std::ostream & operator <<(unsigned short /*val*/);
    std::ostream & operator <<(int /*val*/);
    std::ostream & operator <<(unsigned int /*val*/);
    std::ostream & operator <<(long /*val*/);
    std::ostream & operator <<(unsigned long /*val*/);
    std::ostream & operator <<(float /*val*/);
    std::ostream & operator <<(double /*val*/);
    std::ostream & operator <<(long double /*val*/);
    std::ostream & operator <<(const void* /*val*/);
    std::ostream & operator <<(const char * /*val*/);
    std::ostream & operator <<(const signed char * /*val*/);
    std::ostream & operator <<(const unsigned char * /*val*/);
    std::ostream & operator <<(std::string /*val*/);
    std::ostream & operator <<(std::ostream & /*out*/);
};


#endif

