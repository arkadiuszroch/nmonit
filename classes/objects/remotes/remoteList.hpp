#ifndef NM_REMOTE_LIST
#define NM_REMOTE_LIST

#include "../../../functions/global.hpp"
#include "remote.hpp"

class NM_remoteList
{
  private:
    NM_remote * first, * current, * last;
    static NM_remoteList * instance;
    NM_remoteList();
    NM_remoteList(const NM_remoteList &);
    NM_remoteList & operator =(const NM_remoteList &);
    
    static std::string out(NM_remoteList * /*list*/);
    
  public:
    static NM_remoteList * get_instance();
    ~NM_remoteList();

    void add_remote(NM_remote * /*remote*/);
    void delete_remote(NM_remote * /*remote*/);
    void delete_remote(NM_id /*remote_id*/);

    NM_remote * find_by_id(NM_id /*id*/);
    NM_remote * find_by_name(std::string /*name*/);
    NM_remote * find_by_ip(in_addr /*ip*/);
    NM_remote * find_by_ip(in6_addr /*ip*/);
    NM_remote * find_by_ip(std::string /*ip*/, NM_ip_type = NM_ip_v4 /*ip_type*/);

    NM_id get_remote_id(std::string /*name*/);
    NM_id get_remote_id(in_addr /*ip*/);
    NM_id get_remote_id(in6_addr /*ip*/);
    // We must specify IP version because if we add default parameter NM_ip_type function will have the same declaration as get id by name.
    NM_id get_remote_id(std::string /*ip*/, NM_ip_type /*ip_type*/);

    std::string get_remote_name(NM_id /*id*/);
    std::string get_remote_name(in_addr /*ip*/);
    std::string get_remote_name(in6_addr /*ip*/);
    std::string get_remote_name(std::string /*ip*/, NM_ip_type = NM_ip_v4 /*ip_type*/);

    std::string get_remote_ip(NM_id /*id*/);
    std::string get_remote_ip(std::string /*name*/);
    std::string get_remote_ip(in6_addr /*ip*/);
    
    std::string get_remote_ip6(NM_id /*id*/);
    std::string get_remote_ip6(std::string /*name*/);
    std::string get_remote_ip6(in_addr /*ip*/);

    in_addr get_remote_ip_in_addr(NM_id /*id*/);
    in_addr get_remote_ip_in_addr(std::string /*name*/);
    in_addr get_remote_ip_in_addr(in6_addr /*ip*/);

    in6_addr get_remote_ip_in6_addr(NM_id /*id*/);
    in6_addr get_remote_ip_in6_addr(std::string /*name*/);
    in6_addr get_remote_ip_in6_addr(in_addr /*ip*/);

    friend std::ostream & operator << (NM_stream & /*out*/, NM_remoteList * /*list*/);
    friend std::ostream & operator << (std::ostream & /*out*/, NM_remoteList * /*list*/);
};

std::ostream & operator << (std::ostream & /*out*/, NM_remoteList * /*list*/);
std::ostream & operator << (NM_stream & /*out*/, NM_remoteList * /*list*/);

#endif
