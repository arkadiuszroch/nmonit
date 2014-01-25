<?php

class NM_model_objects extends NM_basic_model
{
    public function get_objects_count()
    {
        $sql = "SELECT COUNT(*) as count FROM objects";
        return $this->query($sql);
    }
    
    public function get_objects()
    {
        $sql = "SELECT * FROM objects";
        return $this->query($sql);
    }
    
    public function get_object_info($master_id, $type)
    {
        $sql = 'SELECT * FROM objects WHERE master_id = ' . $master_id .' AND type = ' . $type;
        return $this->query($sql);
    }
    
    public function get_history($master_id, $type)
    {
        $sql = 'SELECT * FROM objects WHERE id != master_id master_id = ' . $master_id . ' AND type = ' . $type;
        return $this->query($sql);
    }
    
    public function get_used_objects($limit = 0, $order_by_size = false, $desc = false, $object_type = false)
    {
        $limit = ($limit) ? ' LIMIT ' . $limit . ' ' : '';
        $order = '';
        if($order_by_size)
        {
            $order = ' ORDER BY SUM(s.rx_size + s.tx_size) ';
        }
        else
        {
            $order = ' ORDER BY o.name ';
        }
        if($order && $desc)
            $order .= 'DESC ';
        
        $programs = $users = $devices = $remotes = array();
        
        if(!$object_type || $object_type == NM_PROGRAMS)
        {
            $sql = 'SELECT DISTINCT o.master_id, o.name FROM objects_summary s LEFT JOIN objects o ON (o.master_id = s.master_id AND o.type = s.type) WHERE s.type = ' . NM_OBJECT_PROGRAM . ' AND o.master_id IS NOT NULL GROUP BY o.master_id' . $order . $limit;
            $programs = $this->query($sql);
        }
        
        if(!$object_type || $object_type == NM_USERS)
        {
            $sql = 'SELECT DISTINCT o.master_id, o.name, o.info as sys_id FROM objects_summary s LEFT JOIN objects o ON (o.master_id = s.master_id AND o.type = s.type) WHERE s.type = ' . NM_OBJECT_USER . ' AND o.master_id IS NOT NULL GROUP BY o.master_id' . $order . $limit;
            $users = $this->query($sql);
        }
        
        if(!$object_type || $object_type == NM_DEVICES)
        {
            $sql = 'SELECT DISTINCT o.master_id, o.* FROM objects_summary s LEFT JOIN objects o ON (o.master_id = s.master_id AND o.type = s.type) WHERE s.type = ' . NM_OBJECT_DEVICE . ' AND o.master_id IS NOT NULL GROUP BY o.master_id' . $order . $limit;
            $result = $this->query($sql);
            foreach($result as $row)
            {
                list($address, $address6) = explode(';', $row['info']);
                
                unset($row['info']);
                $row['address'] = $address;
                $row['address6'] = $address6;
                $devices[] = $row;
            }
        }
        
        if(!$object_type || $object_type == NM_REMOTES)
        {
            $sql = 'SELECT DISTINCT o.master_id, o.* FROM objects_summary s LEFT JOIN objects o ON (o.master_id = s.master_id AND o.type = s.type) WHERE s.type = ' . NM_OBJECT_REMOTE . ' AND o.master_id IS NOT NULL GROUP BY o.master_id' . $order . $limit;
            $result = $this->query($sql);
            foreach($result as $row)
            {
                list($address, $address6) = explode(';', $row['info']);
                
                unset($row['info']);
                $row['address'] = $address;
                $row['address6'] = $address6;
                $remotes[] = $row;
            }
        }
        
        return array(NM_PROGRAMS => $programs, NM_USERS => $users, NM_DEVICES => $devices, NM_REMOTES => $remotes);
    }
    
