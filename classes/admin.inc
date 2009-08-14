<div class="wrap">
  <h2>Manage Transcripts</h2>
  
  <h3>Queued Transcripts</h3>
  
  <h3>Languages</h3>

  <ul id="language-holder">
    <?php foreach ($options['languages'] as $code => $info) {
      $default = isset($info['default']);
      $name = $info['name'];
      ?>
      <li>
        <strong>
          <?php echo $name ?>
          <?php if ($
        </strong>
        | <a class="verify" href="<?php echo add_query_arg(array('wdts[code]' => $code, 'wdts[action]' => 'delete'), $nonce_url) ?>">Delete</a>
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
      if (!confirm("You are about to delete this language from the available list. Continue?")) {
        Event.stop(e);
      }
    });
  });
</script>

