<?php

if(!defined("NMONIT"))
    die("Direct access to this file is not allowed!");

define('NM_ENV_PRODUCTION', 'production');
define('NM_ENV_DEVELOPMENT', 'development');

// directory separator
define('DS', '/');

// class name separator. replaced to directory separator.
define('CS', '_');

// we need double quotes here because only in double quotes the escape sequences are recognized
define('EOL', "\r\n");

define('PUBLIC_DIR', ROOT_DIR . 'public' . DS);
define('CLASS_DIR', ROOT_DIR . 'classes' . DS);
//define('FUNC_DIR', ROOT_DIR . 'functions' . DS); - defined in index.php
define('MODULES_DIR', ROOT_DIR . 'modules' . DS);
define('MODELS_DIR', ROOT_DIR . 'models' . DS);
define('VIEWS_DIR', ROOT_DIR . 'views' . DS);
define('DATA_DIR', ROOT_DIR . 'data' . DS);
define('CACHE_DIR', ROOT_DIR . 'cache' . DS);
define('DATA_PROGRAMS_DIR', ROOT_DIR . 'data' . DS . 'programs' . DS);
define('DATA_USERS_DIR', ROOT_DIR . 'data' . DS . 'users' . DS);
define('DATA_DEVICES_DIR', ROOT_DIR . 'data' . DS . 'devices' . DS);
define('DATA_REMOTES_DIR', ROOT_DIR . 'data' . DS . 'remotes' . DS);
define('DAEMON_DATA_DIR', ROOT_DIR . '..' . DS .'data' . DS);
define('DATABASE_FILE', DAEMON_DATA_DIR . 'database.sql');

// php files extension
define('PHP_EXT', '.php');

// html files extension
define('HTML_EXT', '.phtml');
define('PHTML_EXT', '.phtml');

// unpack types
// in daemon NM_uint is unsigned short integer
define('NM_UNPACK_NM_UINT', 'S');
define('NM_UNPACK_DOUBLE', 'd');
// unpack don't support string but we use it in NM_fread function to don't use unpack when this parameter is set.
define('NM_UNPACK_STRING', 'string');

define('NM_PROGRAM', 'program');
define('NM_PROGRAMS', 'programs');
define('NM_USER', 'user');
define('NM_USERS', 'users');
define('NM_DEVICE', 'device');
define('NM_DEVICES', 'devices');
define('NM_REMOTE', 'remote');
define('NM_REMOTES', 'remotes');

// use in classes/objects.php
define('PC_INFO_FILE', DATA_DIR . 'pcInfo');
define('DAEMON_PC_INFO_FILE', DAEMON_DATA_DIR . '.pcInfo');

define('NM_UINT_SIZE', 2);
define('NM_DOUBLE_SIZE', 8);
// enum NM_info_pc_objects
define('NM_OBJECT_DEVICE', 1);
define('NM_OBJECT_REMOTE', 2);
define('NM_OBJECT_PROGRAM', 3);
define('NM_OBJECT_USER', 4);
define('NM_DATE', 5);
define('NM_HOUR', 6);

// use in classes/error.php
define('NM_ERR_TYPE_ERROR', 1);
define('NM_ERR_TYPE_EXCEPTION', 2);

define('NM_DEBUG_LEVEL_STOP', 1);
define('NM_DEBUG_LEVEL_PRINT', 2);
define('NM_DEBUG_LEVEL_FILE', 3);
define('NM_DEBUG_LEVEL_OFF', 4);

// exceptions 
define('NM_EX_DAEMON_PC_INFO_READ_FAILED', 1);
define('NM_EX_DAEMON_DATA_READ_FAILED',2);
define('NM_EX_INVALID_OBJECT_TYPE', 3);
define('NM_EX_ELEMENT_DOESNT_EXISTS', 4);
define('NM_EX_OBJECT_DOESNT_EXISTS', 5);
define('NM_EX_UNSERIALIZE_FAILED', 6);
define('NM_EX_FILE_DOESNT_EXISTS', 7);
define('NM_EX_DATA_READ_FAILED', 8);
define('NM_EX_SUMMARY_COMPARE_NO_ID', 9);
define('NM_EX_WRONG_DATA', 10);
define('NM_EX_DB_ERROR', 11);

// default addresses
define('NM_DEF_ADDRESS', '0.0.0.0');
define('NM_DEF_ADDRESS6', '::0');

// size types
define('NM_SIZE_B', 1);
define('NM_SIZE_KB', 2);
define('NM_SIZE_MB', 3);
define('NM_SIZE_GB', 4);
define('NM_SIZE_TB', 5);

// cache storage flags
define('NM_CACHE_FILE', 1);
define('NM_CACHE_VAR', 2);