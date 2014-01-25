<?php

class NM_data_loader extends NM_basic_singleton
{
    public function compare($object_type, $id)
    {
        $cache = NM_cache::get_instance('loader');
        $cache_key = 'NM_loader::compare_' . $object_type . '_' . $id;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $object_path = get_object_path($object_type);
        
        $object_path .= $id . DS;
        
        $total_info_file = $object_path . 'total_info';
        
        if(!file_exists($total_info_file))
            throw new NM_exception(NM_EX_FILE_DOESNT_EXISTS, "File: {$total_info_file} doesn't exists", __FILE__, __LINE__);
            
        $content = file_get_contents($total_info_file);
        
        $data = unserialize($content);
        unset($content);
        
        $compare_data = $data->ids;
        unset($data);
        
        $result = array();
        
        $chart_result = array('rx' => array(), 'tx' => array());
        
        $objects = NM_objects::get_instance();
        
        $objects_colors = array('rx' => array(), 'tx' => array());
        
        foreach($compare_data as $compare_object => $object_data)
        {
            $result[$compare_object] = array('rx' => array(), 'tx' => array());
            
            $colors = get_charts_colors();
            
            $objects_colors['rx'][$compare_object] = array();
            $objects_colors['tx'][$compare_object] = array();
            
            $colors_set = array();
            for($i = 0; count($object_data) > $i; $i++)
            {
                $max_id = array('rx' => -2, 'tx' => -2);
                $max_val = array('rx' => -2, 'tx' => -2);
                
                foreach($object_data as $compare_id => $compare_values)
                {
                    if($max_val['rx'] < $compare_values->rx && !isset($result[$compare_object]['rx'][$compare_id]))
                    {
                        $max_val['rx'] = $compare_values->rx;
                        $max_id['rx'] = $compare_id;
                    }
                    if($max_val['tx'] < $compare_values->tx && !isset($result[$compare_object]['tx'][$compare_id]))
                    {
                        $max_val['tx'] = $compare_values->tx;
                        $max_id['tx'] = $compare_id;
                    }
                }
                
                $name_rx = $objects->find_object($max_id['rx'], $compare_object)->get_name();
                $name_tx = $objects->find_object($max_id['tx'], $compare_object)->get_name();
                
                $size_type_rx = NM_SIZE_B;
                $size_type_tx = NM_SIZE_B;
                $size_rx = $max_val['rx'];
                $size_tx = $max_val['tx'];
                standarize_size($size_rx, $size_type_rx);
                standarize_size($size_tx, $size_type_tx);

                $result[$compare_object]['rx'][$max_id['rx']] = array('name' => $name_rx, 'value' => $size_rx . ' ' . size_type_to_string($size_type_rx));
                $result[$compare_object]['tx'][$max_id['tx']] = array('name' => $name_tx, 'value' => $size_tx . ' ' . size_type_to_string($size_type_tx));
                
                if($i < 8)
                {
                    if(!isset($colors_set[$max_id['rx']]))
                        $colors_set[$max_id['rx']] = array_shift($colors);
                    if(!isset($colors_set[$max_id['tx']]))
                        $colors_set[$max_id['tx']] = array_shift($colors);
                    
                    $objects_colors['rx'][$compare_object][] = $colors_set[$max_id['rx']];
                    $objects_colors['tx'][$compare_object][] = $colors_set[$max_id['tx']];
                    $chart_result['rx'][$compare_object][] = array("$name_rx $size_rx " . size_type_to_string($size_type_rx), $max_val['rx']);
                    $chart_result['tx'][$compare_object][] = array("$name_tx $size_tx " . size_type_to_string($size_type_tx), $max_val['tx']);
                }
                else if($i == 8)
                {
                    $color = array_shift($colors);
                    $objects_colors['rx'][$compare_object][] = $color;
                    $objects_colors['tx'][$compare_object][] = $color;
                    $chart_result['rx'][$compare_object][8] = array('Others', $max_val['rx']);
                    $chart_result['tx'][$compare_object][8] = array('Others', $max_val['tx']);
                }
                else if($i > 8)
                {
                    $chart_result['rx'][$compare_object][8][1] += $max_val['rx'];
                    $chart_result['tx'][$compare_object][8][1] += $max_val['tx'];
                }
            }
            
            if(isset($chart_result['rx'][$compare_object][8]))
            {
                $size = $chart_result['rx'][$compare_object][8][1];
                $type = NM_SIZE_B;
                standarize_size($size, $type);
                $chart_result['rx'][$compare_object][8][0] = "Others {$size} " . size_type_to_string($type);

                $size = $chart_result['tx'][$compare_object][8][1];
                $type = NM_SIZE_B;
                standarize_size($size, $type);
                $chart_result['tx'][$compare_object][8][0] = "Others {$size} " . size_type_to_string($type);
            }
            unset($size);
            unset($type);
        }
        
        $return = array('result' => $result, 'charts' => $chart_result, 'colors' => $objects_colors);
        $cache->set($cache_key, $return);
        return $return;
    }
    
