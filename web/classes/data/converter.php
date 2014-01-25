<?php

final class NM_data_converter extends NM_basic_singleton
{
    private $net_usage;
    private $hour_usage;
    private $day_usage;
    private $mon_usage;
    private $year_usage;
    private $total_usage;
    
    private $date;
    
    private $_baza;
    
    protected function __construct()
    {
        $this->net_usage = new stdClass();
        
        $this->net_usage->programs = array();
        
        $this->net_usage->users = array();
        
        $this->net_usage->devices = array();
        
        $this->net_usage->remotes = array();
        
        $this->hour_usage = clone $this->net_usage;
        $this->day_usage = clone $this->net_usage;
        $this->mon_usage = clone $this->net_usage;
        $this->year_usage = clone $this->net_usage;
        $this->total_usage = clone $this->net_usage;
        
        $this->date = new NM_date();
      /*  
        if(file_exists(DATA_DIR . 'converter_time'))
            $this->date->set_time(file_get_contents(DATA_DIR . 'converter_time'));
        else
            $this->date->set_time(0);
        */
        $this->date->set_time(0);
        $this->_baza = new NM_basic_model();
        $this->_baza->query("DELETE FROM net_usage");
    }
    
    public function convert_data()
    {
        //NM_fb::info("Start");
        
        /*if(file_exists(DATA_DIR . '.converter.lock'))
        {
            $time = file_get_contents(DATA_DIR . '.converter.lock');
            $current = new NM_date();
            if($current->get_time() < ($time + (60 * 60 * 10)))
                unlink(DATA_DIR . '.converter.lock');
            else
                return;
        }
        */
        if(!is_dir(DAEMON_DATA_DIR))
            throw new NM_exception(NM_EX_DAEMON_DATA_READ_FAILED, "Daemon directory is invalid", __FILE__, __LINE__);
        $daemon_dir = opendir(DAEMON_DATA_DIR);
        
        $years = array();
        while(($year = readdir($daemon_dir)) !== false)
        {
            if(preg_match('!\d{4}!', $year) && is_dir(DAEMON_DATA_DIR . $year))
            {
                $years[] = $year;
            }
        }
        
        sort($years);
        
        $start_mon = (int)$this->date->mon;
        $start_day = (int)$this->date->day;
        
        foreach($years as $year)
        {
            if($year < $this->date->year)
                continue;
            
            // next year
            if($year > $this->date->year)
            {
                $this->date->year = $year;
                $this->date->mon = 1;
                $start_mon = 1;
                $this->date->day = 1;
                $start_day = 1;
                $this->date->hour = 0;
                $this->date->min = 0;
            }
            
            $year_dir = opendir(DAEMON_DATA_DIR . $year);
            //NM_fb::info($year);
            for($mon = $start_mon; $mon <= 12; $mon++)
            {
              //  NM_fb::info('checking dir: ' . DAEMON_DATA_DIR . $year . DS . $mon);
                if(is_dir(DAEMON_DATA_DIR . $year . DS . $mon))
                {
                    if($mon < $this->date->mon)
                    {
                       // NM_fb::info('skip month');
                        continue;
                    }
                    // next month
                    if($mon > $this->date->mon)
                    {
                        $this->mon = $mon;
                        $this->date->day = 1;
                        $start_day = 1;
                        $this->date->hour = 0;
                        $this->date->min = 0;
                    }
                    //NM_fb::info("{$year} {$mon}");
                    $mon_dir = opendir(DAEMON_DATA_DIR . $year . DS . $mon);
                    
                    $maks_days = NM_date::maks_days_of_month($year, $mon);
                    for($day = $start_day; $day <= $maks_days; $day++)
                    {
                       // NM_fb::info("{$year} {$mon} {$day}");

                        $file = DAEMON_DATA_DIR . $year . DS . $mon . DS . $day;
                        if(file_exists($file))
                        {
                            if($day < $this->date->day)
                            {
                              //  NM_fb::info('skip day');
                                continue;
                            }
                            // next day
                            if($day > $this->date->day)
                            {
                                $this->date->hour = 0;
                                $this->date->min = 0;
                                $this->date->sec = 0;
                            }
                            
                            //NM_fb::info("{$year} {$mon} {$day}");
                            
                            
                            
                            $this->date->year = $year;
                            $this->date->mon = $mon;
                            $this->date->day = $day;

                            try
                            {
                                $this->read_info($file);
                            }
                            catch(NM_exception $e)
                            {
                                // exception throws when last line of file was not save correctly - probably daemon is now operating on file.
                                closedir($mon_dir);
                                closedir($year_dir);
                                break 3;
                            }
                            //$this->save_info();
                            //$this->save_element_info('hour', 'day');
                            //$this->save_element_info('day', 'mon');
                            //$this->save_day_info();
                        }
                    }
                    $start_day = 1;
                    if(is_resource($mon_dir))
                        closedir($mon_dir);
                    //$this->save_mon_info();
                    //$this->save_element_info('mon', 'year');
                }
            }
            $start_mon = 1;
            if(is_resource($year_dir))
                closedir($year_dir);
            //$this->save_year_info();
            //$this->save_element_info('year', 'total');
        }
        closedir($daemon_dir);
        //$this->save_total_info();
        //file_put_contents(DATA_DIR . 'converter_time', $this->date->get_time());
        
        //if(file_exists(DATA_DIR . '.converter.lock'))
          //  unlink(DATA_DIR . '.converter.lock');
        //vde($this->total_usage);
    }
    
