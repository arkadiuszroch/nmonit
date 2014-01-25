<?php

abstract class NM_basic_object
{
    protected $_id;
    protected $_main_id;
    protected $_name;
    protected $_added_time;
    protected $_modify_time;
    protected $_history;
    
    public function __construct($id, $name, $added)
    {
        $this->_id = $id;
        $this->_main_id = $id;
        $this->_name = $name;
        $this->_added_time = $added;
        $this->_modify_time = $added;
        $this->_history = array();
    }
    
    public function get_id()
    {
        return $this->_id;
    }
    
    public function get_main_id()
    {
        return $this->_main_id;
    }
    
    public function get_name()
    {
        return $this->_name;
    }
    
    public function get_added_time()
    {
        return $this->_added_time;
    }
    
    public function get_modify_time()
    {
        return $this->_modify_time;
    }
    
    public function get_history()
    {
        return $this->_history;
    }
    
    public function __sleep()
    {
        return array('_id', '_main_id', '_name', '_added_time', '_modify_time', '_history');
    }
    
    public function check_id($id)
    {
        if($this->_id == $id)
            return true;
        else
        {
            if(array_key_exists($id, $this->_history))
                return true;
        }
        return false;
    }
    
    abstract public function add($id, $name, $added_time);
}