    public function summary($object)
    {
        $cache = NM_cache::get_instance('loader');
        
        $cache_key = 'NM_loader::summary_' . $object;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $object_path = get_object_path($object);
        
        if(!is_dir($object_path))
            return array();
        
        $summary_file = DATA_DIR . 'summary_'. $object;
        $summary_info = array();
        if(file_exists($summary_file))
        {
            $content = file_get_contents($summary_file);
            if($content)
            {
                $data = unserialize($content);
                if($data)
                {
                    $timestamp = $data->time;
                    $summary_info = $data->summary_info;
                    
                    if(file_exists(DATA_DIR . 'converter_time'))
                    {
                        $converter_time = file_get_contents(DATA_DIR . 'converter_time');
                        if($converter_time < $timestamp)
                        {
                            $cache->set($cache_key, $summary_info);
                            return $summary_info;
                        }
                    }
                }
            }
        }
        
        $objects_dir = opendir($object_path);
        
        while(($id = readdir($objects_dir)) !== false)
        {
            if(preg_match('!^-?\d+$!', $id))
            {
                $total_file = $object_path . $id . DS . 'total_info';
                
                if(!file_exists($total_file))
                    throw new NM_exception(NM_EX_FILE_DOESNT_EXISTS, "File doesn't exists: {$total_file}", __FILE__, __LINE__);
                
                $total_info = file_get_contents($total_file);
                
                if(!$total_info)
                    throw new NM_exception(NM_EX_DATA_READ_FAILED, "Failed read file: {$total_file}", __FILE__, __LINE__);
                
                $info = unserialize($total_info);
                
                if(!$info)
                    throw new NM_exception(NM_EX_UNSERIALIZE_FAILED, 'Unserialize failed', __FILE__, __LINE__);
                
                $summary_info[$id] = new stdClass();
                
                $summary_info[$id]->rx = $info->rx;
                $summary_info[$id]->rx_type = NM_SIZE_B;
                
                $summary_info[$id]->tx = $info->tx;
                $summary_info[$id]->tx_type = NM_SIZE_B;
            }
        }
        
        $date = new NM_date();
        
        $summary = new stdClass();
        $summary->time = $date->get_time();
        $summary->summary_info = $summary_info;
        
        file_put_contents($summary_file, serialize($summary));
        
        $data = $this->summary_calculate($summary_info, $object);
        
        $cache->set($cache_key, $data);
        
        return $data;
    }
    
