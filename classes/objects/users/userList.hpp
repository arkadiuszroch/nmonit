#ifndef NM_USER_LIST
#define NM_USER_LIST

#include "../../../functions/global.hpp"
#include "user.hpp"

class NM_userList
{
  private:
    NM_user * first, * current, * last;
    static NM_userList * instance;
    NM_userList();
    NM_userList(const NM_userList &);
    NM_userList & operator =(const NM_userList &);
    
    static std::string out(NM_userList * /*list*/);
  public:
    static NM_userList * get_instance();
    ~NM_userList();

    void add_user(NM_user * /*user*/);
    void delete_user(NM_user * /*user*/);
    void delete_user(NM_id /*user_id*/);

    NM_user * find_by_id(NM_id /*id*/);
    NM_user * find_by_system_id(NM_id /*system_id*/);
    NM_user * find_by_name(std::string /*name*/);

    NM_id get_user_id(NM_id /*system_id*/);
    NM_id get_user_id(std::string /*name*/);

    NM_id get_user_system_id(NM_id /*id*/);
    NM_id get_user_system_id(std::string /*name*/);

    std::string get_user_name(NM_id /*id*/, bool = false /*system_id*/);

    friend std::ostream& operator <<(NM_stream & /*out*/, NM_userList * /*list*/);
    friend std::ostream& operator <<(std::ostream & /*out*/, NM_userList * /*list*/);
};

std::ostream& operator <<(NM_stream & /*out*/, NM_userList * /*list*/);
std::ostream& operator <<(std::ostream & /*out*/, NM_userList * /*list*/);

#endif
