<?php

final class NM_object_device extends NM_basic_object
{
    private $_address;
    private $_address6;
    
    public function __construct($id, $name, $added_time, $address, $address6)
    {
        parent::__construct($id, $name, $added_time);
        $this->_address = $address;
        $this->_address6 = $address6;
    }
    
    public function get_address()
    {
        return $this->_address;
    }
    public function get_address6()
    {
        return $this->_address6;
    }
    
    public function __sleep()
    {
        $array = parent::__sleep();
        
        $array[] = '_address';
        $array[] = '_address6';
        
        return $array;
    }
    
    public function add($id, $name, $added_time, $address = NM_DEF_ADDRESS, $address6 = NM_DEF_ADDRESS6)
    {
        $bufor = new stdClass();
        
        $bufor->id = $this->_id;
        $bufor->name = $this->_name;
        $bufor->added_time = $this->_added_time;
        $bufor->address = $this->_address;
        $bufor->address6 = $this->_address6;
        
        $this->_history[$this->_id] = $bufor;
        
        $this->_id = $id;
        $this->_name = $name;
        $this->_added_time = $added_time;
        $this->_address = $address;
        $this->_address6 = $address6;
    }
}