    public function summary_calculate($object_data, $object_type)
    {
        $result = array('rx' => array(), 'tx' => array());
        $chart_result = array('rx' => array(), 'tx' => array());
        $objects_colors = array('rx' => array(), 'tx' => array());
        
        $colors = get_charts_colors();

        $objects = NM_objects::get_instance();
        
        $colors_set = array();
        for($i = 0; count($object_data) > $i; $i++)
        {
            $max_id = array('rx' => -2, 'tx' => -2);
            $max_val = array('rx' => -2, 'tx' => -2);

            foreach($object_data as $object_id => $object_values)
            {
                if($max_val['rx'] < $object_values->rx && !isset($result['rx'][$object_id]))
                {
                    $max_val['rx'] = $object_values->rx;
                    $max_id['rx'] = $object_id;
                }
                if($max_val['tx'] < $object_values->tx && !isset($result['tx'][$object_id]))
                {
                    $max_val['tx'] = $object_values->tx;
                    $max_id['tx'] = $object_id;
                }
            }

            $name_rx = $objects->find_object($max_id['rx'], $object_type)->get_name();
            $name_tx = $objects->find_object($max_id['tx'], $object_type)->get_name();

            $size_type_rx = NM_SIZE_B;
            $size_type_tx = NM_SIZE_B;
            $size_rx = $max_val['rx'];
            $size_tx = $max_val['tx'];
            standarize_size($size_rx, $size_type_rx);
            standarize_size($size_tx, $size_type_tx);

            $result['rx'][$max_id['rx']] = array('name' => $name_rx, 'value' => $size_rx . ' ' . size_type_to_string($size_type_rx));
            $result['tx'][$max_id['tx']] = array('name' => $name_tx, 'value' => $size_tx . ' ' . size_type_to_string($size_type_tx));

            if($i < 8)
            {
                if(!isset($colors_set[$max_id['rx']]))
                    $colors_set[$max_id['rx']] = array_shift($colors);
                if(!isset($colors_set[$max_id['tx']]))
                    $colors_set[$max_id['tx']] = array_shift($colors);

                $objects_colors['rx'][] = $colors_set[$max_id['rx']];
                $objects_colors['tx'][] = $colors_set[$max_id['tx']];
                $chart_result['rx'][] = array("$name_rx $size_rx " . size_type_to_string($size_type_rx), $max_val['rx']);
                $chart_result['tx'][] = array("$name_tx $size_tx " . size_type_to_string($size_type_tx), $max_val['tx']);
            }
            else if($i == 8)
            {
                $color = array_shift($colors);
                $objects_colors['rx'][] = $color;
                $objects_colors['tx'][] = $color;
                $chart_result['rx'][8] = array('Others', $max_val['rx']);
                $chart_result['tx'][8] = array('Others', $max_val['tx']);
            }
            else if($i > 8)
            {
                $chart_result['rx'][8][1] += $max_val['rx'];
                $chart_result['tx'][8][1] += $max_val['tx'];
            }
        }

        if(isset($chart_result['rx'][8]))
        {
            $size = $chart_result['rx'][8][1];
            $type = NM_SIZE_B;
            standarize_size($size, $type);
            $chart_result['rx'][8][0] = "Others {$size} " . size_type_to_string($type);

            $size = $chart_result['tx'][8][1];
            $type = NM_SIZE_B;
            standarize_size($size, $type);
            $chart_result['tx'][8][0] = "Others {$size} " . size_type_to_string($type);
        }
        unset($size);
        unset($type);
        
        return array('result' => $result, 'charts' => $chart_result, 'colors' => $objects_colors);
    }
    
    public function download_upload($object, $id)
    {
        $cache = NM_cache::get_instance('loader');
        
        $cache_key = 'NM_loader::download_upload_' . $object . '_' . $id;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $object_path = get_object_path($object);
        
        $total_file = $object_path . $id . DS . 'total_info';
        
        $content = file_get_contents($total_file);
        if(!$content)
            throw new NM_exception(NM_EX_DATA_READ_FAILED, "Filed to read file: {$total_file}", __FILE__, __LINE__);
        
        $data = unserialize($content);
        if(!$data)
            throw new NM_exception(NM_EX_UNSERIALIZE_FAILED, 'Failed to unserialize file content', __FILE__, __LINE__);
        
        $result = array();
        
        $rx_size = $data->rx;
        $rx_size_type = NM_SIZE_B;
        
        standarize_size($rx_size, $rx_size_type);
        $rx_size_type = size_type_to_string($rx_size_type);
        
        $tx_size = $data->tx;
        $tx_size_type = NM_SIZE_B;
        
        standarize_size($tx_size, $tx_size_type);
        $tx_size_type = size_type_to_string($tx_size_type);
        
        $result[] = array('Download ' . $rx_size . ' ' . $rx_size_type, $data->rx);
        $result[] = array('Upload ' . $tx_size . ' ' . $tx_size_type, $data->tx);
        
        $cache->set($cache_key, $result);
        
        return $result;
    }
    
