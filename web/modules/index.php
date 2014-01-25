<?php

class NM_module_index extends NM_basic_module
{
    
    private $_programs;
    private $_users;
    private $_devices;  
    private $_remotes;
    
    public function __construct()
    {   
        $this->_programs = array();
        $this->_users = array();
        $this->_devices = array();
        $this->_remotes = array();
        
        $this->_programs[-1] = new NM_object_program(-1, 'unknown', new NM_date());
        $this->_users[-1] = new NM_object_user(-1, 'unknown', new NM_date(), -1);
        $this->_devices[-1] = new NM_object_device(-1, 'unknown', new NM_date(), NM_DEF_ADDRESS, NM_DEF_ADDRESS6);
        $this->_remotes[-1] = new NM_object_remote(-1, 'unknown', new NM_date(), NM_DEF_ADDRESS, NM_DEF_ADDRESS6);
     
    }
    
            
    public function action_index()
    { 
        /* * /$model = new NM_basic_model();
        
        //$v = $model->query("SELECT date, SUM(rx_size) as rx, SUM(tx_size) as tx FROM net_usage WHERE device_id IN (SELECT id FROM objects WHERE master_id = 2 AND type = 1)");
        /* * /$v = $model->query("SELECT DISTINCT u.date, o.master_id, o2.master_id as compare_id, SUM(u.rx_size), SUM(u.tx_size)
        FROM net_usage u
        JOIN objects o ON (o.id = u.program_id AND o.type = 3) 
        JOIN objects o2 ON (o2.id = u.user_id AND o2.type = 4) 
        WHERE o.master_id = 58 AND o2.master_id = 1
         LIMIT 100;");
         /* * /
        
        ?> <table><tr><?php
        
        $keys = array_keys($v[0]);
        foreach($keys as $key)
        {
            echo "<td>{$key}</td>";
        }
        echo "</tr>";
        foreach($v  as $row)
        {
            echo "<tr>";
            
            foreach($row as $cel)
            {
                echo "<td>{$cel}</td>";
            }
            
            echo "</tr>";
        }
        exit;
        //$v = $model->query("SELECT SUM(rx_size) as rx_size,  SUM(tx_size) as tx_size, o.master_id, o.name FROM net_usage u LEFT JOIN objects o ON (o.id = u.user_id AND type = 4) WHERE u.program_id IN (SELECT id FROM objects WHERE master_id = 58 AND type = 3) GROUP BY o.master_id");
        $v[0]['rx_size_type'] = NM_SIZE_B;
        $v[1]['rx_size_type'] = NM_SIZE_B;
        $v[0]['tx_size_type'] = NM_SIZE_B;
        $v[1]['tx_size_type'] = NM_SIZE_B;
        standarize_size($v[0]['rx_size'], $v[0]['rx_size_type']);
        standarize_size($v[1]['rx_size'], $v[1]['rx_size_type']);
        standarize_size($v[0]['tx_size'], $v[0]['tx_size_type']);
        standarize_size($v[1]['tx_size'], $v[1]['tx_size_type']);
        vde($v);
        header("Content-type: text/html");
        foreach($v as $row)
        {
            $rx_size = $row['rx'];
            $tx_size = $row['tx'];
            $rx = $tx = NM_SIZE_B;
            standarize_size($row['rx'], $rx);
            standarize_size($row['tx'], $tx);
            
            echo date("H:i:s d-m-Y", $row['date']).  " | " . $rx_size . ' - ' . $row['rx'] . size_type_to_string($rx) . " | " . $tx_size . ' - ' . $row['tx'] . size_type_to_string($tx) . '<br />';
        }
        exit; /* */
        $this->add_js_file('js/jqPlot/jquery.jqplot.min.js');
        $this->add_js_file('js/jqPlot/plugins/jqplot.pieRenderer.min.js');
        $this->add_css_file('js/jqPlot/jquery.jqplot.min.css');
        
        $this->add_css_file('css/index.css');
        $this->add_js_file('js/index.js');
        $model = new NM_model_index();
        $this->add_var('data', $model->check_data());
    }
    