    public function get_down_up_info($id, $type)
    {
        $object_type = $this->object_name_to_type($type);
        
        $id = intval($id);
        $sql = "SELECT rx_size as download, tx_size as upload FROM objects_summary s WHERE s.master_id = {$id} AND s.type = {$object_type} ";
        $info = $this->query($sql);
        $info = $info[0];
        $result = array();
        $size = $info['download'];
        $size_type = NM_SIZE_B; 
        standarize_size($size, $size_type);
        $result[] = array('Download ' . $size . ' ' . size_type_to_string($size_type), $info['download']);
        
        $size = $info['upload'];
        $size_type = NM_SIZE_B; 
        standarize_size($size, $size_type);
        $result[] = array('Upload ' . $size . ' ' . size_type_to_string($size_type), $info['upload']);
        
        return $result;
    }
    
    /**
     * @param string $object
     * @param int $id
     * @param string $format
     * @param DateInterval $offset
     * @param NM_date $start_date
     * @param NM_date $end_date
     * @return boolean
     */
    public function network_usage($object, $id, $format, $offset, $start_date, $end_date)
    {
        if(!in_array($object, array(NM_PROGRAM, NM_USER, NM_DEVICE, NM_REMOTE)))
            return false;
        
        $type = intval($this->object_name_to_type($object));
        $id = intval($id);
        
        $sql = 'SELECT SUM(rx_size) as rx, SUM(tx_size) as tx, date FROM net_usage WHERE ' . $object . '_id IN (SELECT id FROM objects WHERE master_id = ' . $id . ' AND type = ' . $type . ') AND date BETWEEN ' . $start_date->get_time() . ' AND ' .$end_date->get_time() .' GROUP BY date';
        $result = $this->query($sql);
        $time_to_key = array();
        $data = array();
        $max_rx = $max_tx = 0;
        
        foreach($result as $row)
        {
            if(!array_key_exists($row['date'], $time_to_key))
                $time_to_key[$row['date']] = date($format, $row['date']);
            
            $key = $time_to_key[$row['date']];
            if(!array_key_exists($key, $data))
                $data[$key] = array('rx' => 0, 'tx' => 0);
            
            $data[$key]['rx'] += $row['rx'];
            $data[$key]['tx'] += $row['tx'];
            
            if($max_rx < $data[$key]['rx'])
                $max_rx = $data[$key]['rx'];
            
            if($max_tx < $data[$key]['tx'])
                $max_tx = $data[$key]['tx'];
        }
        
        $max_size = ($max_rx < $max_tx) ? $max_tx : $max_rx;

        $max_size_rx = $max_size_tx = $max_size_type = NM_SIZE_B;
        
        standarize_size($max_rx, $max_size_rx);
        standarize_size($max_tx, $max_size_tx);
        standarize_size($max_size, $max_size_type);

        $return = array('rx' => array(), 'tx' => array());
        
        // pętla jest po dacie ponieważ musimy uzupełnić puste dane
        while($start_date->get_time() <= $end_date->get_time())
        {
            $key = $start_date->get_date($format);
            if(array_key_exists($key, $data))
            {
                $return['rx'][] = array($key, round($data[$key]['rx'] / pow(1024, $max_size_type - 1), 2));
                $return['tx'][] = array($key, round($data[$key]['tx'] / pow(1024, $max_size_type - 1), 2));
            }
            else
            {
                $return['rx'][] = array($key, 0);
                $return['tx'][] = array($key, 0);
            }
            
            $start_date->add_interval($offset);
        }
        $size_type_diff = array('rx', 'tx');
        $size_type_diff['rx'] = $max_size_type - $max_size_rx;
        $size_type_diff['tx'] = $max_size_type - $max_size_tx;
        
        return array('rx' => $return['rx'],
                     'tx' => $return['tx'],
                     'max_size' => ceil(1.2 * $max_size), 'size_type' => $max_size_type,
                     'max_size_rx' => ceil(1.2 * $max_rx), 'size_type_rx' => $max_size_rx,
                     'max_size_tx' => ceil(1.2 * $max_tx), 'size_type_tx' => $max_size_tx,
                     'size_type_diff' => $size_type_diff);
    }
    
