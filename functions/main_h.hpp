#ifndef NM_MAIN_H
#define NM_MAIN_H

#include "global.hpp"
#include "objects.hpp"

extern pthread_mutex_t NM_mutex_conn_list, NM_mutex_remap_inodes, NM_mutex_remap_devices;

enum NM_info_pc_objects
{
    NM_object_device = 1,
    NM_object_remote,
    NM_object_program,
    NM_object_user,
    NM_date,
    NM_hour
};

enum NM_connection_type
{
    NM_connection_tcp = 1,
    NM_connection_tcp6,
    NM_connection_udp,
    NM_connection_udp6,
    NM_connection_icmp
};

enum NM_thread_state
{
    NM_thread_state_starting = 1,
    NM_thread_state_running,
    NM_thread_state_stopped
};

enum NM_protocol_type
{
    NM_protocol_tcp = 1,
    NM_protocol_udp,
    NM_protocol_icmp
};

struct NM_inode
{
    NM_device * device;
    NM_program * program;
    NM_user * user;
    NM_remote * remote;
};

struct NM_date
{
    NM_uint year;
    NM_uint month;
    NM_uint day;
    NM_uint hour;
    NM_uint minute;
    NM_uint second;
};

struct NM_thread
{
    pthread_t thread;
    NM_thread_state state;

    NM_thread()
    {
        this->state = NM_thread_state_stopped;
    }
};

typedef std::map<int, struct NM_inode> NM_inode_list;
typedef std::map<int /*device id*/, NM_thread /*thread*/> NM_thread_list;

#endif

