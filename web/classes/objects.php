<?php

final class NM_objects extends NM_basic_singleton
{
    private $_programs;
    private $_users;
    private $_devices;  
    private $_remotes;
    private $_count;
    private $_model;
    
    protected function __construct()
    {
        $this->_programs = array();
        $this->_users = array();
        $this->_devices = array();
        $this->_remotes = array();
        
        $this->_count = 0;
        
        $this->_model = new NM_model_objects();
        
        $this->_programs[-1] = new NM_object_program(-1, 'unknown', new NM_date());
        $this->_users[-1] = new NM_object_user(-1, 'unknown', new NM_date(), -1);
        $this->_devices[-1] = new NM_object_device(-1, 'unknown', new NM_date(), NM_DEF_ADDRESS, NM_DEF_ADDRESS6);
        $this->_remotes[-1] = new NM_object_remote(-1, 'unknown', new NM_date(), NM_DEF_ADDRESS, NM_DEF_ADDRESS6);
        
        $this->read_data();
    }
    
    private function read_data()
    {
        $read = false;
        $contents = '';
        
        if( !file_exists(PC_INFO_FILE) )
            $read = true;
        else
        {
            
            $contents = file_get_contents(PC_INFO_FILE);
            if($contents)
            {
                $objects = unserialize($contents);

                if($objects)
                {
                    $this->_count = $objects['count'];

                    $objects_count = $this->_model->get_objects_count();

                    if($this->_count != $objects_count[0]['count'])
                        $read = true;
                    else
                    {
                        $this->_programs = $objects[NM_PROGRAM];
                        $this->_users = $objects[NM_USER];
                        $this->_devices = $objects[NM_DEVICE];
                        $this->_remotes = $objects[NM_REMOTE];
                    }
                    unset($objects);
                }
                else // $objects
                    $read = true;
            }
            else // $contents
                $read = true;
        }
        
        NM_fb::info('checking read');
        
        if( $read )
        {
            $cache = NM_cache::get_instance('objects');
            $cache->clear();
            
            $objects = $this->_model->get_objects();
            
            $date = new NM_date();
            
            $loaded_programs = $loaded_users = $loaded_devices = $loaded_remotes = array();
            
            foreach($objects as $object)
            {
                $date->set_time(strtotime($object['date']));
                switch($object['type'])
                {
                    case NM_OBJECT_DEVICE:
                    {
                        list($addres, $addres6) = explode(';', $object['info']);
                        
                        if(isset($loaded_devices[$object['name']]))
                        {
                            $old_id = $loaded_devices[$object['name']];
                            $this->_devices[$old_id]->add($object['id'], $object['name'], clone $date, $addres, $addres6);
                        }
                        else
                        {
                            $this->_devices[$object['id']] =  new NM_object_device($object['id'], $object['name'], clone $date, $addres, $addres6);
                            $loaded_devices[$object['name']] = $object['id'];
                        }
                    }
                    break;
                    case NM_OBJECT_REMOTE:
                    {
                        list($addres, $addres6) = explode(';', $object['info']);
                        
                        if(isset($loaded_remotes[$object['name']]))
                        {
                            $old_id = $loaded_remotes[$object['name']];
                            $this->_remotes[$old_id]->add($object['id'], $object['name'], clone $date, $addres, $addres6);
                        }
                        else
                        {
                            $this->_remotes[$object['id']] =  new NM_object_remote($object['id'], $object['name'], clone $date, $addres, $addres6);
                            $loaded_remotes[$object['name']] = $object['id'];
                        }
                    }
                    break;
                    case NM_OBJECT_PROGRAM:
                    {
                        if(isset($loaded_programs[$object['name']]))
                        {
                            $old_id = $loaded_programs[$object['name']];
                            $this->_programs[$old_id]->add($object['id'], $object['name'], clone $date);
                        }
                        else
                        {
                            $this->_programs[$object['id']] =  new NM_object_program($object['id'], $object['name'], clone $date);
                            $loaded_programs[$object['name']] = $object['id'];
                        }
                    }
                    break;
                    case NM_OBJECT_USER:
                    {
                        if(isset($loaded_users[$object['name']]))
                        {
                            $old_id = $loaded_users[$object['name']];
                            $this->_users[$old_id]->add($object['id'], $object['name'], clone $date, $object['info']);
                        }
                        else
                        {
                            $this->_users[$object['id']] =  new NM_object_user($object['id'], $object['name'], clone $date, $object['info']);
                            $loaded_users[$object['name']] = $object['id'];
                        }
                    }
                    break;
                }
            }
            
            $file_objects = array();
            $file_objects['count'] = count($objects);
            $file_objects[NM_PROGRAM] = $this->_programs;
            $file_objects[NM_USER] = $this->_users;
            $file_objects[NM_DEVICE] = $this->_devices;
            $file_objects[NM_REMOTE] = $this->_remotes;
            $to_file = serialize($file_objects);
            file_put_contents(PC_INFO_FILE, $to_file);
        }
    }
    public function get_programs()
    {
        return $this->_programs;
    }
    
