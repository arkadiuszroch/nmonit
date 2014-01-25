#ifndef NM_DATABASE
#define	NM_DATABASE

#include "../functions/global.hpp"
#include "sqlite/sqlite3.h"

class NM_database
{
  private:
    static NM_database * instance;
    static pthread_mutex_t mutex;
    sqlite3 * db;
    int error_code;
    std::string error_message;
    NM_string_list queries;
    
  private:
    NM_database();
    void open();
    void close();
    NM_2D_string_list select(std::string /*query*/);
  public:
    static NM_database * get_instance();
    ~NM_database();
    NM_2D_string_list query(std::string /*query*/);
    bool is_error();
    std::string get_error();
    int get_error_code();
};

#endif

