#ifndef NM_PROGRAM
#define NM_PROGRAM

#include "../../../functions/global.hpp"

class NM_programList;

class NM_program
{
  private:
    static NM_id program_id_count;
    NM_id program_id, master_id;
    std::string program_name;
    NM_monitored_state monitored;
    NM_program * next, * prev;

    ~NM_program();
    
    static std::string out(NM_program * /*program*/);
  public:
    NM_program(std::string /*name*/, NM_id = 0 /*id*/, NM_id = 0 /*master_id*/);

    void set_next(NM_program * /*next*/);
    NM_program * get_next();
    void set_prev(NM_program * /*prev*/);
    NM_program * get_prev();

    void set_monitored(NM_monitored_state = NM_monitored /*monitored*/);
    NM_monitored_state is_monitored();

    static NM_id get_program_id_count();
    static void incr_program_id_count();
    static NM_id generate_program_id();

    NM_id get_program_id();

    NM_id get_master_id();
    void set_master_id(NM_id /*master_id*/);

    std::string get_program_name();

    friend std::ostream & operator << (NM_stream & /*out*/, NM_program * /*program*/);
    friend std::ostream & operator << (std::ostream & /*out*/, NM_program * /*program*/);
    friend class NM_programList;

};

std::ostream & operator << (NM_stream & /*out*/, NM_program * /*program*/);
std::ostream & operator << (std::ostream & /*out*/, NM_program * /*program*/);

#endif