    public function action_load_data()
    {
        $model = new NM_model_index();
        $programs = $model->get_programs_summary();
        $users = $model->get_users_summary();
        $devices = $model->get_devices_summary();
        
        $this->add_var('data', array(NM_PROGRAM => array('rx' => $programs['rx']['data'], 'tx' => $programs['tx']['data']),
                                    NM_USER => array('rx' => $users['rx']['data'], 'tx' => $users['tx']['data']),
                                    NM_DEVICE => array('rx' => $devices['rx']['data'], 'tx' => $devices['tx']['data'])));
        $this->add_var('types', array(NM_PROGRAM => 'Program', NM_USER => 'User', NM_DEVICE => 'Device'));
        
        
        $this->set_ajax_var('programs', array('charts' => array('rx' => $programs['rx']['chart'], 'tx' => $programs['tx']['chart']),
                                              'colors' => array('rx' => $programs['rx']['colors'], 'tx' => $programs['tx']['colors'])));
        $this->set_ajax_var('users', array('charts' => array('rx' => $users['rx']['chart'], 'tx' => $users['tx']['chart']),
                                           'colors' => array('rx' => $users['rx']['colors'], 'tx' => $users['tx']['colors'])));
        $this->set_ajax_var('devices', array('charts' => array('rx' => $devices['rx']['chart'], 'tx' => $devices['tx']['chart']),
                                             'colors' => array('rx' => $devices['rx']['colors'], 'tx' => $devices['tx']['colors'])));
        $this->set_ajax_var('status', true);
        $this->set_ajax(true);
        
        $this->set_view('summary');
    }
    
    public function action_converter()
    {
        //exit;
        NM_data_converter::get_instance()->convert_data();
        exit;
    }
    
    public function action_clear_cache()
    {
        NM_cache::clear_all();
    }
    
    public function action_test()
    {
        NM_data_loader::get_instance()->compare(NM_PROGRAM, 58);
    }
    
