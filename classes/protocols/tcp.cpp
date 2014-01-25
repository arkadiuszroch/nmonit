#include "tcp.hpp"

using namespace std;

NM_protocolTcp::NM_protocolTcp()
{
    NM_debug * debug = NM_debug::get_instance("TCP Protocol - Constructor");
    debug->info("New instance of TCP class without parameters");

    this->init();
}

NM_protocolTcp::NM_protocolTcp(NM_header_tcp * packet, in_addr source_ip, in_addr dest_ip, int packet_size, int ip_size)
{
    NM_debug * debug = NM_debug::get_instance("TCP Protocol - Constructor");

    debug->info("New instance of TCP class with parameters");
    string key;
    int tcp_size;
    bool rx = true;
    NM_device * dev = NULL;
    
    try
    {
        this->load_file();
    }
    catch( NM_exception e )
    {
        NM_trigger_warning("Blad ladowania polaczen TCP: " + e.get_message());
    }
    
    tcp_size = TH_OFF(packet)*4;

    debug->info("Getting device");
    dev = NM_deviceList::get_instance()->find_by_ip(source_ip);
    
    if( dev != NULL )
    {
        debug->info("Device is not null: " + dev->get_device_name() + ". Source addres is local address");
        this->local_ip = source_ip;
        this->local_port = ntohs(packet->th_sport);

        this->rem_ip = dest_ip;
        this->rem_port = ntohs(packet->th_dport);

        rx = false;
    }
    else
    {
        dev = NM_deviceList::get_instance()->find_by_ip(ip_to_string(dest_ip));
        debug->info("Device is null so local address is destination address");
        this->local_ip = dest_ip;
        this->local_port = ntohs(packet->th_dport);

        this->rem_ip = source_ip;
        this->rem_port = ntohs(packet->th_sport);

        rx = true;
    }

    if(dev == NULL)
    {
        NM_bug("Unrecognized local device. Drop packet: ");
        NM_out << "\tSource address: " << ip_to_string(source_ip) << ":" << packet->th_sport << endl;
        NM_out << "\tDestination address: " << ip_to_string(dest_ip) << ":" << packet->th_dport << endl;
        NM_out << "End dropped packet" << endl;
        return;
    }
    
    debug->info("Getting size of packet");
    this->size = ntohs(packet_size) - (ip_size + tcp_size);

    debug->info("Generate key for this packet");
    key = this->generate_list_key(ip_to_string(this->local_ip), this->local_port, ip_to_string(this->rem_ip), this->rem_port);
    debug->info("Generated key: " + key);
    debug->info("Checking wheather connection with that key exists");
    
    pthread_mutex_lock(&NM_mutex_conn_list);
    
    if( ::NM_connections_v4.count(key) == 0 )
    {
        debug->info("Connection doesn't exist so we scan the tcp connections");
        //pthread_mutex_unlock(&NM_mutex_conn_list);
        this->load_connections();
        
        //pthread_mutex_lock(&NM_mutex_conn_list);
        debug->info("Checking that the connection now exist");
        if( ::NM_connections_v4.count(key) == 0 )
        {
            debug->info("Connection not exists so we display info about this bug");
            NM_bug("Connection was closed before we get information about it. Well, it happens so.");
            ::NM_connections_v4[key] = new NM_connection(this->local_ip, this->local_port, this->rem_ip, this->rem_port, ::NM_inodes.find(0), NM_ip_v4, NM_protocol_tcp);
            ::NM_connections_v4[key]->set_remote(NM_add_remote(this->rem_ip));
            pthread_mutex_unlock(&NM_mutex_conn_list);
            
            NM_stream out;
            
            out.set_stream(NM_stream_file, ".bug", false);
            out << "Unrecognized tcp packet" << endl;
            
            out << "Source IP: \t" << ip_to_string(source_ip);
            out << ":" << packet->th_sport << endl;
            
            out << "Dest IP: \t" << ip_to_string(dest_ip);
            out << ":" << packet->th_dport << endl;
            
            out << "Seq number:\t" << packet->th_seq << endl;
            
            out << "Flags: \t" << packet->th_flags << endl;
            
            
            out << "Ack Flag: \t" << packet->th_ack << endl;
            
            out << "Window: \t" << packet->th_win << endl;
            
            out << "Checksum: \t" << packet->th_sum << endl;
            
            out << "Payload size \t" << this->size << endl;
            
            out << "------------- END OF PACKET --------------" << endl;
            
            return;
        }
    }
    
    debug->info("Connection exists so we add size of this packet");
    
    ::NM_connections_v4[key]->reset_counter();
    ::NM_connections_v4[key]->add_size(this->size + tcp_size + ip_size, rx);
    
    ::NM_connections_v4[key]->get_inode()->second.remote = NM_add_remote(rem_ip);
    pthread_mutex_unlock(&NM_mutex_conn_list);
    
    this->file_buf.clear();
}

