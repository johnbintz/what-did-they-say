<?php
  $pages = array(
    'introduction' => __('Introduction', 'what-did-they-say'),
    'how-it-works' => __('How It Works', 'what-did-they-say'),
    'capabilities' => array(__('Capabilities', 'what-did-they-say'), 'edit_users'),
    'default-styles' => array(__('Styles', 'what-did-they-say'), 'edit_themes'),
    'change-languages' => array(__('Languages', 'what-did-they-say'), 'change_languages'),
    'shortcodes-info' => array(__('Short Codes Info', 'what-did-they-say'), 'submit_transcriptions'),
    'misc-options' => array(__('Misc. Options', 'what-did-they-say'), 'manage_options'),
  );

  extract($this->plugin_data);
?>
<div class="wrap">
  <h2><?php _e('What Did They Say?!?', 'what-did-they-say') ?></h2>

  <div id="wdts-tab-bar">
    <?php foreach ($pages as $page => $title) {
      $ok = true;
      if (is_array($title)) {
        $ok = current_user_can(end($title));
        $title = reset($title);
      }

      if ($ok) {
        ?><a id="wdts-tab-<?php echo $page ?>" href="#" class="wdts-tab"><?php echo $title ?></a>
      <?php }
    } ?>
  </div>

  <div id="wdts-container">
    <?php foreach ($pages as $page => $title) { ?>
      <div id="wdts-page-<?php echo $page ?>" class="wdts-page" style="display:none">
        <?php include("_${page}.inc") ?>
      </div>
    <?php } ?>
  </div>

  <div id="wdts-footer">
    <form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="donate">
    <input type="hidden" name="cmd" value="_s-xclick">
    <input type="hidden" name="hosted_button_id" value="8429526">
    <input type="image" src="https://www.paypal.com/en_US/i/btn/btn_donate_SM.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
    <img alt="" border="0" src="https://www.paypal.com/en_US/i/scr/pixel.gif" width="1" height="1">
    </form>

    <p>
			<?php
			  printf(
				  __('%1$s Version %2$s is &copy; 2009 %3$s.', 'what-did-they-say'),
					'<a href="' . $PluginURI . '">' . __('What Did They Say?!?', 'what-did-they-say') . '</a>',
					$Version,
					$Author
			  );
		  ?>
      <?php printf(__('Might I suggest a $%1$0.2f donation for your %2$d transcripts?', 'what-did-they-say'), $suggested_amount, $this->transcript_count) ?> |
			<a href="http://bugs.comicpress.org/index.php?project=3&amp;do=index"><?php _e('File a bug', 'what-did-they-say') ?></a>
    </p>
  </div>
</div>

<script type="text/javascript">
  function make_active(tab) {
    if (tab) {
      var page = $(tab.id.replace('-tab-', '-page-'));
      if (page) {
        $$('.wdts-tab').each(function(t) {
          (t == tab) ? t.removeClassName('inactive') : t.addClassName('inactive');
        });

        $$('.wdts-page').each(function(p) {
          if (p == page) {
            if (!p.visible()) {
              new Effect.BlindDown(p, { duration: 0.25 });
            }
          } else {
            if (p.visible()) {
              new Effect.BlindUp(p, { duration: 0.25 });
            }
          }
        });
      }
    }
  }

  $$('.wdts-tab').each(function(tab) {
    tab.observe('click', function(e) {
      Event.stop(e);
      make_active(tab);
    });
  });

  $$('.wdts-page').each(function(page) {
    page.select('form').each(function(form) {
      var return_page = new Element("input", { type: 'hidden', name: 'wdts[return_page]', value: page.id });
      form.insert(return_page);
    });
  });

  var iframe_url = '<?php echo plugin_dir_url(realpath(dirname(__FILE__) . '/../..'))  . 'what-did-they-say.php' ?>';

  var load_sample_transcript = function() {
    var which = $('wdts-filters-to-use').options[$('wdts-filters-to-use').selectedIndex].value;
    $('wdts-sample-transcript-holder').innerHTML = '';
    var iframe = new Element('iframe', { width: "100%", height: "200px", src: iframe_url + '?wdts[filter_preview]=' + which });
    $('wdts-sample-transcript-holder').insert(iframe);
  };

  $('wdts-filters-to-use').observe('change', load_sample_transcript);
  $('refresh-filter-preview').observe('click', function(e) {
    Event.stop(e);
    load_sample_transcript();
  });

  Event.observe(window, 'load', load_sample_transcript);

  <?php if (isset($_POST['wdts']['return_page'])) { ?>
    make_active($('<?php echo str_replace('-page-', '-tab-', $_POST['wdts']['return_page']) ?>'));
  <?php } else { ?>
    make_active($$('.wdts-tab')[0]);
  <?php } ?>
</script>