var NULL = undefined;

$.urlParam = function(name){
    var results = new RegExp('[\\?&]' + name + '=([^&#]*)').exec(window.location.href);
    if (results==null){
        return null;
    }else{
        return results[1] || 0;
    }
}

$(function() 
{
    $( "#tabs" ).tabs(
    {
        cache: true,
        /*beforeLoad: function( event, ui ) 
        {
            ui.jqXHR.error(function() 
            {
                ui.panel.html(
                    "Couldn't load this tab. We'll try to fix this as soon as possible. " +
                    "If this wouldn't be a demo." );
            });
        },*/
        ajaxOptions: 
        {
            dataFilter: function(result)
            {
                var data = $.parseJSON(result);
                
                load_ajax_data(data);
            },
            cache: false
        },
        select:function (e, ui) 
        {
            var panel = $(ui.panel);

            if (panel.is(":empty")) 
            {
                panel.append('<h1>Please wait. Loading...</h1><img src = "images/main-loader.gif" border = "0" /><p class = "loader-desc">It may take a few seconds for a large amount of data</p>');
            }
        }
    });
});

jQuery(document).on('click', 'input[name=load]', function()
{
    var err = jQuery('div.err_msg');
    err.fadeOut(300);
      
    var range_type = jQuery('select#type').val();
    var range_start = jQuery('input[name=range_start]').val();
    var range_end = jQuery('input[name=range_end]').val();
    
    var img = jQuery('img#net-graph-loader');
    var button = jQuery(this);
    
    button.attr('disabled', 'disabled');
    img.fadeIn(200);
    
    
    jQuery.ajax({
        url: '?action=objects&task=net-usage&object=' + $.urlParam('object') + '&id=' + $.urlParam('id'),
        type: 'POST',
        data:
        {
            range_type: range_type,
            range_start : range_start,
            range_end : range_end
        },
        success: function(result)
        {
            var data = jQuery.parseJSON(result);
            
            load_ajax_data(data);
            
            jQuery('select#type').val(range_type);
            jQuery('select#type').trigger('change');
            jQuery('input[name=range_start]').val(range_start);
            jQuery('input[name=range_end]').val(range_end);
            jQuery('a#filter_text').trigger('click');
            button.attr('disabled', false);
            img.fadeOut(200);
        }
    });
});

function load_ajax_data(data)
{
    if(data['vars']['action'] == 'download_upload')
    {
        var ui = $('#ui-tabs-1');
        ui.html('<div id = "net_usage"></div>');
        show_chart(data['vars']['data'], 'net_usage', 'Download / Upload', data['vars']['colors']);
    }
    else if(data['vars']['action'] == 'net-usage')
    {
        var ui = $('#ui-tabs-2');

        if(data['vars']['status'] == false)
        {
            var err = $('div.err_msg');
            if(err.length == 0)
            {
                ui.html('<div class = "err_msg"></div>');
                err = $('div.err_msg');
            }

            err.html(data['vars']['message']);
            err.fadeIn(300);
        }
        else
        {
            ui.html(data['content']);

            window.nmonit.net_usage = data['vars'];
            
            jQuery('input[type=radio][name=data_type]').trigger('change');
            
            //show_data_chart('usage_upload_chart',data['vars']['usage_tx']['data'] , 'Upload', data['vars']['format'], data['vars']['usage']['tx']['size_type'], data['vars']['usage']['tx']['max_size'])
        }
    }
    else if(data['vars']['action'] == 'net-usage-compare')
    {
        var ui = $('#ui-tabs-3');

        ui.html(data['content']);
        var types = {rx: 'download', tx: 'upload'};
        
        jQuery.each(data['vars']['charts'], function(type, compare_object){
            jQuery.each(compare_object, function(object_name, values)
            {
                show_chart(values, object_name + '_' + types[type], 'Download by ' + object_name, data['vars']['colors'][type][object_name]);
            });
        });
    }
}


function show_chart(data, object, title, colors)
{   
    if(colors == null)
        colors = new Array();
    
    var plot1 = jQuery.jqplot (object, [data],
    {
        seriesDefaults: 
        {
            // Make this a pie chart.
            renderer: jQuery.jqplot.PieRenderer,
            rendererOptions: 
            {
                // Put data labels on the pie slices.
                // By default, labels show the percentage of the slice.
                showDataLabels: true
            }
        },
        seriesColors : colors,
        title: 
        {
            text: title,
            show: true
        },
        
        legend: 
        { 
            show:true, 
            location: 'e'
        }
    });
}

