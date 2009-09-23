<?php
  $pages = array(
    'capabilities' => __('Capabilities', 'what-did-they-say'),
    'default-styles' => __('Styles', 'what-did-they-say'),
    'change-languages' => __('Languages', 'what-did-they-say'),
    'misc-options' => __('Misc. Options', 'what-did-they-say'),
    'shortcodes-info' => __('Shortcodes Info', 'what-did-they-say')
  );
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