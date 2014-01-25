<?php

if(!defined("NMONIT"))
    die("Direct access to this file is not allowed!");

abstract class NM_basic_registry
{
    protected static $instances = array();
    
    protected function __constructor($instance = 'default')
    {
        
    }
    
    public static function get_instance($instance = "default")
    {
        static $instances = array();
        
        $class = get_called_class();
        
        if(!isset($instances[$class]))
        {
            $instances[$class] = array();
            $instances[$class][$instance] = new $class($instance);
        }
        else if(!isset($instances[$class][$instance]))
        {
            $instances[$class][$instance] = new $class($instance);
        }
        
        return $instances[$class][$instance];
    }
}