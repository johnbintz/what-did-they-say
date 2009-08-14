<div class="wrap">
  <h2><?php _e('Manage Transcripts', 'what-did-they-sahy') ?></h2>
  
  <h3><?php _e('Queued Transcripts', 'what-did-they-say') ?></h3>
  
  <h3><?php _e('Languages', 'what-did-they-say') ?></h3>

  <ul id="language-holder">
    <?php foreach ($options['languages'] as $code => $info) {
      $default = isset($info['default']);
      $name = $info['name'];
      ?>
      <li>
        <strong>
          <?php echo $name ?>
          <?php if ($default) { _e('(default)', 'what-did-they-say'); } ?>
        </strong>
        | <a href="<?php echo add_query_arg(array('wdts[code]' => $code, 'wdts[action]' => 'default'), $nonce_url) ?>"><?php _e('Default', 'what-did-they-say') ?></a>
        | <a class="verify" href="<?php echo add_query_arg(array('wdts[code]' => $code, 'wdts[action]' => 'delete'), $nonce_url) ?>"><?php _e('Delete', 'what-did-they-say') ?></a>
      </li>
    <?php } ?>
  </ul>
</div>
<script type="text/javascript">
  var basic_languages = [ <?php echo implode(",", $basic_languages) ?> ];
  var all_languages = [ <?php echo implode(",", $all_languages) ?> ];

  var language_map = { <?php echo implode(",\n", $language_map_pairs) ?> };

  $$('a.verify').each(function(a) {
    Event.observe(a, 'click', function(e) {
      if (!confirm("<?php _e('You are about to delete this language from the available list. Continue?', 'what-did-they-say') ?>")) {
        Event.stop(e);
      }
    });
  });
</script>

