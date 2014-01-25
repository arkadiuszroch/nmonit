<?php

final class NM_object_user extends NM_basic_object
{
    private $_sys_id;
    
    public function __construct($id, $name, $added_time, $sys_id)
    {
        parent::__construct($id, $name, $added_time);
        $this->_sys_id = $sys_id;
    }
    
    public function get_sys_id()
    {
        return $this->_sys_id;
    }
    
    public function __sleep()
    {
        $array = parent::__sleep();
        
        $array[] = '_sys_id';
        return $array;
    }
    
    public function add($id, $name, $added_time, $sys_id = 0)
    {
        $bufor = new stdClass();
        
        $bufor->id = $this->_id;
        $bufor->sys_id = $this->_sys_id;
        $bufor->name = $this->_name;
        $bufor->added_time = $this->_added_time;
        
        $this->_history[$this->_id] = $bufor;
        
        $this->_id = $id;
        $this->_sys_id = $sys_id;
        $this->_name = $name;
        $this->_added_time = $added_time;
    }
}