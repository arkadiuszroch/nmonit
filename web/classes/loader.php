<?php

if(!defined("NMONIT"))
    die("Direct access to this file is not allowed!");

final class NM_loader
{
    private function __construct()
    {
        
    }
    
    public static function get_from_array(array $array, $key, $default = NULL)
    {
        if(!isset($array[$key]))
            return $default;
        return $array[$key];
    }
    
    public static function get_from_class($class, $key, $default = NULL)
    {
        if(!isset($class->$key))
            return $default;
        return $class->$key;
    }
    
    public static function get_post($key, $default = NULL)
    {
        return self::get_from_array($_POST, $key, $default);
    }
    
    public static function get_get($key, $default = NULL)
    {
        return self::get_from_array($_GET, $key, $default);
    }
    
    public static function get_session($key, $default = NULL)
    {
        return self::get_from_array($_SESSION, $key, $default);
    }
    
    public static function set_session($key, $val)
    {
        $_SESSION[$key] = $val;
    }
    
    public static function get_server($key, $default = NULL)
    {
        return self::get_from_array($_SERVER, $key, $default);
    }
    
    public static function set_server($key, $val)
    {
        $_SERVER[$key] = $val;
    }
    
    public static function get_request($key, $default = NULL)
    {
        return self::get_from_array($_REQUEST, $key, $default);
    }
    
    public static function get_env($key, $default = NULL)
    {
        return self::get_from_array($_ENV, $key, $default);
    }
}