#ifndef NM_REMOTE
#define NM_REMOTE

#include "../../../functions/global.hpp"

class NM_remoteList;

class NM_remote
{
  private:
    static NM_id remote_id_count;
    NM_id remote_id, master_id;
    std::string remote_name;
    in_addr remote_ip_address;
    in6_addr remote_ip6_address;
    NM_monitored_state monitored; 
    NM_remote * next, * prev;
    ~NM_remote();
    
    void init(std::string /*name*/, in_addr /*address*/, in6_addr /*address6*/, NM_id = 0 /*id*/, NM_id = 0 /*master_id*/);
    static std::string out(NM_remote * /*remote*/);
  public:
    NM_remote(std::string /*name*/, in_addr /*address*/, NM_id = 0 /*id*/, NM_id = 0 /*master_id*/);
    NM_remote(std::string /*name*/, in6_addr /*address*/, NM_id = 0 /*id*/, NM_id = 0 /*master_id*/);
    NM_remote(std::string /*name*/, in_addr /*address*/, in6_addr /*address6*/, NM_id = 0 /*id*/, NM_id = 0 /*master_id*/);

    void set_next(NM_remote * /*next*/);
    NM_remote * get_next();
    void set_prev(NM_remote * /*prev*/);
    NM_remote * get_prev();

    void set_monitored(NM_monitored_state = NM_monitored /*monitored*/);
    NM_monitored_state is_monitored();

    static NM_id get_remote_id_count();
    static void incr_remote_id_count();
    static NM_id generate_remote_id();

    NM_id get_remote_id();

    NM_id get_master_id();
    void set_master_id(NM_id /*master_id*/);

    std::string get_remote_name();

    bool set_remote_ip(in_addr /*ip*/);
    bool set_remote_ip(std::string /*ip*/);
    
    bool set_remote_ip6(in6_addr /*ip*/);
    bool set_remote_ip6(std::string /*ip*/);
    
    std::string get_remote_ip();
    std::string get_remote_ip6();

    in_addr get_remote_ip_in_addr();
    in6_addr get_remote_ip_in6_addr();

    friend std::ostream & operator <<(std::ostream & /*out*/, NM_remote * /*remote*/);
    friend std::ostream & operator <<(NM_stream & /*out*/, NM_remote * /*remote*/);
    friend class NM_remoteList;
};

std::ostream & operator << (std::ostream & /*out*/, NM_remote * /*remote*/);
std::ostream & operator << (NM_stream & /*out*/, NM_remote * /*remote*/);

#endif