    public function get_compare_data($object, $id)
    {
        
        $return_chart = array('rx' => array(), 'tx' => array());
        $return_colors = array('rx' => array(), 'tx' => array());
        $return_data = array();
        
        if($object != NM_PROGRAM)
        {
            $colors = get_charts_colors();
            $colors_set = array();
        
            $rx = $this->get_compare($object, $id, 'rx', NM_PROGRAM, $colors, $colors_set);
            $tx = $this->get_compare($object, $id, 'tx', NM_PROGRAM, $colors, $colors_set);
            $return_chart['rx'][NM_PROGRAM] = $rx['chart'];
            $return_chart['tx'][NM_PROGRAM] = $tx['chart'];
            $return_colors['rx'][NM_PROGRAM] = $rx['colors'];
            $return_colors['tx'][NM_PROGRAM] = $tx['colors'];
            $return_data[NM_PROGRAM]['rx'] = $rx['data'];
            $return_data[NM_PROGRAM]['tx'] = $tx['data'];
        }

        if($object != NM_USER)
        {
            $colors = get_charts_colors();
            $colors_set = array();
        
            $rx = $this->get_compare($object, $id, 'rx', NM_USER, $colors, $colors_set);
            $tx = $this->get_compare($object, $id, 'tx', NM_USER, $colors, $colors_set);
            $return_chart['rx'][NM_USER] = $rx['chart'];
            $return_chart['tx'][NM_USER] = $tx['chart'];
            $return_colors['rx'][NM_USER] = $rx['colors'];
            $return_colors['tx'][NM_USER] = $tx['colors'];
            $return_data[NM_USER]['rx'] = $rx['data'];
            $return_data[NM_USER]['tx'] = $tx['data'];
        }

        if($object != NM_DEVICE)
        {
            $colors = get_charts_colors();
            $colors_set = array();
        
            $rx = $this->get_compare($object, $id, 'rx', NM_DEVICE, $colors, $colors_set);
            $tx = $this->get_compare($object, $id, 'tx', NM_DEVICE, $colors, $colors_set);
            $return_chart['rx'][NM_DEVICE] = $rx['chart'];
            $return_chart['tx'][NM_DEVICE] = $tx['chart'];
            $return_colors['rx'][NM_DEVICE] = $rx['colors'];
            $return_colors['tx'][NM_DEVICE] = $tx['colors'];
            $return_data[NM_DEVICE]['rx'] = $rx['data'];
            $return_data[NM_DEVICE]['tx'] = $tx['data'];
        }
        
        if($object != NM_REMOTE)
        {
            $colors = get_charts_colors();
            $colors_set = array();
        
            $rx = $this->get_compare($object, $id, 'rx', NM_REMOTE, $colors, $colors_set);
            $tx = $this->get_compare($object, $id, 'tx', NM_REMOTE, $colors, $colors_set);
            $return_chart['rx'][NM_REMOTE] = $rx['chart'];
            $return_chart['tx'][NM_REMOTE] = $tx['chart'];
            $return_colors['rx'][NM_REMOTE] = $rx['colors'];
            $return_colors['tx'][NM_REMOTE] = $tx['colors'];
            $return_data[NM_REMOTE]['rx'] = $rx['data'];
            $return_data[NM_REMOTE]['tx'] = $tx['data'];
        }
        
        return array('charts' => $return_chart, 'colors' => $return_colors, 'result' => $return_data);
    }
    
    private function object_name_to_type($type)
    {
        if($type == NM_PROGRAM)
            return NM_OBJECT_PROGRAM;
        else if($type == NM_USER)
            return NM_OBJECT_USER;
        else if($type == NM_DEVICE)
            return NM_OBJECT_DEVICE;
        else if($type == NM_REMOTE)
            return NM_OBJECT_REMOTE;
        return false;
    }
    
    private function object_type_to_name($name)
    {
        if($name == NM_OBJECT_PROGRAM)
            return NM_PROGRAM;
        else if($name == NM_OBJECT_USER)
            return NM_USER;
        else if($name == NM_OBJECT_DEVICE)
            return NM_DEVICE;
        else if($name == NM_OBJECT_REMOTE)
            return NM_REMOTE;
        return false;
    }
    
