<?php

if(!defined("NMONIT"))
    die("Direct access to this file is not allowed!");

class NM_application extends NM_basic_singleton
{
    private $_action;
    private $_task;
    private $_vars;
    private $_css;
    private $_js;
    private $_file;
    private $_ajax;
    private $_ajax_vars;
    private $_title;
    private $_console_mode;
            
    public function initialize()
    {
        if(isset($_SERVER['argc']) && isset($_SERVER['argv']))
        {
            $argv = $_SERVER['argv'];
            
            if(!isset($argv[1]))
                $argv = '';
            
            if($argv[1] != 'convert-mode')
                die('[***ERROR***] Runnig webservice from console is not allowed' . EOL);
            
            $this->_action = 'index';
            $this->_task = 'converter';
            
            $this->console_mode = true;
            
            ini_set('display_errors', 'E_ALL');
            
            if(isset($argv[2]) && $argv[2] == 'console-debug')
                NM_loader::set_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_PRINT);
            else
                NM_loader::set_server('NM_DEBUG_LEVEL', NM_DEBUG_LEVEL_OFF); // OFF
            
            NM_fb::initialize();
            return;
        }
        
        $this->console_mode = false;
        
        if(!isset($_SESSION['NMonit']) || $_SESSION['NMonit'] !== true)
        {
            $_SESSION['NMonit'] = true;
            NM_cache::clear_all();
        }
        
        $this->_action = NM_loader::get_get('action', 'index');
        if(!preg_match("!^[a-z]+$!", $this->_action))
            $this->_action = 'index';
        
        $this->_task = preg_replace('![^a-z]!', '_', NM_loader::get_get('task', 'index'));
        if(!preg_match("!^[a-z\_]+$!", $this->_task))
            $this->_task = 'index';
        
        $this->_vars = new stdClass();
        $this->_css = array('files' => array(), 'codes' => array());
        $this->_js = array('files' => array(), 'codes' => array());
        $this->_file = '';
        
        $this->_title = 'NMonit webservice';
        
        $this->_ajax = false;
        $this->_ajax_vars = array();
        
        NM_fb::initialize();
    }
    
    public function run()
    {
        $this->add_css_file('css/nmonit.css');
        $this->add_js_file('js/jQuery.js');
        $this->add_js_file('js/nmonit.js');
        
        $class = "NM_module_{$this->_action}";
        $method = "action_{$this->_task}";
        
        if(!class_exists($class, true))
        {
            $class = 'NM_module_index';
            $this->_action = 'index';
        }
        
        $module = new $class();
        
        if(method_exists($module, 'init'))
            $module->init();
        
        if(!method_exists($module, $method))
        {
            $method = 'action_index';
            $this->_task = 'index';
        }
        
        $module->$method();
    }
    
    public function end()
    {
        $cache = NM_cache::get_instance('NM_application');
        $key = md5($_SERVER['REQUEST_URI'] . serialize($_POST));
        if($data = $cache->get($key))
        {
            echo $data;
            return;
        }
        ob_start();
        if($this->_ajax)
            $this->display_ajax();
        else
            $this->display();
        $contents = ob_get_contents();
        ob_end_clean();
        $cache->add($key, $contents, 60);
        echo $contents;
    }
    
    public function add_var($var, $value)
    {
        $this->_vars->$var = $value;
    }
    
    public function add_css_code($code)
    {
        $this->_css['codes'][] = $code;
    }
    
    public function add_css_file($file)
    {
        $this->_css['files'][] = $file;
    }
    
    public function add_js_code($code)
    {
        $this->_js['codes'][] = $code;
    }
    
    public function add_js_file($file)
    {
        $this->_js['files'][] = $file;
    }
    
    public function set_view($file)
    {
        $this->_file = $file;
    }
    
    public function set_ajax($ajax = true)
    {
        $this->_ajax = $ajax;
    }
    
    public function set_ajax_var($var, $value)
    {
        $this->_ajax_vars[$var] = $value;
    }
    
    public function set_title($title = true)
    {
        $this->_title = "{$title} - $this->_title";
    }
    
    public function get_console_mode()    
    {
        return $this->console_mode;
    }
    
    private function get_head()
    {
        $head = "<title>{$this->_title}</title>" . EOL;
        $head .= $this->get_css();
        $head .= $this->get_js();
        
        return $head;
    }
    
    private function get_js()
    {
        $return = '';
        foreach($this->_js['files'] as $file)
        {
            $return .= '<script type = "text/javascript" src = "' . $file . '"></script>' . EOL;
        }
        
        if(count($this->_js['codes']) > 0)
        {
            $return .= '<script type = "text/javascript">' . EOL;
            
            foreach($this->_js['codes'] as $code)
            {
                $return .= $code;
            }
            
            $return .= '</script>' . EOL;
        }
        
        return $return;
    }
    
    private function get_css()
    {
        $return = '';
        foreach($this->_css['files'] as $file)
        {
            $return .= '<link rel = "Stylesheet" type = "text/css" href = "' . $file . '" />' . EOL;
        }
        
        if(count($this->_css['codes']) > 0)
        {
            $return .= '<style type = "text/css>' . EOL;
            
            foreach($this->_css['codes'] as $code)
            {
                $return .= $code;
            }
            
            $return .= '</style>' . EOL;
        }
        
        return $return;
    }
    
    private function display()
    {
        if($this->_file == '')
            $this->_file = 'index';
        ob_start();
        include VIEWS_DIR . 'header' . PHTML_EXT;
        include VIEWS_DIR . $this->_action . DS . $this->_file . PHTML_EXT;
        include VIEWS_DIR . 'footer' . PHTML_EXT;
        $contents = ob_get_contents();
        ob_end_clean();
        
        echo $contents;
    }
    
    private function display_ajax()
    {
        $contents = '';
        if($this->_file != '')
        {
            ob_start();
            include VIEWS_DIR . $this->_action . DS . $this->_file . PHTML_EXT;
            $contents = ob_get_contents();
            ob_end_clean();
        }
        $ajax = array('vars' => $this->_ajax_vars, 'content' => $contents);
        
        echo json_encode($ajax);
    }
}
