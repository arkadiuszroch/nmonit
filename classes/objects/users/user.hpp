#ifndef NM_USER
#define NM_USER

#include "../../../functions/global.hpp"

class NM_userList;

class NM_user
{
  private:
    static NM_id user_id_count;
    NM_id user_id, master_id;
    NM_id user_system_id;
    std::string user_name;
    NM_monitored_state monitored;
    NM_user * next, * prev;

    ~NM_user();
    
    static std::string out(NM_user * /*user*/);
  public:
    NM_user(NM_id /*system_id*/, std::string /*name*/, NM_id = 0 /*id*/, NM_id = 0 /*master_id*/);

    void set_next(NM_user * /*next*/);
    NM_user * get_next();
    void set_prev(NM_user * /*prev*/);
    NM_user * get_prev();

    void set_monitored(NM_monitored_state = NM_monitored /*monitored*/);
    NM_monitored_state is_monitored();

    static NM_id get_user_id_count();
    static void incr_user_id_count();
    static NM_id generate_user_id();

    NM_id get_user_id();

    NM_id get_master_id();
    void set_master_id(NM_id /*master_id*/);

    NM_id get_user_system_id();

    std::string get_user_name();
    friend std::ostream& operator <<(NM_stream & /*out*/, NM_user * /*user*/);
    friend std::ostream& operator <<(std::ostream & /*out*/, NM_user * /*user*/);
    friend class NM_userList;
};

std::ostream& operator <<(NM_stream & /*out*/, NM_user * /*user*/);
std::ostream& operator <<(std::ostream & /*out*/, NM_user * /*user*/);
#endif
