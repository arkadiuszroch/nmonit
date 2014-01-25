<?php

final class NM_module_objects extends NM_basic_module
{
    private $_model;
    
    public function init()
    {
        $this->_model = new NM_model_objects();
        $this->add_js_file('js/jQuery.js');
        $this->add_js_file('js/jQuery-ui.js');
        
        $this->add_css_file('css/jQuery-ui.css');
        $this->add_css_file('css/objects.css');
    }
    
    public function action_index()
    {
        $objects = $this->_model->get_used_objects(15, true, true);
        
        $this->add_var('programs', $objects[NM_PROGRAMS]);
        $this->add_var('users', $objects[NM_USERS]);
        $this->add_var('devices', $objects[NM_DEVICES]);
        $this->add_var('remotes', $objects[NM_REMOTES]);
    }
    
    public function action_list()
    {
        $type = NM_loader::get_get('object', NULL);
        
        if(is_null($type) || !in_array($type, array('programs', 'users', 'devices', 'remotes')))
            throw new NM_exception(NM_EX_INVALID_OBJECT_TYPE, 'Ivalid object type', __FILE__, __LINE__);
        
        $objects = $this->_model->get_used_objects(0, (NM_loader::get_get('order', false) == 'usage'), (bool)NM_loader::get_get('desc', false), $type);
        
        $this->add_var($type, $objects[$type]);
        $this->set_view("lists/{$type}");
    }
    
    public function action_details()
    {
        $exists = $valid = true;
        $history = array();
        $object_name = NM_loader::get_get('object', NULL);
        if(is_null($object_name) || !in_array($object_name, array('program', 'user', 'device', 'remote')))
            throw new NM_exception(NM_EX_INVALID_OBJECT_TYPE, 'Ivalid object type', __FILE__, __LINE__);
        
        $object = NULL;
        
        $id = intval(NM_loader::get_get('id', -1));
        
        if($id >= 0)        
        {
            $method = "get_{$object_name}s";
            $objects = NM_objects::get_instance()->$method();
            if(isset($objects[$id]))
            {
                $object = $objects[$id];
                $history = array_reverse($object->get_history());
            } // isset $objects[$id]
            else
            {
                $exists = false;
            }
        } // $id >= 0
        else
        {
            $valid = false;
        }
        
        $this->add_var('valid', $valid);
        $this->add_var('exists', $exists);
        $this->add_var('type', $object_name);
        $this->add_var('id', $id);
        $this->add_var('history', $history);
        $this->add_var($object_name, $object);
        $this->add_var('back', NM_loader::get_server('HTTP_REFERER', '?action=objects'));
        
        $title = '';
        if($object_name == NM_REMOTE)
        {
            $this->add_js_code('$(document).ready(function() {
                $( "#tabs" ).tabs("option","disabled", [1, 2, 3]);
                $( document ).tooltip();
                    });');
            $title = ' title = "This information is unavailable for remote objects"';
        }
        $this->add_var('title', $title);
        
        $this->add_js_file('js/jqPlot/jquery.jqplot.min.js');
        $this->add_js_file('js/jqPlot/plugins/jqplot.pieRenderer.min.js');
        $this->add_js_file('js/jqPlot/plugins/jqplot.dateAxisRenderer.min.js');
        $this->add_js_file('js/jqPlot/plugins/jqplot.highlighter.min.js');
        $this->add_js_file('js/jqPlot/plugins/jqplot.cursor.min.js');
        $this->add_js_file('js/jqPlot/plugins/jqplot.canvasAxisTickRenderer.min.js');
        $this->add_js_file('js/jqPlot/plugins/jqplot.canvasTextRenderer.min.js');
        $this->add_css_file('js/jqPlot/jquery.jqplot.min.css');
        
        $this->add_js_file('js/objects.js');
        