    public function usage($object_type, $id, $type, $start, $end)
    {
        $cache = NM_cache::get_instance('loader');
        $cache_key = 'NM_loader::usage_' . $object_type . '_' . $id . '_' . $type . '_' . $start->get_time() . '_' . $end->get_time();
        $cache_data = $cache->get($cache_key);
        if($cache_data)
        {
            return $cache_data;
        }
        $object_path = get_object_path($object_type);
        
        $method = "{$type}_usage";
        $result = $this->$method($object_type, $object_path, $id, $start, $end);
        
        $cache->set($cache_key, $result);
        return $result;
    }
    
    private function year_usage($object_type, $object_path, $id, $start, $end)
    {
        if($end->year <= $start->year)
            throw new NM_exception(NM_EX_WRONG_DATA, 'End year must be higher than start year', __FILE__, __LINE__);
        
        $object_path .= $id . DS;
        
        $result = array('rx' => array(), 'tx' => array());
        
        $max_size = array('rx' => 0.0, 'tx' => 0.0);
        
        for($year = $start->year; $year <= $end->year; $year++)
        {
            $file = $object_path . $year . DS . 'year_info';
            
            $rx = array($year . '-1-1', 0);
            $tx = array($year . '-1-1', 0);
            if(file_exists($file))
                $this->load_usage($file, $rx[1], $tx[1], $max_size);
            
            $result['rx'][] = $rx;
            $result['tx'][] = $tx;
        }
        
        return $this->prepare_return_usage($result, $max_size);
    }
    
    private function mon_usage($object_type, $object_path, $id, $start, $end)
    {
        if($start->year > $end->year || ($start->year == $end->year  && $start->mon >= $end->mon))
            throw new NM_exception(NM_EX_WRONG_DATA, 'End year must be higher than start year', __FILE__, __LINE__);
        
        $start_mon = $start->mon;
        //$end_mon = ($start->year == $end->year) ? $end->mon : 12;
        $element_dir = $object_path . $id . DS;
        
        $result = array('rx' => array(), 'tx' => array());
        $max_size = array('rx' => 0.0, 'tx' => 0.0);
        
        for($year = $start->year; $year <= $end->year; $year++)
        {
            $year_dir = $element_dir . $year . DS;
            if(true)//(is_dir($year_dir))
            {
                $end_mon = ($year == $end->year) ? $end->mon : 12;
                for($mon = $start_mon; $mon <= $end_mon; $mon++)
                {
                    $file = $year_dir . (int)$mon . DS . 'mon_info';
                    $rx = array("{$year}-{$mon}-1", 0);
                    $tx = array("{$year}-{$mon}-1", 0);
                    if(file_exists($file))
                        $this->load_usage($file, $rx[1], $tx[1], $max_size);
                    
                    $result['rx'][] = $rx;
                    $result['tx'][] = $tx;
                }
            }
            $start_mon = 1;
        }
        return $this->prepare_return_usage($result, $max_size);
    }
    
