<?php

class NM_model_index extends NM_basic_model
{
    public function check_data()
    {
        $sql = "SELECT date FROM net_usage LIMIT 1";
        return (count($this->query($sql)) > 0);
    }
    
    public function get_programs_summary()
    {
        $colors = get_charts_colors();
        $colors_set = array();
        
        $rx = $this->get_summary(NM_PROGRAM, 'rx', $colors, $colors_set);
        $tx = $this->get_summary(NM_PROGRAM, 'tx', $colors, $colors_set);
        
        return array('rx' => $rx, 'tx' => $tx);
    }
    
    public function get_users_summary()
    {
        $colors = get_charts_colors();
        $colors_set = array();
        $rx = $this->get_summary(NM_USER, 'rx', $colors, $colors_set);
        $tx = $this->get_summary(NM_USER, 'tx', $colors, $colors_set);
        
        return array('rx' => $rx, 'tx' => $tx);
    }
    
    public function get_devices_summary()
    {
        $colors = get_charts_colors();
        $colors_set = array();
        $rx = $this->get_summary(NM_DEVICE, 'rx', $colors, $colors_set);
        $tx = $this->get_summary(NM_DEVICE, 'tx', $colors, $colors_set);
        
        return array('rx' => $rx, 'tx' => $tx);
    }
    
    
    private function get_summary($object, $type, &$colors, &$colors_used)
    {
        if(!in_array($type, array('rx', 'tx')))
            return array();
        
        if($object == NM_PROGRAM)
            $object_type = NM_OBJECT_PROGRAM;
        else if($object == NM_USER)
            $object_type = NM_OBJECT_USER;
        else if($object == NM_DEVICE)
            $object_type = NM_OBJECT_DEVICE;
        else if($object == NM_REMOTE)
            $object_type = NM_OBJECT_REMOTE;
        
        $sql = "SELECT o.master_id as id, o.name, s.{$type}_size as size FROM objects_summary s LEFT JOIN objects o ON (o.master_id = s.master_id AND o.type = s.type) WHERE s.type = {$object_type} GROUP BY o.master_id ORDER BY s.{$type}_size DESC ";
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
                if(!array_key_exists($row['id'], $colors_used))
                    $colors_used[$row['id']] = array_shift($colors);
                $return_colors[] = $colors_used[$row['id']];
                
                $chart[] = array($name, $row['size']);
            }
            
            $i++;
        }
        return array('data' => $return, 'chart' => $chart, 'colors' => $return_colors);
    }
}
