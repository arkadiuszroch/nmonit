<?php

final class NM_error extends NM_basic_singleton
{
    private $_errors;
    
    protected function __construct()
    {
        $this->_errors = array();
    }
    
    public function add_error($code, $msg, $file, $line, $context, $type)
    {
        $error = new stdClass();
        
        $error->code = $code;
        $error->msg = $msg;
        $error->file = preg_replace('!' . ROOT_DIR . '!', '', $file);
        $error->line = $line;
        $error->context = $context;
        $error->type = $type;
        
        $this->_errors[] = $error;
    }
    
    public function __toString()
    {
        $errors = '';
        
        foreach($this->_errors as $error)
        {
            $errors .= $this->error_template($error->code, $error->msg, $error->file, $error->line, $error->context, $error->type);
        }
        
        $return = <<< HTML
        <div style = "width: 100%; font-size: 12px;">
            <h1 style = "font-size: 20px;">NMonit error handler</h1>
            {$errors}
        </div>
HTML;
        return $return;
    }
    
    private function error_template($code, $msg, $file, $line, $context, $type)
    {
        static $i = 0;
        if(($type == NM_ERR_TYPE_ERROR))
        {
            $type_text = $this->code_to_text($code);
            $context = $this->print_context($context);
            
        }
        else
        {
            $type_text = 'Exception';
            $context = $this->print_trace($context);
        }
        
        $return = <<< HTML
        <div style = "border: 2px solid #555; background: #eee; padding: 10px;">
            <h2 style = "font-size: 15px;">Type: {$type_text}</h2>
            <p><b>Message:</b> {$msg}</p>
            <p><b>In file:</b> {$file}</p>
            <p><b>In line:</b> {$line}</p>
            <div>
                <u onClick = "document.getElementById('NMonit_error_more_{$i}').style.display = (document.getElementById('NMonit_error_more_{$i}').style.display == 'none') ? 'block' : 'none'">
                More info
                <span style = "font-size: 10px;">(click to show / hide)</span></u>:
                <div id = "NMonit_error_more_{$i}" style = "display: none;">
                    {$context}
                </div>
            </div>
        </div>
        <br />
HTML;
        $i++;
        return $return;
    }
    
    private function code_to_text($code)
    {
        switch($code)
        {
            case E_WARNING : 
                return "Warning";
            break;
            case E_NOTICE : 
                return "Notice";
            break;
            case E_DEPRECATED :
                return "Deprecated";
            break;
            case E_STRICT : 
                return "Strict";
            break;
            default: 
                return "Unrecognized error code";
        }
    }
    
    private function print_context($context)
    {
        return '<pre>' . vdp($context) . '</pre>';
    }
    
    private function print_trace($trace)
    {
        $return = '';
        foreach($trace as $array)
        {
            $return .= <<< HTML
        <div style = "width: 800px; margin-left: 20px; padding: 10px; border: 1px solid #555;">
HTML;
            if(isset($array['file']))
            {
                $function = (isset($array['class'])) ? $array['class'] . $array['type'] . $array['function'] : $array['function'];
                $file = preg_replace('!' . ROOT_DIR . '!', '', $array['file']);
                $args = vdp($array['args']);
                
                $return .= <<< HTML
                    <b>File: </b>{$file}<br />
                    <b>Line: </b>{$array['line']}<br />
                    <b>Function: </b>{$function}<br />
                    <b>Args: </b><br /><pre>{$args}</pre>
HTML;
            }
            $return .= <<< HTML
        </div>
HTML;
        }
        
        return $return;
    }
}