    private function read_info($file_path)
    {
       // $current_time = new NM_date();
        //file_put_contents(DATA_DIR . '.converter.lock', $current_time->get_time());
       // unset($current_time);
        
        //$objects = NM_objects::get_instance();
        
        $file = fopen($file_path, "rb");
        $actual_hour = NULL;
        $dane = array();
        while(!feof($file))
        {
            $hour = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
            $min = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
            
            if(feof($file))
                break;
            
            if($hour === false || $min === false)
                throw new NM_exception(NM_EX_DAEMON_DATA_READ_FAILED, "Failed to read data file", __FILE__, __LINE__);

            $program_id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
            $user_id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
            $device_id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
            $remote_id = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

            if($program_id === false || $user_id === false || $device_id === false || $remote_id === false)
                throw new NM_exception(NM_EX_DAEMON_DATA_READ_FAILED, "Failed to read data file", __FILE__, __LINE__);

            if($remote_id == 0)
                $remote_id = -1;
            
            if($program_id == 0 || $device_id == 0)
            {
                // ids must by -1 because user with id 0 is root.
                // so unknown packets will be added to root 
                // in file we can't save -1 value because we save unsigned integer;
                $program_id = -1;
                $user_id = -1;
                $device_id = -1;
            }
            else
            {
                /*$program_id = $objects->find_object($program_id, NM_PROGRAM)->get_main_id();
                $user_id = $objects->find_object($user_id, NM_USER)->get_main_id();
                $device_id = $objects->find_object($device_id, NM_DEVICE)->get_main_id();
                $remote_object = $objects->find_object($remote_id, NM_REMOTE);
                */
                //if(!$remote_object instanceof NM_object_remote)
                  //  $remote_id = -1;
                //else
                  //  $remote_id = $remote_object->get_main_id();
            }
            // we check remote in another if because remote is self-contained from tcp/udp ets. files
            if($remote_id == 0)
                $remote_id = -1;

            $ip_type = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);
            $protocol_type = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

            if($ip_type  === false || $protocol_type === false)
                throw new NM_exception(NM_EX_DAEMON_DATA_READ_FAILED, "Failed to read data file", __FILE__, __LINE__);

            $rx = NM_fread($file, NM_DOUBLE_SIZE, NM_UNPACK_DOUBLE);
            $rx_type = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

            $tx = NM_fread($file, NM_DOUBLE_SIZE, NM_UNPACK_DOUBLE);
            $tx_type = NM_fread($file, NM_UINT_SIZE, NM_UNPACK_NM_UINT);

            if($rx === false || $rx_type === false || $tx === false || $tx_type === false)
                throw new NM_exception(NM_EX_DAEMON_DATA_READ_FAILED, "Failed to read data file", __FILE__, __LINE__);

            if(((int)$hour < (int)$this->date->hour) || ((int)$hour == (int)$this->date->hour && (int)$min < (int)$this->date->min))
            {
                //NM_fb::info($hour . ':' . $min . ' - skip minute');
                //NM_fb::warn($this->date->get_date('H:i:s'));
                continue;
            }
            
           // NM_fb::info($hour . ':' . $min . ' - converting');
            
            $this->date->hour = $hour;
            $this->date->min = $min;
            $this->date->sec = 0;
            
            
            //$ids = array(NM_USER => $user_id, NM_DEVICE => $device_id, NM_REMOTE => $remote_id);
            
            $key = "{$program_id};{$user_id};{$device_id};{$remote_id};{$this->date->get_time()}";
            
            $rx = $rx * pow(1024, $rx_type - 1);
            $tx = $tx * pow(1024, $tx_type - 1);
            
            if(!array_key_exists($key, $dane))
                $dane[$key] = array('rx' => 0, 'tx' => 0);
            
            $dane[$key]['rx'] += $rx;
            $dane[$key]['tx'] += $tx;
            
            
            /*
            $this->add_to_size(NM_PROGRAM, $program_id, $rx, $tx, $ids);
            
            unset($ids[NM_USER]);
            $ids[NM_PROGRAM] = $program_id;
            $this->add_to_size(NM_USER, $user_id, $rx, $tx, $ids);
            
            unset($ids[NM_DEVICE]);
            $ids[NM_USER] = $user_id;
            $this->add_to_size(NM_DEVICE, $device_id, $rx, $tx, $ids);*/
            /*
            unset($ids[NM_REMOTE]);
            $ids[NM_DEVICE] = $device_id;
            $this->add_to_size(NM_REMOTE, $remote_id, $rx, $tx, $ids);
           */
            
            if(is_null($actual_hour))
            {
                $actual_hour = $hour;
                continue;
            }
            
            // next hour
            /*if($actual_hour != $hour)
            {
                //$this->save_element_info('hour', 'day');
                $actual_hour = $hour;
                
                $this->date->hour = $hour;
                $this->date->min = 0;
                $this->date->sec = 0;
            }*/
        }
        
