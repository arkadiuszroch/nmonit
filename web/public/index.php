<?php

define('NMONIT', true);

header('Content-type: text/html; charset=UTF-8');

$root_directory = preg_replace('!public/index.php!', '', $_SERVER['SCRIPT_FILENAME']);

define('ROOT_DIR', $root_directory);

unset($root_directory);

define('FUNC_DIR', ROOT_DIR . 'functions/');

// loading functions 
if(!include FUNC_DIR . 'functions.php')
    die('Failed to load functions.php file');

defined('NM_APPLICATION_ENV')
    || define('NM_APPLICATION_ENV', (getenv('NM_APPLICATION_ENV')) ?: NM_ENV_PRODUCTION);

session_start();

$app = NM_application::get_instance();

$app->initialize();

$app->run();

$app->end();
