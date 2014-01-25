#ifndef NM_DEVICE_LIST
#define NM_DEVICE_LIST

#include "../../../functions/global.hpp"
#include "device.hpp"

class NM_deviceList
{
  private:
    NM_device * first, * current, * last;
    static NM_deviceList * instance;
    NM_deviceList();
    NM_deviceList(const NM_deviceList &);
    NM_deviceList & operator =(const NM_deviceList &);
    
    static std::string out(NM_deviceList * /*list*/);
    
  public:
    static NM_deviceList * get_instance();
    ~NM_deviceList();

    void add_device(NM_device * /*device*/);
    void delete_device(NM_device * /*device*/);
    void delete_device(NM_id /*device_id*/);

    NM_device * get_first();
    
    NM_device * find_by_id(NM_id /*id*/);
    NM_device * find_by_name(std::string /*name*/);
    NM_device * find_by_ip(in_addr /*ip*/);
    NM_device * find_by_ip(in6_addr /*ip*/);
    NM_device * find_by_ip(std::string /*ip*/, NM_ip_type = NM_ip_v4 /*ip_type*/);

    NM_id get_device_id(std::string /*name*/);
    NM_id get_device_id(in_addr /*ip*/);
    NM_id get_device_id(in6_addr /*ip*/);
    NM_id get_device_id(std::string /*ip*/, NM_ip_type = NM_ip_v4 /*ip_type*/);

    std::string get_device_name(NM_id /*id*/);
    std::string get_device_name(in_addr /*ip*/);
    std::string get_device_name(in6_addr /*ip*/);
    std::string get_device_name(std::string /*ip*/, NM_ip_type = NM_ip_v4 /*ip_type*/);

    std::string get_device_ip(NM_id /*id*/);
    std::string get_device_ip(std::string /*name*/);
    std::string get_device_ip(in6_addr /*ip*/);
    
    std::string get_device_ip6(NM_id /*id*/);
    std::string get_device_ip6(std::string /*name*/);
    std::string get_device_ip6(in_addr /*ip*/);

    in_addr get_device_ip_in_addr(NM_id /*id*/);
    in_addr get_device_ip_in_addr(std::string /*name*/);
    in_addr get_device_ip_in_addr(in6_addr /*ip*/);
    
    in6_addr get_device_ip_in6_addr(NM_id /*id*/);
    in6_addr get_device_ip_in6_addr(std::string /*name*/);
    in6_addr get_device_ip_in6_addr(in_addr /*ip*/);

    friend std::ostream& operator <<(NM_stream & /*out*/, NM_deviceList * /*list*/);
    friend std::ostream& operator <<(std::ostream & /*out*/, NM_deviceList * /*list*/);
};

std::ostream& operator <<(NM_stream & /*out*/, NM_deviceList * /*list*/);
std::ostream& operator <<(std::ostream & /*out*/, NM_deviceList * /*list*/);

#endif
