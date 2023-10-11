$(function () {
    // init layout
    var main_layout = $('#main_layout').w2layout({
        name: 'main_layout',
        panels: [
            { type: 'top', size: 45, style: 'border: 0px; border-bottom: 1px solid silver; background-color: #fff; color: #555;', overflow: 'hidden'},
            { type: 'left', size: 230, resizable: true, style: 'border-right: 1px solid silver;'
             , tabs: {
                    name:   'left_tabs',
                    style:  'background-color: #F0F0F0',
                    active: 'sidebar-live',
                    tabs: [
                        { id: 'sidebar-live', caption: 'Live' },
                        { id: 'sidebar-cfg', caption: 'Config' },
                        { id: 'sidebar-vod', caption: 'Vod' }
                    ],
                    onClick: function (event) {

                        var cmd = event.target;
                        //if (parseInt(cmd.substr(cmd.length-1)) != cmd.substr(cmd.length-1)) return;
                        if(flv_destroy)
                          flv_destroy();
                          
                        load_html('left', cmd);
                    }
               }
            },
            { type: 'main', style: 'background-color: Snow;'
            },
            { type: 'channel', hidden: true, content: '0'}
        ]
    });
    
    w2ui['main_layout'].content('top', '<div style="padding: 12px 20px; font-size: 18px; background-color: SkyBlue;">HiView Webs</div>');
    load_html('left', 'sidebar-live');

});

function load_html(panel, cmd) {
    // load example
    $.get('html/'+ cmd +'.html', function (data) {
        var tmp = data.split('<!--CODE-->');
        if (tmp.length == 1) {
            alert('ERROR: cannot parse html.');
            console.log('ERROR: cannot parse html.', data);
            return;
        }
        var html = tmp[1] ? $.trim(tmp[1]) : '';
        var js   = tmp[2] ? $.trim(tmp[2]) : '';
        var css  = tmp[3] ? $.trim(tmp[3]) : '';
        var json = tmp[4] ? $.trim(tmp[4]) : '';
        js = js.replace(/^<script[^>]*>/, '').replace(/<\/script>$/, '');
        js = $.trim(js);
        css = css.replace(/^<style[^>]*>/, '').replace(/<\/style>$/, '');
        css = $.trim(css);
        json = json.replace(/^<script[^>]*>/, '').replace(/<\/script>$/, '');
        json = $.trim(json);
        
        w2ui['main_layout'].content(panel, tmp[0]);
     
        if(panel == 'main')
        {
          $('#main_view').html(
                   html +
                  '<script type="text/javascript">' + js + '</script>' +
                  '<style>' + css + '</style>');
        }
        else
        {
          $('#example_view').html(
                  html +
                  '<script type="text/javascript">' + js + '</script>' +
                  '<style>' + css + '</style>');
        }
                
    });
}