    public function get_users()
    {
        return $this->_users;
    }
    
    public function get_devices()
    {
        return $this->_devices;
    }
    
    public function get_remotes()
    {
        return $this->_remotes;
    }
    
    
    public function get_current_programs()
    {
        return $this->get_current('programs');
    }
    
    public function get_incative_programs()
    {
        return $this->get_inactive('programs');
    }
    
    public function get_current_users()
    {
        return $this->get_current('users');
    }
    
    public function get_incative_users()
    {
        return $this->get_inactive('users');
    }
    
    public function get_current_devices()
    {
        return $this->get_current('devices');
    }
    
    public function get_incative_devices()
    {
        return $this->get_inactive('devices');
    }
    
    public function get_current_remotes()
    {
        return $this->get_current('remotes');
    }
    
    public function get_current_objects()
    {
        $return = array();
        
        $return['programs'] = $this->get_current_programs();
        $return['users'] = $this->get_current_users();
        $return['devices'] = $this->get_current_devices();
        $return['remote'] = $this->get_current_remotes();
        
        return $return;
    }
    
    public function get_current($element)
    {
        if(!in_array($element, array('programs', 'users', 'devices', 'remotes')))
            return false;
        
        $cache = NM_cache::get_instance('objects');
        $cache_key = 'NM_objects::get_current_' . $element;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $return = array();
        
        $field = "_{$element}";
        
        foreach($this->$field as $id => $object)
        {
            if(preg_match('!^\d+$!', $id))
                if(is_dir(DATA_DIR . $element . DS . $id) || $element == 'remotes')
                    $return[$id] = $object;
        }
        
        $cache->set($cache_key, $return);
        return $return;
    }
    
    
    public function get_inactive($element)
    {
        if(!in_array($element, array('programs', 'users', 'devices', 'remotes')))
            return false;
        
        $cache = NM_cache::get_instance('objects');
        $cache_key = 'NM_objects::get_inactive_' . $element;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $return = array();
        
        $field = "_{$element}";
        
        foreach($this->$field as $id => $object)
        {
            if(preg_match('!^\d+$!', $id))
                if(!is_dir(DATA_DIR . $element . DS . $id))
                    $return[$id] = $object;
        }
        
        $cache->set($cache_key, $return);
        return $return;
    }
    
    public function find_object($id, $object_type, $ignore_read = false)
    {
        $cache = NM_cache::get_instance('objects');
        $cache_key = 'NM_objects::find_object_' . $id . '_' . $object_type . '_' . (string)$ignore_read;
        $cache_data = $cache->get($cache_key);
        if($cache_data)
            return $cache_data;
        
        $object_name = "_{$object_type}s";
        
        if(!isset($this->$object_name))
            throw new NM_exception(NM_EX_OBJECT_DOESNT_EXISTS, "Object: {$object_type} doesnt extsts", __FILE__, __LINE__);
        
        $objects = $this->$object_name;
        
        if(isset($objects[$id]))
            return $objects[$id];
        
        foreach($objects as $object)
        {
            if($object->check_id($id))
                return $object;
        }
        
        if($ignore_read)
            return false;
        
        $this->read_data();
        
        $return = $this->find_object($id, $object_type, true);
        
        $cache->set($cache_key, $return);
        
        return $return;
    }
}