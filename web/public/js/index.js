jQuery(function()
{
    if(jQuery('#main-loader').length)
    {
        jQuery.ajax({
            url: '?action=index&task=load-data',
            dataType: 'json',
            success: function(response)
            {
                jQuery('#main-loader').fadeOut();
                jQuery('#index-content').html(response['content']);

                show_charts('Download', 'programs_rx', response['vars']['programs']['charts']['rx'], response['vars']['programs']['colors']['rx']);
                show_charts('Upload', 'programs_tx', response['vars']['programs']['charts']['tx'], response['vars']['programs']['colors']['tx']);

                show_charts('Download', 'users_rx', response['vars']['users']['charts']['rx'], response['vars']['users']['colors']['rx']);
                show_charts('Upload', 'users_tx', response['vars']['users']['charts']['tx'], response['vars']['users']['colors']['tx']);

                show_charts('Download', 'devices_rx', response['vars']['devices']['charts']['rx'], response['vars']['devices']['colors']['rx']);
                show_charts('Upload', 'devices_tx', response['vars']['devices']['charts']['tx'], response['vars']['devices']['colors']['tx']);
            }
        });
    }
});

function show_charts(title, object, data, colors)
{
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