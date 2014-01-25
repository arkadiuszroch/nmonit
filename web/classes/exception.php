<?php

final class NM_exception extends Exception
{
    public function __construct($code, $msg, $file, $line)
    {
        $this->code = $code;
        $this->message = $msg;
        $this->file = $file;
        $this->line = $line;
    }
}
