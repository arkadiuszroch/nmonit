<?php

if(!defined("NMONIT"))
    die("Direct access to this file is not allowed!");

include FUNC_DIR . 'const.php';

function vd()
{
    echo '<pre>';
    call_user_func_array('var_dump', func_get_args());
    echo '</pre>';
}

function vde()
{
    call_user_func_array('vd', func_get_args());
    exit;
}

function vdp()
{
    ob_start();
    call_user_func_array('var_dump', func_get_args());
    $contents = ob_get_contents();
    ob_end_clean();

    return $contents;
}

function basic_error()
{
    $last = error_get_last();
    if($last != NULL)
    {
        /*switch($last['type'])
        {
            case E_ERROR:
            case E_WARNING:
            {*/
                $file = preg_replace('!' . ROOT_DIR . '!', '', $last['file']);
                echo "<h1 style = 'font-size: 20px;'>NMonit system error</h1>";
                echo "<p style = 'font-size: 15px;'>{$last['message']}<br />";
                echo "In file {$file} on line {$last['line']}</p>";
            /*}
            break;
        }*/
    }
    
    if(NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF) == NM_DEBUG_LEVEL_PRINT)
        echo NM_error::get_instance();
}

function error_handler($code, $msg, $file, $line, $context, $type = NM_ERR_TYPE_ERROR)
{
    if(preg_match('!database is locked!', $msg))
    {
        include VIEWS_DIR . 'dbLock.phtml';
        exit;
    }
            
    $level = NM_loader::get_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF);
    
    if($level == NM_DEBUG_LEVEL_OFF)
        return;
    
    $error = NM_error::get_instance();
    $error->add_error($code, $msg, $file, $line, $context, $type);
    
    if($level == NM_DEBUG_LEVEL_STOP)
    {
        echo $error;
        exit;
    }
}

function exception_handler($e)
{
    if(!preg_match('!database is locked!', $e->getMessage()))
        error_handler($e->getCode(), $e->getMessage(), $e->getFile(), $e->getLine(), $e->getTrace(), NM_ERR_TYPE_EXCEPTION);
    else
    {
        include VIEWS_DIR . 'dbLock.phtml';
        exit;
    }
}

function __autoload($class)
{
    
    $class = preg_replace('!^NM_!', '', $class);
    $class = preg_replace('!'. CS . '!', DS, $class);
    $class = strtolower($class);
    
    if(preg_match('!^module!', $class))
    {
        $dir = MODULES_DIR;
        $class = preg_replace('!^module!', DS, $class);
    }
    else if(preg_match('!^model!', $class))
    {
        $dir = MODELS_DIR;
        $class = preg_replace('!^model!', DS, $class);
    }
    else
        $dir = CLASS_DIR;
    
    $file = $dir . $class . PHP_EXT;
    if(file_exists($file))
        include $file;
}

function cut_string($string, $length, $word = false)
{
    if(strlen($string) > $length - 3)
    {
        if($word)
            $string = substr($string, 0, strrpos(substr($string, 0, $length), ' '));
        else
            $string = substr($string, 0, $length);
        $string .= '...';
    }
    return $string;
}

function NM_fread($handle, $length, $type)
{
    if(feof($handle))
        return false;
    
    $return = fread($handle, $length);
    
    if(!$return)
        return false;
    
    if($type != NM_UNPACK_STRING)
    {
        $unpack = unpack($type, $return);
        $return = $unpack[1];
    }
    
    return $return;
}

function standarize_size(&$size, &$size_type)
{
    while($size > 1024.0 && $size_type < NM_SIZE_TB)
    {
        $size /= 1024.0;
        $size_type++;
    }
    
    $size = round($size, 2);
}

function size_type_to_string($size_type)
{
    switch($size_type)
    {
        case NM_SIZE_B:
            return 'B';
        break;
        case NM_SIZE_KB:
            return 'KB';
        break;
        case NM_SIZE_MB:
            return 'MB';
        break;
        case NM_SIZE_GB:
            return 'GB';
        break;
        case NM_SIZE_TB:
            return 'TB';
        break;
    }
}

function get_charts_colors()
{
    return array('#4bb2c5', '#c5b47f', '#EAA228', '#579575', '#839557', '#958c12', '#b55589', '#7b9df4', '#DC143C', '#DA70D6', '#800000', '#999999', '#CC3300', '#663399', '#99CC66', '#CC9999');
}

function get_object_path($object)
{
    $object_path = '';
    switch($object)
    {
        case NM_PROGRAM : 
            $object_path = DATA_PROGRAMS_DIR;
        break;
        case NM_USER : 
            $object_path = DATA_USERS_DIR;
        break;
        case NM_DEVICE : 
            $object_path = DATA_DEVICES_DIR;
        break;
        case NM_REMOTE : 
            $object_path = DATA_REMOTES_DIR;
        break;
        default:
            throw new NM_exception(NM_EX_INVALID_OBJECT_TYPE, 'Invalid object type', __FILE__, __LINE__);
    }
    
    return $object_path;
}

register_shutdown_function('basic_error');
set_error_handler('error_handler', E_ALL);
set_exception_handler('exception_handler');