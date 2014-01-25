<?php

final class NM_cache extends NM_basic_registry
{
    private $_keys;
    private $_instance;
    
    protected function __construct($instance = 'default')
    {
        $this->_instance = $instance;
        $this->_keys = array();
        $keys = NM_loader::get_session("NM_cache_{$this->_instance}", array());
        
        
        foreach($keys as $key => $val)
        {
            $current_date = new NM_date();
            
            if(is_string($val->date))
                $val->date = unserialize($val->date);
            $time = $val->time;
            $date = $val->date;
            
            
            if($date->get_time() + $time >= $current_date->get_time())
            {
                $this->_keys[$key] = $val;
            }
            else
            {
                if($val->type == NM_CACHE_FILE)
                    if(file_exists($val->val))
                        unlink($val->val);
            }
        }
    }
    
    public function __destruct()
    {
        $keys = array();
        foreach($this->_keys as $key => $val)
        {
            $val->date = serialize($val->date);
            
            $keys[$key] = $val;
        }
        
        NM_loader::set_session("NM_cache_{$this->_instance}", $keys);
    }
    
    public function set($key, $val, $time = 60)
    {
        $this->add($key, $val, $time, true);
    }
    
    public function add($key, $val, $time = 60, $replace = false)
    {
        if(!$replace && isset($this->_keys[$key]))
            return false;
        
        $cache = $this->get_key_structure();
        
        if(is_bool($val) || is_int($val) || is_double($val) || is_float($val) || is_string($val) || is_null($val) || is_numeric($val))
        {
            $cache->val = $val;
            $cache->type = NM_CACHE_VAR;
        }
        else
        {
            $val = serialize($val);
            $hash = sha1($val);
            $file = CACHE_DIR . $hash;
            
            file_put_contents($file, $val);
            
            $cache->val = $file;
            $cache->type = NM_CACHE_FILE;
        }
        
        $cache->time = $time;
        $cache->date = new NM_date();
        
        $this->_keys[$key] = $cache;
    }
    
    public function get($key)
    {
        if(!isset($this->_keys[$key]) || NM_APPLICATION_ENV == NM_ENV_DEVELOPMENT)
        {
            return false;
        }
        else
        {
            $var = $this->_keys[$key];
            if($var->type == NM_CACHE_VAR)
                return $var->val;
            else
            {
                if(file_exists($var->val))
                {
                    $content = file_get_contents($var->val);
                    $val = unserialize($content);
                    if($val !== false)
                        return $val;
                    
                    return false;
                }
            }
        }
        
        return false;
    }
    
    public static function clear_all()
    {
        foreach(self::$instances as $instance)
        {
            NM_cache::get_instance($instance)->clear();
        }
        
        $cache_dir = opendir(CACHE_DIR);
        while(($file = readdir($cache_dir)) !== false)
        {
            if($file == '.' || $file == '..')
                continue;
            
            unlink(CACHE_DIR . $file);
        }
    }
    public function clear()
    {
        foreach($this->_keys as $val)
        {
            if($val->type == NM_CACHE_FILE)
            {
                if(file_exists($val->val))
                    unlink($val->val);
            }
        }
        
        unset($this->_keys);
        $this->_keys = array();
        
        NM_loader::set_session("NM_cache_{$this->_instance}", $this->_keys);
    }
    
    private function get_key_structure()
    {
        $return = new stdClass();
        
        $return->date = 0;
        $return->time = 300; // 5 minutes
        $return->val = '';
        $return->type = NM_CACHE_VAR;
        
        return $return;
    }
}