    private function get_compare($object, $id, $type, $compare_object, &$colors, &$colors_used)
    {
        if(!in_array($type, array('rx', 'tx')))
            return array();
        
        $object_type = $this->object_name_to_type($object);
        if(!$object_type)
            return array();
        
        $compare_object_type = $this->object_name_to_type($compare_object);
        if(!$compare_object_type)
            return array();
        
        if(($object_type == NM_OBJECT_USER && $compare_object_type == NM_OBJECT_PROGRAM)
            || (($object_type == NM_OBJECT_DEVICE && $compare_object_type == NM_OBJECT_PROGRAM) || ($object_type == NM_OBJECT_DEVICE && $compare_object_type == NM_OBJECT_USER))
            || (($object_type == NM_OBJECT_REMOTE && $compare_object_type == NM_OBJECT_PROGRAM) || ($object_type == NM_OBJECT_REMOTE && $compare_object_type == NM_OBJECT_USER) || ($object_type == NM_OBJECT_REMOTE && $compare_object_type == NM_OBJECT_DEVICE)))
            $sql = "SELECT o.master_id, o.name, c.{$type}_size as size FROM objects_compare c LEFT JOIN objects o ON (o.master_id = c.master_id AND o.type = c.type) WHERE c.compare_master_id = {$id} AND c.compare_type = {$object_type} AND c.type = {$compare_object_type} GROUP BY o.master_id ORDER BY c.{$type}_size DESC ";
        else
            $sql = "SELECT o.master_id, o.name, c.{$type}_size as size FROM objects_compare c LEFT JOIN objects o ON (o.master_id = c.compare_master_id AND o.type = c.compare_type) WHERE c.master_id = {$id} AND c.type = {$object_type} AND c.compare_type = {$compare_object_type} GROUP BY o.master_id ORDER BY c.{$type}_size DESC ";
        //$sql = 'SELECT SUM(' . $type . '_size) as size, o.master_id, o.name FROM net_usage u LEFT JOIN objects o ON (o.id = u.' . $compare_object . '_id AND type = ' . $compare_object_type . ') WHERE u.' . $object . '_id IN (SELECT id FROM objects WHERE master_id = ' . $id . ' AND type = '. $object_type .') GROUP BY o.master_id ORDER BY SUM(' . $type . '_size) DESC;';
        //$sql = 'SELECT o.master_id as id, o.name, SUM(u.' . $type . '_size) as size FROM net_usage u LEFT JOIN objects o ON (o.id = u.' . $object . '_id AND o.type = ' . $object_type . ') WHERE ' . $compare_object . '_id IN () GROUP BY o.master_id ORDER BY SUM(u.' . $type . '_size) DESC';
        $result = $this->query($sql);
        $return = array();
        $return_colors = array();
        $chart = array();
        $i = 0;
        
        foreach($result as $row)
        {
            $name = ($row['name']) ?: 'Unknown';
            $size_type = NM_SIZE_B;
            $size = $row['size'];
            standarize_size($size, $size_type);
            $return[] = array('name' => $name, 'value' => sprintf('%.2f', $size) . ' ' . size_type_to_string($size_type));
            
            if($i >= 7 && count($result) > 8)
            {
                if(!array_key_exists('others', $colors_used))
                {
                    $colors_used['others'] = array_shift($colors);
                    $return_colors[7] = $colors_used['others'];
                }
                
                if(!array_key_exists(7, $return_colors))
                    $return_colors[7] = $colors_used['others'];
                
                if(!array_key_exists(7, $chart))
                    $chart[7] = array('Others', 0);
                $chart[7][1] += $row['size'];
            }
            else
            {
                if(!array_key_exists($row['master_id'], $colors_used))
                    $colors_used[$row['master_id']] = array_shift($colors);
                $return_colors[] = $colors_used[$row['master_id']];
                
                $chart[] = array($name, $row['size']);
            }
            
            $i++;
        }
        return array ('data' => $return, 'chart' => $chart, 'colors' => $return_colors);
    }
}