<?php if (current_user_can('manage_options')) { ?>
  <h3><?php _e('Reset Settings to Default', 'what-did-they-say') ?></h3>
  <p><?php _e('Click the button below to reset capabilities and languages to their defaults. This will not affect any transcriptions you have already created, but some transcriptions may become inaccessible if you don\'t redefine the original language.', 'what-did-they-say') ?></p>
  <form method="post" id="reset-wdts">
    <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
    <input type="hidden" name="wdts[module]" value="reset-options" />
    <input type="submit" class="button" value="<?php _e('Reset What Did They Say?!?', 'what-did-they-say') ?>" />
  </form>
<?php } ?>

<script type="text/javascript">
  if ($('reset-wdts')) {
    Event.observe($('reset-wdts'), 'submit', function(e) {
      if (!confirm('<?php _e('Are you sure? You could leave transcriptions inaccessible.', 'what-did-they-say') ?>')) {
        Event.stop(e);
      }
    });
  }
</script>