    public function action_transfer()
    {
        $type = NM_loader::get_get('type');
        
        if(!in_array($type, array('usage', 'objects', 'optimize'))) return;
        
        if($type == 'usage')
        {
            set_time_limit(0);
            $c = NM_data_converter::get_instance();
            $c->convert_data();
            return;
        }
        else if($type == 'optimize')
        {
            $sql = "BEGIN TRANSACTION; ";
            $sql .= "INSERT OR REPLACE INTO objects_summary SELECT o.master_id, o.type,
        SUM(rx_size) + ifnull((SELECT s.rx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0), 
        SUM(tx_size) + ifnull((SELECT s.tx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0) 
        FROM net_usage u LEFT JOIN objects o ON (o.id = u.program_id AND o.type = " . NM_OBJECT_PROGRAM . ") 
        WHERE u.date > (SELECT od.date FROM optimize_date od WHERE od.type = \"summary\")
        GROUP BY o.master_id;";
    
    $sql .= "INSERT OR REPLACE INTO objects_summary SELECT o.master_id, o.type, 
        SUM(rx_size) + ifnull((SELECT s.rx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0), 
        SUM(tx_size) + ifnull((SELECT s.tx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0) 
        FROM net_usage u LEFT JOIN objects o ON (o.id = u.user_id AND o.type = " . NM_OBJECT_USER . ") 
        WHERE u.date > (SELECT od.date FROM optimize_date od WHERE od.type = \"summary\")
        GROUP BY o.master_id;";

    $sql .= "INSERT OR REPLACE INTO objects_summary SELECT o.master_id, o.type, 
        SUM(rx_size) + ifnull((SELECT s.rx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0), 
        SUM(tx_size) + ifnull((SELECT s.tx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0) 
        FROM net_usage u LEFT JOIN objects o ON (o.id = u.device_id AND o.type = " . NM_OBJECT_DEVICE . ") 
        WHERE u.date > (SELECT od.date FROM optimize_date od WHERE od.type = \"summary\")
        GROUP BY o.master_id;";
   
    $sql .= "INSERT OR REPLACE INTO objects_summary SELECT o.master_id, o.type, 
        SUM(rx_size) + ifnull((SELECT s.rx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0), 
        SUM(tx_size) + ifnull((SELECT s.tx_size FROM objects_summary s WHERE s.master_id = o.master_id AND s.type = o.type), 0) 
        FROM net_usage u LEFT JOIN objects o ON (o.id = u.remote_id AND o.type = " . NM_OBJECT_REMOTE . ") 
        WHERE u.date > (SELECT od.date FROM optimize_date od WHERE od.type = \"summary\")
        GROUP BY o.master_id;";
    
    $sql .= "UPDATE optimize_date SET date = (SELECT date FROM net_usage ORDER BY date DESC LIMIT 1) WHERE type = \"summary\";";
    $sql .= "END TRANSACTION;";
            
    $model = new NM_basic_model();
    $model->query($sql);
    
            return;
        }
        $file = fopen(DAEMON_PC_INFO_FILE, "rb");

        $date = new NM_date();

        $loaded_programs = $loaded_users = $loaded_devices = $loaded_remotes = array();
        $baza = new NM_basic_model();
        $baza->query("DELETE FROM objects");
        while(!feof($file))
        {
            $object = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
            if($object === false)
                break;

            switch($object)
            {
                case NM_OBJECT_DEVICE:
                {
                    $id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

                    $length = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    if(!$length)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $name = NM_fread($file, $length, NM_UNPACK_STRING);

                    $length = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    if(!$length)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $address = NM_fread($file, $length, NM_UNPACK_STRING);

                    $length = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    if(!$length)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $address6 = NM_fread($file, $length, NM_UNPACK_STRING);

                    if($id === false || $name === false || $address === false || $address6 === false)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    if(isset($loaded_devices[$name]))
                    {
                        $old_id = $loaded_devices[$name];
                        $this->_devices[$old_id]->add($id, $name, clone $date, $address, $address6);
                        $sql = "INSERT INTO objects VALUES({$id}, {$old_id}, {$object}, '{$name}', '{$address};{$address6}', '{$date->get_date("H:i:s d-m-Y")}')";
                    }
                    else
                    {
                        $this->_devices[$id] =  new NM_object_device($id, $name, clone $date, $address, $address6);
                        $sql = "INSERT INTO objects VALUES({$id}, {$id}, {$object}, '{$name}', '{$address};{$address6}', '{$date->get_date("H:i:s d-m-Y")}')";
                        $loaded_devices[$name] = $id;
                    }
                    $baza->query($sql);
                }
                break;
                case NM_OBJECT_REMOTE:
                {
                    $id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

                    $length = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    if(!$length)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $name = NM_fread($file, $length, NM_UNPACK_STRING);

                    $length = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    if(!$length)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $address = NM_fread($file, $length, NM_UNPACK_STRING);

                    $length = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    if(!$length)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $address6 = NM_fread($file, $length, NM_UNPACK_STRING);

                    if($id === false || $name === false || $address === false || $address6 === false)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    if(isset($loaded_remotes[$name]))
                    {
                        $old_id = $loaded_remotes[$name];
                        $this->_remotes[$old_id]->add($id, $name, clone $date, $address, $address6);
                        $sql = "INSERT INTO objects VALUES({$id}, {$old_id}, {$object}, '{$name}', '{$address};{$address6}', '{$date->get_date("H:i:s d-m-Y")}')";
                    }
                    else
                    {
                        $this->_remotes[$id] =  new NM_object_remote($id, $name, clone $date, $address, $address6);
                        $loaded_remotes[$name] = $id;
                        $sql = "INSERT INTO objects VALUES({$id}, {$id}, {$object}, '{$name}', '{$address};{$address6}', '{$date->get_date("H:i:s d-m-Y")}')";
                    }
                    $baza->query($sql);
                }
                break;
                case NM_OBJECT_PROGRAM:
                {
                    $id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

                    $length = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    if(!$length)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $name = NM_fread($file, $length, NM_UNPACK_STRING);

                    if($id === false || $name === false)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    if(isset($loaded_programs[$name]))
                    {
                        $old_id = $loaded_programs[$name];
                        $this->_programs[$old_id]->add($id, $name, clone $date);
                        $sql = "INSERT INTO objects VALUES({$id}, {$old_id}, {$object}, '{$name}', '', '{$date->get_date("H:i:s d-m-Y")}')";
                    }
                    else
                    {
                        $this->_programs[$id] =  new NM_object_program($id, $name, clone $date);
                        $loaded_programs[$name] = $id;
                        $sql = "INSERT INTO objects VALUES({$id}, {$id}, {$object}, '{$name}', '', '{$date->get_date("H:i:s d-m-Y")}')";
                    }
                    $baza->query($sql);
                }
                break;
                case NM_OBJECT_USER:
                {
                    $id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

                    $sys_id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

                    $length = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    if(!$length)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $name = NM_fread($file, $length, NM_UNPACK_STRING);

                    if($id === false || $sys_id === false || $name === false)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    if(isset($loaded_users[$name]))
                    {
                        $old_id = $loaded_users[$name];
                        $this->_users[$old_id]->add($id, $name, clone $date, $sys_id);
                        $sql = "INSERT INTO objects VALUES({$id}, {$old_id}, {$object}, '{$name}', '{$sys_id}', '{$date->get_date("H:i:s d-m-Y")}')";
                    }
                    else
                    {
                        $this->_users[$id] =  new NM_object_user($id, $name, clone $date, $sys_id);
                        $loaded_users[$name] = $id;
                        $sql = "INSERT INTO objects VALUES({$id}, {$id}, {$object}, '{$name}', '{$sys_id}', '{$date->get_date("H:i:s d-m-Y")}')";
                    }
                    $baza->query($sql);
                }
                break;
                case NM_DATE:
                {
                    $year = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    $mon = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    $day = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

                    if($year === false || $mon === false || $day === false)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $date->year = $year + 1900;
                    $date->mon = $mon + 1;
                    $date->day = $day;
                }
                break;
                case NM_HOUR:
                {
                    $hour = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    $min = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
                    $sec = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

                    if($hour === false || $min === false || $sec === false)
                        throw new NM_exception(NM_EX_DAEMON_PC_INFO_READ_FAILED, "Unexpected end while reading daemon .pcInfo file", __FILE__, __LINE__);

                    $date->hour = $hour;
                    $date->min = $min;
                    $date->sec = $sec;

                }
                break;
            }
        }
    }
}