function show_usage_chart(element, rx, tx, title, format, size_type, max_size, size_diff)
{
    var data;
    var labels;
    var colors;
    
    if(rx == NULL)
    {
        data = [tx]
        labels = ['Upload'];
        colors = ['#EAA228'];
    }
    else if(tx == NULL)
    {
        data = [rx];
        labels = ['Download'];
        colors = ['#4bb2c5'];
    }
    else
    {
        data = [rx, tx];
        labels = ['Download', 'Uploads'];
        colors = ['#4bb2c5', '#EAA228'];
    }
    
    if((rx == NULL || tx == NULL) && size_diff > 0)
    { 
        var buf = (rx == NULL) ? tx : rx;
        // metoda slice nie chciała działać więc oprócz zmiany wartości trzeba także przepisać całą tablicę
        data = new Array(new Array());
        jQuery(buf).each(function(index, value){
            data[0][index] = new Array();
            data[0][index][0] = value[0];
            data[0][index][1] = value[1] * Math.pow(1024, size_diff);
        });
    }
    
    var plot2 = $.jqplot(element, data,
    {
        title: title,
        axes:{
            xaxis:{
                renderer:$.jqplot.DateAxisRenderer,
                tickOptions:{formatString: format}
            },
            yaxis:{
                tickOptions:{
                  formatString:'%.2f ' + size_type
                },
                min: 0.00,
                max: max_size,
                numberTicks: 6
            }
        },
        seriesDefaults:{
                lineWidth:2,
                showMarker: false,
                pointLabels: { show:true },
                markerOptions: {
                    show: false         
                }
            },
        seriesColors: colors,
        highlighter: {
            show: true,
            sizeAdjust: 7.5
        },
        cursor: {
            show: true,
            tooltipLocation:'ne'
        },
        legend: {
            show: true,
            labels: labels,
            placement: 'outsideGrid',
            location: 'ne',     // compass direction, nw, n, ne, e, se, s, sw, w.
            xoffset: 50,        // pixel offset of the legend box from the x (or x2) axis.
            yoffset: 12       // pixel offset of the legend box from the y (or y2) axis.
        }
    });
}


jQuery(document).on('click', 'a#filter_text', function(){
    
    if($('span#filter_arrow').html() == '/\\')
    {
        $('#filter').slideUp(200, function()
        {
            $('span#filter_arrow').html('\\/');
        });
    }
    else
    {
        $('#filter').slideDown(200, function()
        {
            $('span#filter_arrow').html('/\\');
        });
    }
});

jQuery(document).on('change', 'select#type', function(){

    var value = jQuery('select#type').val();
    var tip = jQuery('span#date_format');
    if(value == 'min')
        tip.html('yyyy-mm-dd hh:mm');
    else if(value == 'hour')
        tip.html('yyyy-mm-dd hh');
    else if(value == 'day')
        tip.html('yyyy-mm-dd');
    else if(value == 'mon')
        tip.html('yyyy-mm');
    else if(value == 'year')
        tip.html('yyyy');
    else
        tip.html('Unrecognized range type');
});

jQuery(document).on('change', 'input[type=radio][name=data_type]:checked', function(){
    
    var val = jQuery(this).val();
    var data = window.nmonit.net_usage;
    
    jQuery('#usage_download_chart').html('');
    
    if(val == 'download')
        show_usage_chart('usage_download_chart', data['usage_rx'], NULL, 'Download', data['format'], data['size_type_rx'], data['max_size_rx'], data['size_type_diff']['rx']);
    else if(val == 'upload')
    {
        var rx = data['usage_tx'][0];
        rx[1] = 0.0;
        show_usage_chart('usage_download_chart', NULL, data['usage_tx'], 'Upload', data['format'], data['size_type_tx'], data['max_size_tx'], data['size_type_diff']['tx']);
    }
    else
        show_usage_chart('usage_download_chart', data['usage_rx'], data['usage_tx'], 'Download & Upload', data['format'], data['size_type'], data['max_size'], 0)
    
});