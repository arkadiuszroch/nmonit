<?php

final class NM_date
{
    public $year;
    public $mon;
    public $day;
    public $hour;
    public $min;
    public $sec;
    
    public function __construct($year = NULL, $mon = NULL, $day = NULL, $hour = NULL, $min = NULL, $sec = NULL)
    {
        if(is_null($year) || !is_int($year))
            $this->year = date('Y');
        else
            $this->year = $year;
        
        if(is_null($mon) || !is_int($mon))
            $this->mon = date('m');
        else
            $this->mon = $mon;
        
        if(is_null($day) || !is_int($day))
            $this->day = date('d');
        else
            $this->day = $day;
        
        if(is_null($hour) || !is_int($hour))
            $this->hour = date('H');
        else
            $this->hour = $hour;
        
        if(is_null($min) || !is_int($min))
            $this->min = date('i');
        else
            $this->min = $min;
        
        if(is_null($sec) || !is_int($sec))
            $this->sec = date('s');
        else
            $this->sec = $sec;
    }
    
    public function set_time($timestamp = NULL)
    {
        $this->year = date('Y', $timestamp);
        $this->mon = date('m', $timestamp);
        $this->day = date('d', $timestamp);
        
        $this->hour = date('H', $timestamp);
        $this->min = date('i', $timestamp);
        $this->sec = date('s', $timestamp);
    }
    
    public function get_time()
    {
        return mktime($this->hour, $this->min, $this->sec, $this->mon, $this->day, $this->year);
    }
    
    public function get_date($format = "H:i:s d-m-Y")
    {
        return date($format, $this->get_time());
    }
    
    public function __sleep()
    {
        $return = array();
        $return[] = 'year';
        $return[] = 'mon';
        $return[] = 'day';
        $return[] = 'hour';
        $return[] = 'min';
        $return[] = 'sec';
        
        return $return;
    }
    
    public function days_of_month($year = NULL, $mon = NULL)
    {
        if($year == NULL)
            $year = $this->year;
        if($mon == NULL)
            $mon = $this->mon;
        
        return date('t', mktime(0, 0, 0, $mon, 1, $year));
    }
    
    public static function maks_days_of_month($year, $mon)
    {
        return date('t', mktime(0, 0, 0, $mon, 1, $year));
    }
    
    public function add_sec($sec)
    {
        $this->set_time($this->get_time() + $sec);
        return $this;
    }
    
    public function add_min($min)
    {
        $this->set_time($this->get_time() + ($min * 60));
        return $this;
    }
    
    public function add_hour($hour)
    {
        $this->set_time($this->get_time() + ($hour * 60 * 60));
        return $this;
    }
    
    public function add_days($days)
    {
        $this->set_time($this->get_time() + ($days * 60 * 60 * 24));
        return $this;
    }
    
    public function add_mon($months)
    {
        $date = new DateTime();
        $date->setTimestamp($this->get_time());
        $interval = new DateInterval("P{$months}m");
        $date->add($interval);
        $this->set_time($date->getTimestamp());
        return $this;
    }
    
    public function add_years($years)
    {
        $this->year += $years;
        return $this;
    }
    
    public function add_interval($format)
    {
        $date = new DateTime();
        $date->setTimestamp($this->get_time());
        if($format instanceof DateInterval)
            $interval = $format;
        else
            $interval = new DateInterval($format);
        $date->add($interval);
        $this->set_time($date->getTimestamp());
        return $this;
    }
    
    public function sub_interval($format)
    {
        $date = new DateTime();
        $date->setTimestamp($this->get_time());
        if($format instanceof DateInterval)
            $interval = $format;
        else
            $interval = new DateInterval($format);
        $date->sub($interval);
        $this->set_time($date->getTimestamp());
        return $this;
    }
}