void NM_protocolTcp::load_file()
{
    NM_debug * debug = NM_debug::get_instance("TCP Protocol - Loading file");

    debug->info("Start loading tcp file");
    fstream file("/proc/net/tcp", ios::in);
    string line;
    
    debug->info("Opening tcp file");
    
    if( !file.good() )
        throw NM_exception(NM_ex_code_file_perm_denied, "Unable to open /proc/net/tcp file");

    debug->info("Getting firs line");
    
    getline(file, line); // ingoring first line
    
    debug->info("Start loop");
    
    while( !file.eof() )
    {
        getline(file, line);
        this->file_buf.push_back(line);
    }
    
    debug->info("Closing file");
    
    file.close();
    
    debug->info("TCP file loaded");
}

void NM_protocolTcp::load_connections()
{
    NM_debug * debug = NM_debug::get_instance("TCP Protocol - Loading connections");

    debug->info("Start loading connections");
    
    for(unsigned int i = 0; i < this->file_buf.size(); i++)
    {
        unsigned int a, b, c, d, local_port, rem_port;
        long int inode;
        char hex_local_ip[128], hex_rem_ip[128];
        struct in_addr local_ip, rem_ip;
        string str_local_ip, str_rem_ip, key, line = this->file_buf[i];
        ostringstream sstream;
        NM_device * device = NULL;

        debug->info("Scanign line for search ips, ports and inode");
        int matches = sscanf(line.c_str(), "%*d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %*X %*X:%*X %*X:%*X %*X %*d %*d %ld %*512s\n",
                hex_local_ip, &local_port, hex_rem_ip, &rem_port, &inode);

        if( matches != 5 )
            continue;

        // checking that connection is in TIME_WAIT state. It's realy old data
        if (inode > 0) 
        {
            sscanf(hex_local_ip, "%2x%2x%2x%2x", &d, &c, &b, &a);

            sstream << a << "." << b << "." << c << "." << d;
            str_local_ip = sstream.str();
            sstream.str("");

            device = NM_deviceList::get_instance()->find_by_ip(str_local_ip);
            
            if( device == NULL )
                continue; // Device isn't monitored so we get next line
            if( ::NM_inodes.count(inode) == 0 )
            {
                NM_remap_inodes();

                if( ::NM_inodes.count(inode) == 0 )
                    continue;
            }
            ::NM_inodes[inode].device = device;

            sscanf(hex_rem_ip, "%2x%2x%2x%2x", &d, &c, &b, &a);

            sstream << a << "." << b << "." << c << "." << d;
            str_rem_ip = sstream.str();
            sstream.str("");

            local_ip = ip_to_in_addr(str_local_ip);
            rem_ip = ip_to_in_addr(str_rem_ip);

            key = this->generate_list_key(str_local_ip, local_port, str_rem_ip, rem_port);
            //pthread_mutex_lock(&::NM_mutex_conn_list);
            if( ::NM_connections_v4.count(key) == 0 )
            {
                NM_inode_list::iterator inode_it;
                if(NM_inodes.count(inode) == 0)
                {
                    inode_it = NM_inodes.find(0);
                    //continue;
                }
                else
                    inode_it = ::NM_inodes.find(inode);

                try
                {
                    //if(inode_it->first != 0)
                        ::NM_connections_v4[key] = new NM_connection(local_ip, local_port, rem_ip, rem_port, inode_it, NM_ip_v4, NM_protocol_tcp);
                }
                catch( NM_exception e )
                {
                    NM_trigger_error("Could not create new connection: " + e.get_message());
                }
            }
            else
            {

                if( !NM_connections_v4[key]->has_inode() || ::NM_connections_v4[key]->get_inode()->first != inode )
                {
                    ::NM_connections_v4[key]->set_inode(::NM_inodes.find(inode));
                }
            }
	}
        
        //pthread_mutex_unlock(&::NM_mutex_conn_list);
    }

    debug->info("Loading TCP connections finished");
}

string NM_protocolTcp::generate_list_key(string local_ip, unsigned int local_port, string rem_ip, unsigned int rem_port)
{
    ostringstream sstream;

    sstream << "tcp_v4|" << local_ip << ":" << local_port << "|" << rem_ip << ":" << rem_port;

    return sstream.str();
}

