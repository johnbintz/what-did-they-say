<?php
  $pages = array(
    'introduction' => __('Introduction', 'what-did-they-say'),
    'capabilities' => __('Capabilities', 'what-did-they-say'),
    'default-styles' => __('Styles', 'what-did-they-say'),
    'change-languages' => __('Languages', 'what-did-they-say'),
    'shortcodes-info' => __('Shortcodes Info', 'what-did-they-say'),
    'misc-options' => __('Misc. Options', 'what-did-they-say'),
  );

  extract($this->plugin_data);
?>
<div class="wrap">
  <h2><?php _e('What Did They Say?!?', 'what-did-they-say') ?></h2>

  <div id="wdts-tab-bar">
    <?php foreach ($pages as $page => $title) { ?><a id="wdts-tab-<?php echo $page ?>" href="#" class="wdts-tab"><?php echo $title ?></a><?php } ?>
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
      <a href="<?php echo $PluginURI ?>"><?php _e('What Did They Say?!?', 'what-did-they-say') ?></a> Version <?php echo $Version ?> is &copy; 2009 <?php echo $Author ?>.
      <?php printf(__('Might I suggest a $%0.2f donation?', 'what-did-they-say'), $suggested_amount) ?>
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

  <?php if (isset($_POST['wdts']['return_page'])) { ?>
    make_active($('<?php echo str_replace('-page-', '-tab-', $_POST['wdts']['return_page']) ?>'));
  <?php } else { ?>
    make_active($$('.wdts-tab')[0]);
  <?php } ?>
</script>