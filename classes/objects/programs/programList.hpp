#ifndef NM_PROGRAM_LIST
#define NM_PROGRAM_LIST

#include "../../../functions/global.hpp"
#include "program.hpp"

class NM_programList
{
  private:
    NM_program * first, * current, * last;
    static NM_programList * instance;
    NM_programList();
    NM_programList(const NM_programList &);
    NM_programList & operator =(const NM_programList &);
    
    static std::string out(NM_programList * /*list*/);
    
  public:
    static NM_programList * get_instance();
    ~NM_programList();

    void add_program(NM_program * /*program*/);
    void delete_program(NM_program * /*program*/);
    void delete_program(NM_id /*program_id*/);

    NM_program * find_by_id(NM_id /*id*/);
    NM_program * find_by_name(std::string /*name*/);

    NM_id get_program_id(std::string /*name*/);

    std::string get_program_name(NM_id /*id*/);

    friend std::ostream& operator <<(NM_stream & /*out*/, NM_programList * /*list*/);
    friend std::ostream& operator <<(std::ostream & /*out*/, NM_programList * /*list*/);
};

std::ostream& operator <<(NM_stream & /*out*/, NM_programList * /*list*/);
std::ostream& operator <<(std::ostream & /*out*/, NM_programList * /*list*/);

#endif