    private function day_usage($object_type, $object_path, $id, $start, $end)
    {
        $start->hour = $start->min = $start->sec = 0;
        $end->hour = $end->min = $end->sec = 0;
        
        if($start->get_time() >= $end->get_time())
            throw new NM_exception(NM_EX_WRONG_DATA, 'End year must be higher than start year', __FILE__, __LINE__);
        
        $start_mon = $start->mon;
        //$end_mon = ($start->year == $end->year) ? $end->mon : 12;
        
        $start_day = $start->day;
        //$end_day = ($start->mon == $end->mon) ? $end->day : NM_date::maks_days_of_month($start->year, $start_mon);
        
        $element_dir = $object_path . $id . DS;
        
        $result = array('rx' => array(), 'tx' => array());
        $max_size = array('rx' => 0.0, 'tx' => 0.0);
        
        for($year = $start->year; $year <= $end->year; $year++)
        {
            $year_dir = $element_dir . $year . DS;
            if(true)//(is_dir($year_dir))
            {
                $end_mon = ($year == $end->year) ? $end->mon : 12;
                
                for($mon = $start_mon; $mon <= $end_mon; $mon++)
                {
                    $mon_dir = $year_dir . (int)$mon . DS;
                    NM_fb::info($mon_dir);
                    if(true)//(is_dir($mon_dir))
                    {
                        $end_day = ($year == $end->year && $mon == $end->mon) ? $end->day : NM_date::maks_days_of_month($year, $mon);
                        for($day = $start_day; $day <= $end_day; $day++)
                        {
                            $day_dir = $mon_dir . (int)$day .DS;
                            NM_fb::info($day);
                            if(true)//(is_dir($day_dir))
                            {
                                $file = $mon_dir . $day . DS . 'day_info';
                                $rx = array("{$year}-{$mon}-{$day}", 0);
                                $tx = array("{$year}-{$mon}-{$day}", 0);
                                if(file_exists($file))
                                    $this->load_usage($file, $rx[1], $tx[1], $max_size);

                                $result['rx'][] = $rx;
                                $result['tx'][] = $tx;
                            }
                        }
                    }
                    $start_day = 1;
                }
            }
            else
                $start_day = 1;
            $start_mon = 1;
        }
        return $this->prepare_return_usage($result, $max_size);
    }
    
    private function hour_usage($object_type, $object_path, $id, $start, $end)
    {
        $start->min = $start->sec = 0;
        $end->min = $end->sec = 0;
        
        if($start->get_time() >= $end->get_time())
            throw new NM_exception(NM_EX_WRONG_DATA, 'End year must be higher than start year', __FILE__, __LINE__);
        
        $start_mon = $start->mon;
        
        $start_day = $start->day;
        
        $start_hour = $start->hour;
        
        $element_dir = $object_path . $id . DS;
        
        $result = array('rx' => array(), 'tx' => array());
        $max_size = array('rx' => 0.0, 'tx' => 0.0);
        
        for($year = $start->year; $year <= $end->year; $year++)
        {
            $year_dir = $element_dir . $year . DS;
            
            if(true)//if(is_dir($year_dir))
            {
                $end_mon = ($year == $end->year) ? $end->mon : 12;
                for($mon = $start_mon; $mon <= $end_mon; $mon++)
                {
                    $mon_dir = $year_dir . (int)$mon . DS;
                    if(true)//if(is_dir($mon_dir))
                    {
                        $end_day = ($year == $end->year && $mon == $end->mon) ? $end->day : NM_date::maks_days_of_month($year, $mon);
                        for($day = $start_day; $day <= $end_day; $day++)
                        {
                            $day_dir = $mon_dir . (int)$day . DS;
                            if(true)//if(is_dir($day_dir))
                            {
                                $end_hour = ($year == $end->year && $mon == $end->mon && $day == $end->day) ? $end->hour : 23;
                                for($hour = $start_hour; $hour <= $end_hour; $hour++)
                                {
                                    $file = $day_dir . (int)$hour . '_hour';
                                    $rx = array("{$year}-{$mon}-{$day} {$hour}", 0);
                                    $tx = array("{$year}-{$mon}-{$day} {$hour}", 0);

                                    if(file_exists($file))
                                    {
                                        $this->load_usage($file, $rx[1], $tx[1], $max_size);
                                    }
                                    
                                    $result['rx'][] = $rx;
                                    $result['tx'][] = $tx;
                                }
                            }
                            $start_hour = 0;
                        }
                    }
                    else
                        $start_hour = 0;
                    $start_day = 1;
                }
            }
            else
            {
                $start_day = 1;
                $start_hour = 0;
            }
            $start_mon = 1;
        }
        
        return $this->prepare_return_usage($result, $max_size);
    }
    