        $sql = "BEGIN TRANSACTION; ";
        
        $i = 1;
        
        foreach($dane as $key => $row)
        {
            $rx = (int)$row['rx'];
            $tx = (int)$row['tx'];
            if($rx == 0 && $tx ==  0)
                continue;
            
            list($program_id,$user_id,$device_id,$remote_id,$time) = explode(";", $key);
            $sql .= "INSERT INTO net_usage VALUES ({$time},{$program_id},{$user_id},{$device_id},{$remote_id},{$rx},{$tx}); ";
            if(($i % 25) == 0)
            {
                $i = 1;
                $sql .= "END TRANSACTION;";
                $this->_baza->query($sql);
                unset($sql);
                $sql = "BEGIN TRANSACTION; ";
            }
            $i++;
        }
        $sql .= "END TRANSACTION;";
        $this->_baza->query($sql);
        
        header("Data pliku: {$this->date->hour}:{$this->date->min} {$this->date->day} {$this->date->mon} {$this->date->year}");
    }
    
    private function add_to_size($object_type, $id, $rx, $tx, $ids)
    {
        $object = "{$object_type}s";
        $element =& $this->net_usage->$object;
        
        $hour_element =& $this->hour_usage->$object;
        /*$day_element =& $this->day_usage->$object;
        $mon_element =& $this->mon_usage->$object;
        $year_element =& $this->year_usage->$object;
        $total_element =& $this->total_usage->$object;
        */
        $hour = $this->date->hour;
        $min = $this->date->min;
        
        if(!isset($element[$id]))
            $element[$id] = array($hour => array($min => $this->get_information_structure()));
        else if(!isset($element[$id][$hour]))
            $element[$id][$hour] = array($min => $this->get_information_structure());
        else if(!isset($element[$id][$hour][$min]))
            $element[$id][$hour][$min] = $this->get_information_structure();
        
        $year = $this->date->year;
        $mon = $year . DS . $this->date->mon;
        $day = $mon . DS . $this->date->day;
        $hour_key = $day . DS . $hour;
        
        if(!isset($hour_element[$id]))
        {
            $hour_element[$id] = array($hour_key => $this->get_information_structure());
        }
        
        if(!isset($hour_element[$id][$hour_key]))
        {
            $hour_element[$id][$hour_key] = $this->get_information_structure();
        }
        
        $element[$id][$hour][$min]->rx += $rx;
        $element[$id][$hour][$min]->tx += $tx;
        
        $element_ids =& $element[$id][$hour][$min]->ids;
        
        $hour_element[$id][$hour_key]->rx += $rx;
        $hour_element[$id][$hour_key]->tx += $tx;
        
        foreach($ids as $compare_object => $compare_id)
        {
            if(!isset($element_ids[$compare_object]))
                $element_ids[$compare_object] = array($compare_id => $this->get_information_structure(false));
            else if(!isset($element_ids[$compare_object][$compare_id]))
                $element_ids[$compare_object][$compare_id] = $this->get_information_structure(false);
            
            $element_ids[$compare_object][$compare_id]->rx += $rx;
            $element_ids[$compare_object][$compare_id]->tx += $tx;
            
            if(!isset($hour_element[$id][$hour_key]->ids[$compare_object]))
                $hour_element[$id][$hour_key]->ids[$compare_object] = array($compare_id => $this->get_information_structure(false));
            else if(!isset($hour_element[$id][$hour_key]->ids[$compare_object][$compare_id]))
                $hour_element[$id][$hour_key]->ids[$compare_object][$compare_id] = $this->get_information_structure(false);
            
            $hour_element[$id][$hour_key]->ids[$compare_object][$compare_id]->rx += $rx;
            $hour_element[$id][$hour_key]->ids[$compare_object][$compare_id]->tx += $tx;
        }
    }
    
    private function get_information_structure($ids = true)
    {
        $return = new stdClass();
        $return->rx = 0.0;
        $return->tx = 0.0;
        
        if($ids)
            $return->ids = array();
        
        return $return;
    }
    
    private function save_info()
    {
        if(!is_dir(DATA_PROGRAMS_DIR))
            mkdir(DATA_PROGRAMS_DIR, 0777, true);
        if(!is_dir(DATA_USERS_DIR))
            mkdir(DATA_USERS_DIR, 0777, true);
        if(!is_dir(DATA_DEVICES_DIR))
            mkdir(DATA_DEVICES_DIR, 0777, true);
        
        $objects = NM_objects::get_instance();
        
        foreach($this->net_usage->programs as $id => $program)
        {
            $main_id = $objects->find_object($id, NM_PROGRAM)->get_main_id();
            $this->save_info_to_file(DATA_PROGRAMS_DIR, $program, $main_id);
        }
        
        $this->net_usage->programs = array();
        
        foreach($this->net_usage->users as $id => $user)
        {
            $main_id = $objects->find_object($id, NM_USER)->get_main_id();
            $this->save_info_to_file(DATA_USERS_DIR, $user, $main_id);
        }
        
        $this->net_usage->users = array();
        
        foreach($this->net_usage->devices as $id => $device)
        {
            $main_id = $objects->find_object($id, NM_DEVICE)->get_main_id();
            $this->save_info_to_file(DATA_DEVICES_DIR, $device, $main_id);
        }
        
        $this->net_usage->devices = array();
        /*
        foreach($this->net_usage->remotes as $id => $remote)
        {
            $main_id = $objects->find_object($id, NM_REMOTE)->get_main_id();
            $this->save_info_to_file(DATA_REMOTES_DIR, $remote, $main_id);
        }
        
        $this->net_usage->remotes = array();
         */
    }
    
    private function save_info_to_file($dir, $element, $id)
    {
        $id_dir = $dir . $id;
        $day_dir = $id_dir . DS . $this->date->year . DS . $this->date->mon . DS . $this->date->day;
        if(!is_dir($day_dir))
            mkdir($day_dir, 0777, true);
        $file = $day_dir . DS . 'info';
        
        if(file_exists($file))
        {
            $content = file_get_contents($file);
            if($content)
            {
                $saved_data = unserialize($content);
                if($saved_data)
                {
                    foreach($element as $hour => $hour_info)
                    {
                        if(isset($saved_data[$hour]))
                        {
                            foreach($hour_info as $min => $min_info)
                            {
                                if(isset($saved_data[$hour][$min]))
                                {
                                    $saved_data[$hour][$min]->rx += $min_info->rx;
                                    $saved_data[$hour][$min]->tx += $min_info->tx;

                                    $saved_data[$hour][$min]->ids = $this->add_ids($saved_data[$hour][$min]->ids, $min_info->ids);
                                }
                                else
                                    $saved_data[$hour][$min] = $min_info;
                            }
                        }
                        else
                            $saved_data[$hour] = $hour_info;
                    }
                    $element = $saved_data;
                }
            }
        }
        
        file_put_contents($file, serialize($element));
    }
    
    private function save_element_info($part, $parent_part)
    {
        $objects = array('programs' => DATA_PROGRAMS_DIR, 'users' => DATA_USERS_DIR, 'devices' => DATA_DEVICES_DIR);
        
        $part_info = $part . '_usage';
        $parent_part_info = $parent_part . '_usage';
        $part_element = &$this->$part_info;
        foreach($objects as $object => $dir)
        {
            foreach($part_element->$object as $id => $element)
            {
                $object_dir = $dir . $id . DS;
                
                $keys = array_keys($element);
                
                $key = explode(DS, $keys[0]);
                
                unset($key[count($key) - 1]);
                $d2 = implode(DS, $key);
                unset($keys);
                unset($key);
                
                if(!is_dir($object_dir . $d2))
                    mkdir($object_dir . $d2, 0777, true);
                $parent =& $this->$parent_part_info;
                $parent_element =& $parent->$object;
                
                foreach($element as $key => $vals)
                {
                    if($part != 'hour')
                    {
                        $file = $object_dir. $key . DS . $part . '_info';
                    }
                    else
                        $file = $object_dir . $key . '_'. $part;
                    
                    $parent_key = substr($key, 0, strrpos($key, DS));
                    
                    if(!isset($parent_element[$id]))
                    {
                        $parent_element[$id] = array($parent_key => $this->get_information_structure());
                        $parent_element[$id][$parent_key]->rx = $vals->rx;
                        $parent_element[$id][$parent_key]->tx = $vals->tx;
                    }
                    else if(!isset($parent_element[$id][$parent_key]))
                    {
                        $parent_element[$id][$parent_key] = $this->get_information_structure();
                        $parent_element[$id][$parent_key]->rx = $vals->rx;
                        $parent_element[$id][$parent_key]->tx = $vals->tx;
                    }
                    else
                    {
                        $parent_element[$id][$parent_key]->rx += $vals->rx;
                        $parent_element[$id][$parent_key]->tx += $vals->tx;
                    }
                    
                    $parent_element[$id][$parent_key]->ids = $this->add_ids($parent_element[$id][$parent_key]->ids, $vals->ids);
                    
                    if(file_exists($file))
                    {
                        $content = unserialize(file_get_contents($file));
                        $vals->rx += $content->rx;
                        $vals->tx += $content->tx;
                        
                        $vals->ids = $this->add_ids($vals->ids, $content->ids);
                    }

                    file_put_contents($file, serialize($vals));
                }
            }
            $part_element->$object = array();
        }
        
        file_put_contents(DATA_DIR . 'converter_time', $this->date->get_time());
    }
    
    private function save_total_info()
    {
        $objects = array('programs' => DATA_PROGRAMS_DIR, 'users' => DATA_USERS_DIR, 'devices' => DATA_DEVICES_DIR);
        
        foreach($objects as $object => $dir)
        {
            foreach($this->total_usage->$object as $id => $vals)
            {
                $vals = $vals[''];
                $file = $dir . $id . DS . 'total_info';
                if(file_exists($file))
                {
                    $content = unserialize(file_get_contents($file));
                    
                    $vals->rx += $content->rx;
                    $vals->tx += $content->tx;
                    
                    $vals->ids = $this->add_ids($vals->ids, $content->ids);
                }
                file_put_contents($file, serialize($vals));
            }
        }
    }
    
    private function add_ids($ids, $add)
    {
        foreach($add as $object_type => $object_data)
        {
            foreach($object_data as $object_id => $object_vals)
            {
                if(!isset($ids[$object_type]))
                    $ids[$object_type] = array($object_id => $this->get_information_structure(false));
                else if(!isset($ids[$object_type][$object_id]))
                    $ids[$object_type][$object_id] = $this->get_information_structure(false);
                
                $ids[$object_type][$object_id]->rx += $object_vals->rx;
                $ids[$object_type][$object_id]->tx += $object_vals->tx;
            }
        }
        return $ids;
    }
}
