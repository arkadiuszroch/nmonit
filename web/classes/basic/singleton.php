<?php

if(!defined("NMONIT"))
    die("Direct access to this file is not allowed!");

abstract class NM_basic_singleton
{
    protected function __constructor()
    {
        
    }
    
    public final static function get_instance()
    {
        static $instances = array();
        
        $class = get_called_class();
        
        if(!isset($instances[$class]))
            $instances[$class] = new $class();
        
        return $instances[$class];
    }
}