    private function min_usage($object_type, $object_path, $id, $start_date, $end_date)
    {
        $start_date->sec = 0;
        $end_date->sec = 0;
        
        NM_fb::info($start_date->get_time());
        NM_fb::info($end_date->get_time());
        if($start_date->get_time() >= $end_date->get_time())
            throw new NM_exception(NM_EX_WRONG_DATA, 'End year must be higher than start year', __FILE__, __LINE__);
        
        $start_mon = $start_date->mon;
        
        $start_day = $start_date->day;
        
        $start_hour = $start_date->hour;
        $start_min = $start_date->min;
        
        $element_dir = $object_path . $id . DS;
        
        $result = array('rx' => array(), 'tx' => array());
        $max_size = array('rx' => 0.0, 'tx' => 0.0);
        
        for($year = $start_date->year; $year <= $end_date->year; $year++)
        {
            $year_dir = $element_dir . $year . DS;
            
            if(true)//(is_dir($year_dir))
            {
                $end_mon = ($year == $end_date->year) ? $end_date->mon : 12;
                for($mon = $start_mon; $mon <= $end_mon; $mon++)
                {
                    $mon_dir = $year_dir . (int)$mon . DS;
                    if(true)//(is_dir($mon_dir))
                    {
                        $end_day = ($year == $end_date->year && $mon == $end_date->mon) ? $end_date->day : NM_date::maks_days_of_month($year, $mon);
                        for($day = $start_day; $day <= $end_day; $day++)
                        {
                            $day_dir = $mon_dir . (int)$day . DS;
                            if(true)//(is_dir($day_dir))
                            {
                                $file = $day_dir . 'info';
                                $info = array();
                                if(file_exists($file))
                                {
                                    $contents = file_get_contents($file);
                                    if($contents)
                                    {
                                        $info = unserialize($contents);
                                        if(!$info)
                                            $info = array();
                                    }
                                }
                                
                                $end_hour = ($year == $end_date->year && $mon == $end_date->mon && $day == $end_date->day) ? $end_date->hour : 23;
                                
                                for($hour = $start_hour; $hour <= $end_hour; $hour++)
                                {
                                    $end_min = ($year == $end_date->year && $mon == $end_date->mon && $day == $end_date->day && $hour == $end_date->hour) ? $end_date->min : 59;

                                    if(!isset($info[$hour]))
                                        $info[$hour] = array();

                                    $min_info = $info[$hour];
                                    for($min = $start_min; $min <= $end_min; $min++)
                                    {
                                        if(!isset($min_info[$min]))
                                        {
                                            $rx = 0;
                                            $tx = 0;
                                        }
                                        else
                                        {
                                            $rx = $min_info[$min]->rx;
                                            $tx = $min_info[$min]->tx;
                                        }
                                        
                                        if($max_size['rx'] < $rx)
                                            $max_size['rx'] = $rx;
                                        if($max_size['tx'] < $tx)
                                            $max_size['tx'] = $tx;
                                        
                                        $result['rx'][] = array("{$year}-{$mon}-{$day} {$hour}:{$min}", $rx);
                                        $result['tx'][] = array("{$year}-{$mon}-{$day} {$hour}:{$min}", $tx);
                                    }
                                    $start_min = 0;
                                }
                                $start_hour = 0;
                            }
                        }
                    }
                    $start_day = 1;
                }
            }
            else
                $start_day = 1;
            $start_mon = 1;
        }
        
        return $this->prepare_return_usage($result, $max_size);
    }
    
    private function load_usage($file, &$rx, &$tx, &$max)
    {
        if(file_exists($file))
        {
            $contents = file_get_contents($file);
            if($contents)
            {
                $info = unserialize($contents);
                if($info)
                {
                    $rx = $info->rx;
                    $tx = $info->tx;
                    
                    if($max['rx'] < $info->rx)
                        $max['rx'] = $info->rx;

                    if($max['tx'] < $info->tx)
                        $max['tx'] = $info->tx;
                }
            }
        }
    }
    
