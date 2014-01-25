#ifndef NM_DEVICE
#define NM_DEVICE

#include "../../../functions/global.hpp"

class NM_deviceList;

class NM_device
{
  private:
    static NM_id device_id_count;
    NM_id device_id, master_id;
    std::string device_name;
    in_addr device_ip_address;
    in6_addr device_ip6_address;
    NM_monitored_state monitored;
    NM_device * next, * prev;
    ~NM_device();
    
    static std::string out(NM_device * /*device*/);

  public:
    NM_device(std::string /*name*/, in_addr /*address*/, in6_addr /*address6*/, NM_id = 0 /*id*/, NM_id = 0 /*master_id*/);

    void set_next(NM_device * /*next*/);
    NM_device * get_next();
    void set_prev(NM_device * /*prev*/);
    NM_device * get_prev();

    void set_monitored(NM_monitored_state = NM_monitored /*monitored*/);
    NM_monitored_state is_monitored();

    static NM_id get_device_id_count();
    static void incr_device_id_count();
    static NM_id generate_device_id();

    NM_id get_device_id();
    
    NM_id get_master_id();
    void set_master_id(NM_id /*master_id*/);

    std::string get_device_name();

    std::string get_device_ip();
    std::string get_device_ip6();

    in_addr get_device_ip_in_addr();
    in6_addr get_device_ip_in6_addr();

    friend std::ostream & operator << (NM_stream & /*out*/, NM_device * /*device*/);
    friend std::ostream & operator <<(std::ostream & /*out*/, NM_device * /*device*/);
    friend class NM_deviceList;
};

std::ostream & operator <<(std::ostream & /*out*/, NM_device * /*device*/);
std::ostream & operator << (NM_stream & /*out*/, NM_device * /*device*/);
#endif
