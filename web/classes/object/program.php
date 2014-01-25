<?php

final class NM_object_program extends NM_basic_object
{
    public function add($id, $name, $added_time)
    {
        $bufor = new stdClass();
        
        $bufor->id = $this->_id;
        $bufor->name = $this->_name;
        $bufor->added_time = $this->_added_time;
        $this->_history[$this->_id] = $bufor;
        
        $this->_id = $id;
        $this->_name = $name;
        $this->_added_time = $added_time;
    }
    
    public function check_id($id)
    {
        return isset($this->_history[$id]);
    }
}