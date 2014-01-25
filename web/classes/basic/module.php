<?php

abstract class NM_basic_module
{
    public function __construct()
    {
        
    }
    
    public function action_index()
    {
        
    }
    
    protected function add_var($var, $value)
    {
        NM_application::get_instance()->add_var($var, $value);
    }
    
    protected function add_css_code($code)
    {
        NM_application::get_instance()->add_css_code($code);
    }
    
    protected function add_css_file($file)
    {
        NM_application::get_instance()->add_css_file($file);
    }
    
    protected function add_js_code($code)
    {
        NM_application::get_instance()->add_js_code($code);
    }
    
    protected function add_js_file($file)
    {
        NM_application::get_instance()->add_js_file($file);
    }
    
    protected function set_view($file)
    {
        NM_application::get_instance()->set_view($file);
    }
    
    protected function set_ajax($ajax = true)
    {
        NM_application::get_instance()->set_ajax($ajax);
    }
    
    protected function set_ajax_var($var, $value)
    {
        NM_application::get_instance()->set_ajax_var($var, $value);
    }
    
    protected function set_title($title)
    {
        NM_application::get_instance()->set_title($title);
    }
}