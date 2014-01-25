#ifndef NM_PROTOCOL
#define NM_PROTOCOL

#include "../functions/main_h.hpp"

extern NM_inode_list NM_inodes;

class NM_protocol
{
  protected:
    unsigned int inode, local_port, rem_port, size;
    in_addr local_ip, rem_ip;
    NM_string_list file_buf;
  public:
    virtual ~NM_protocol();
    virtual unsigned int get_inode();

    // info about 
    virtual unsigned int get_local_port();
    virtual in_addr get_local_ip();

    // info about recipient
    virtual unsigned int get_rem_port();
    virtual in_addr get_rem_ip();

    virtual unsigned int get_size();

    virtual void load_connections() = 0;
  protected:
    void init();
    
    virtual void load_file() = 0;
};

#endif
