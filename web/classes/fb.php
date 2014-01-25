<?php

include_once CLASS_DIR . 'fb/fb' . PHP_EXT;

final class NM_fb extends FB
{
    private static $console_mode;
    
    private function __contructor()
    {
        
    }
    
    public static function initialize()
    {
        self::$console_mode = NM_application::get_instance()->get_console_mode();
    }
    
    public static function log($object, $label = null)
    {
        if(NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF) == NM_DEBUG_LEVEL_OFF)
            return;
        if(self::$console_mode)
            echo 'Log: ', $object, EOL;
        else
            parent::log($object, $label);
    }
    
    public static function info($object, $label = null)
    {
        if(NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF) == NM_DEBUG_LEVEL_OFF)
            return;
        
        if(self::$console_mode)
            echo 'Info: ', $object, EOL;
        else
            parent::info($object, $label);
    }
    
    public static function warn($object, $label = null)
    {
        if(NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF) == NM_DEBUG_LEVEL_OFF)
            return;
        
        if(self::$console_mode)
            echo 'Warn: ', $object, EOL;
        else
            parent::warn($object, $label);
    }
    
    public static function error($object, $label = null)
    {
        if(NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF) == NM_DEBUG_LEVEL_OFF)
            return;
        
        if(self::$console_mode)
            echo 'Error: ', $object, EOL;
        else
            parent::error($object, $label);
    }
    
    public static function dump($key, $variable)
    {
        if(NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF) == NM_DEBUG_LEVEL_OFF)
            return;
        
        if(self::$console_mode)
            echo 'Dump is not available in console mode', EOL;
        else
            parent::dump($key, $variable);
    }
    
    public static function trace($label)
    {
        if(NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF) == NM_DEBUG_LEVEL_OFF)
            return;
        
        if(self::$console_mode)
            echo 'Trace is not available in console mode', EOL;
        else
            parent::dump($label);
    }
    
    public static function table($label, $table)
    {
        if(NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF) == NM_DEBUG_LEVEL_OFF)
            return;
        
        if(self::$console_mode)
            echo 'Table is not available in console mode', EOL;
        else
            parent::dump($label, $table);
    }
}