        $this->set_view('details');
    }
    
    public function action_up_down()
    {
        $id = NM_loader::get_get('id', -1);
        $type = NM_loader::get_get('object');
        
        
        $this->set_ajax_var('action', 'download_upload');
        $this->set_ajax_var('data', $this->_model->get_down_up_info($id, $type));
        $this->set_ajax_var('colors', array_slice(get_charts_colors(), 0, 2));
        
        $this->set_ajax(true);
    }
    
    public function action_net_usage()
    {
        $object = NM_loader::get_get('object');
        $id = NM_loader::get_get('id');
        $range_type = NM_loader::get_request('range_type', 'hour');
        $range_start = NM_loader::get_request('range_start', '');
        $range_end = NM_loader::get_request('range_end', '');
        
        $status = true;
        
        if($range_end == '' && $range_start == '')
        {
            $range_start = new NM_date();
            $range_end = new NM_date();
        }
        elseif(($range_end != '' && $range_start == '') || ($range_end == '' && $range_start != ''))
        {
            $status = false;
            $err_msg = 'Please specify both date';
        }
        
        if($status)
        {
            if($range_type == 'min')
            {
                $format = 'Y-m-d H:i';
                $offset = 'PT1M';
                $date_header_format = 'd-m-Y';
                $chart_date_format = '%H:%M<br />%#d %b %Y';
                $date_element_format = 'H:i';
                $date_sufix = ':00';
                $max_range = new DateInterval('PT2H');
                //$max_range = 60 * 60 * 2; // 2 hours
            }
            else if($range_type == 'hour')
            {
                $format = 'Y-m-d H';
                $offset = 'PT1H';
                $date_header_format = 'd-m-Y';
                $date_element_format = 'H';
                $chart_date_format = '%H:00<br />%#d %b %Y';
                $date_sufix = ':00:00';
                $max_range = new DateInterval('P3D');
                //$max_range = 60 * 60 * 24 * 3; // 3 days
            }
            else if($range_type == 'day')
            {
                $format = 'Y-m-d';
                $offset = 'PT24H';
                $date_header_format = 'F Y';
                $date_element_format = 'd';
                $chart_date_format = '%#d %b %Y';
                $date_sufix = ' 00:00:00';
                $max_range = new DateInterval('P3M');
                //$max_range = 60 * 60 * 24 * 30 * 3; // 3 months
            }
            else if($range_type == 'mon')
            {
                $format = 'Y-m';
                $offset = 'P1M';
                $date_header_format = 'Y';
                $date_element_format = 'F';
                $chart_date_format = '%b %Y';
                $date_sufix = '-1 00:00:00';
                $max_range = new DateInterval('P5Y');
                //$max_range = 60 * 60 * 24 * 31 * 12 * 5; // 5 years
            }
            else if($range_type == 'year')
            {
                $format = 'Y';
                $offset = 'P1Y';
                $date_header_format = '';
                $date_element_format = 'Y';
                $chart_date_format = '%Y';
                $date_sufix = '-1-1 00:00:00';
                $max_range = new DateInterval('P20Y');
                //$max_range = 60 * 60 * 24 * 30 * 12 * 20;
            }
            else
            {
                $status = false;
                $err_msg = 'Unrecognized range type';
            }

            if($status)
            {

                if($range_start instanceof NM_date)
                {
                    $start_date = $range_start;
                }
                else
                {
                    // we need full date format to strtotime function
                    $start_time = strtotime($range_start . $date_sufix);
                    $start_date = new NM_date();
                    $start_date->set_time($start_time);
                }

                if($range_end instanceof NM_date)
                {
                    $end_date = $range_end;
                    $end_date->set_time($end_date->get_time() - 60);
                    $start_date->sub_interval($max_range);
                }
                else
                {
                    // we need full date format to strtotime function
                    $end_time = strtotime($range_end . $date_sufix);
                    $end_date = new NM_date();
                    $end_date->set_time($end_time);
                }

                $buf_date = clone $end_date;
                $buf_date->sub_interval($max_range);
                if($buf_date->get_time() > $start_date->get_time())
                {
                    $status = false;
                    $err_msg = 'Range is to large. Max range for %s is %s';
                    if($range_type == 'min')
                        $err_msg = sprintf ($err_msg, 'minutes', '2 hours');
                    else if($range_type == 'hour')
                        $err_msg = sprintf ($err_msg, 'hours', '3 days');
                    else if($range_type == 'day')
                        $err_msg = sprintf ($err_msg, 'days', '3 month');
                    else if($range_type == 'mon')
                        $err_msg = sprintf ($err_msg, 'months', '5 years');
                    else if($range_type == 'year')
                        $err_msg = sprintf ($err_msg, 'years', '20 years');
                }
                else
                {
                    switch($range_type)
                    {
                        case 'year':
                            $end_date->mon = $start_date->mon;
                        case 'mon':
                            $end_date->day = $start_date->day;
                        case 'day':
                            $end_date->hour = $start_date->hour;
                        case 'hour':
                            $end_date->min = $start_date->min;
                        case 'min':
                            $end_date->sec = $start_date->sec;
                    }
                    
                    try
                    {
                        $usage = $this->_model->network_usage($object, $id, $format, $offset, $start_date, $end_date);
                    }
                    catch(NM_exception $e)
                    {
                        $status = false;
                        $err_msg = $e->getMessage();
                    }

                    if($status)
                    {
                        $result = array();

                        $prev_header_date = '';
                        $header_date = '';
                        foreach($usage['rx'] as $i => $info)
                        {
                            $date = $info[0]; // rx and tx date is the same
                            
                            $rx_info = $info[1];
                            $tx_info = $usage['tx'][$i][1];

                            $header_date = date($date_header_format, strtotime($date . $date_sufix));

                            if($prev_header_date != $header_date)
                            {
                                if($prev_header_date != '' && count($result[$prev_header_date]) == 0)
                                    unset($result[$prev_header_date]);
                                $prev_header_date = $header_date;
                            }

                            if(!isset($result[$header_date]))
                                $result[$header_date] = array();

                            if($rx_info > 0 || $tx_info > 0)
                            {
                                $d = date($date_element_format, strtotime($date . $date_sufix));
                                $result[$header_date][$d] = new stdClass();
                                $result[$header_date][$d]->rx = $rx_info;
                                $result[$header_date][$d]->tx = $tx_info;
                            }
                        }

                        if($prev_header_date != '' && count($result[$prev_header_date]) == 0)
                            unset($result[$prev_header_date]);

                        $this->set_ajax_var('status', true);
                        $this->set_ajax_var('format', $chart_date_format);
                        $this->set_ajax_var('action', 'net-usage');
                        $this->set_ajax_var('size_type', size_type_to_string($usage['size_type']));
                        $this->set_ajax_var('size_type_rx', size_type_to_string($usage['size_type_rx']));
                        $this->set_ajax_var('size_type_tx', size_type_to_string($usage['size_type_tx']));
                        $this->set_ajax_var('usage_rx', $usage['rx']);
                        $this->set_ajax_var('usage_tx', $usage['tx']);
                        $this->set_ajax_var('max_size', $usage['max_size']);
                        $this->set_ajax_var('max_size_rx', $usage['max_size_rx']);
                        $this->set_ajax_var('max_size_tx', $usage['max_size_tx']);
                        $this->set_ajax_var('size_type_diff', $usage['size_type_diff']);

                        $this->set_ajax(true);
                        
                        $this->add_var('usage', $result);
                        $this->add_var('size_type', size_type_to_string($usage['size_type']));
                        $this->set_view('usage');
                        return;
                    }
                }
            }
        }
        
        $this->set_ajax(true);
        $this->set_ajax_var('action', 'net-usage');
        $this->set_ajax_var('status', false);
        $this->set_ajax_var('message', $err_msg);
    }
    
    public function action_net_usage_compare()
    {
        $compare_data = $this->_model->get_compare_data(NM_loader::get_get('object'), NM_loader::get_get('id', -1));
        
        $this->add_var('compare_data', $compare_data['result']);
        $this->add_var('types', array('rx' => 'Download', 'tx' => 'Upload'));
        
        $this->set_ajax_var('action', 'net-usage-compare');
        $this->set_ajax_var('charts', $compare_data['charts']);
        $this->set_ajax_var('colors', $compare_data['colors']);
        $this->set_ajax(true);
        
        $this->set_view('compare');
    }
}