    private function prepare_return_usage($result, $max_size)
    {
        $type = NM_SIZE_B;
        $size_type = array('rx' => NM_SIZE_B, 'tx' => NM_SIZE_B);
        $max = ($max_size['rx'] > $max_size['tx']) ? $max_size['rx'] : $max_size['tx'];
        
        standarize_size($max, $type);
        standarize_size($max_size['rx'], $size_type['rx']);
        standarize_size($max_size['tx'], $size_type['tx']);
        
        $size_type_diff = array('rx', 'tx');
        $size_type_diff['rx'] = $type - $size_type['rx'];
        $size_type_diff['tx'] = $type - $size_type['tx'];
        
        // rx and tx array has the same size
        for($i = 0; count($result['rx']) > $i; $i++)
        {
            // jeśli w bajtach dzielnik powinien być jeden więc tutaj także ma być minus. nie zmieniać
            $result['rx'][$i][1] = round($result['rx'][$i][1] / pow(1024, $type - 1), 2);
            $result['tx'][$i][1] = round($result['tx'][$i][1] / pow(1024, $type - 1), 2);
        }
        
        return array('rx' => $result['rx'],
                     'tx' => $result['tx'],
                     'max_size' => ceil(1.2 * $max), 'size_type' => $type,
                     'max_size_rx' => ceil(1.2 * $max_size['rx']), 'size_type_rx' => $size_type['rx'],
                     'max_size_tx' => ceil(1.2 * $max_size['tx']), 'size_type_tx' => $size_type['tx'],
                     'size_type_diff' => $size_type_diff);
    }
    
    
    public function get_programs_list($max = -1)
    {
        $cache = NM_cache::get_instance('objects');
        $cache_key = 'programs_list_' . (string)$max;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $programs = array();
        $i = 0;
        
        foreach(NM_objects::get_instance()->get_current_programs() as $id => $program)
        {
            $bufor = new stdClass();
            $bufor->id = $id;
            $bufor->name = $program->get_name();
            if(strlen($bufor->name) >= 53)
                $bufor->name = substr($bufor->name, 0, 50) . '...';
            
            $programs[] = $bufor;
            unset($bufor);
            
            if($max > 0 && ++$i >= $max)
                break;
        }
        
        $cache->set($cache_key, $programs);
        return $programs;
    }
    
    public function get_users_list($max = -1)
    {
        $cache = NM_cache::get_instance('objects');
        $cache_key = 'users_list_' . (string)$max;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $users = array();
        $i = 0;
        
        foreach(NM_objects::get_instance()->get_current_users() as $id => $user)
        {
            $bufor = new stdClass();
            $bufor->id = $id;
            $bufor->sys_id = $user->get_sys_id();
            $bufor->name = $user->get_name();
            if(strlen($bufor->name) >= 53)
                $bufor->name = substr($bufor->name, 0, 50) . '...';
            
            $users[] = $bufor;
            unset($bufor);
            
            if($max > 0 && ++$i >= $max)
                break;
        }
        
        $cache->set($cache_key, $users);
        return $users;
    }
    
    public function get_devices_list($max = -1)
    {
        $cache = NM_cache::get_instance('objects');
        $cache_key = 'devices_list_' . (string)$max;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $devices = array();
        $i = 0;
        
        foreach(NM_objects::get_instance()->get_current_devices() as $id => $device)
        {
            $bufor = new stdClass();
            $bufor->id = $id;
            $bufor->name = $device->get_name();
            $bufor->address = $device->get_address();
            $bufor->address6 = $device->get_address6();
            if(strlen($bufor->name) >= 53)
                $bufor->name = substr($bufor->name, 0, 50) . '...';
           
            $devices[] = $bufor;
            unset($bufor);
            
            if($max > 0 && ++$i >= $max)
                break;
        }
        
        $cache->set($cache_key, $devices);
        return $devices;
    }
    
    public function get_remotes_list($max = -1)
    {
        $cache = NM_cache::get_instance('objects');
        $cache_key = 'remotes_list_' . (string)$max;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $remotes = array();
        $i = 0;
        
        foreach(NM_objects::get_instance()->get_current_remotes() as $id => $remote)
        {
            $bufor = new stdClass();
            $bufor->id = $id;
            $bufor->name = $remote->get_name();
            $bufor->address = $remote->get_address();
            $bufor->address6 = $remote->get_address6();
            if(strlen($bufor->name) >= 53)
                $bufor->name = substr($bufor->name, 0, 50) . '...';
           
            $remotes[] = $bufor;
            unset($bufor);
            
            if($max > 0 && ++$i >= $max)
                break;
        }
        
        $cache->set($cache_key, $remotes);
        return $remotes;
    }
}