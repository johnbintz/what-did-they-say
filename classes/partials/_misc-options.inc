<?php if (current_user_can('manage_options')) { ?>
  <h3><?php _e('Force a High Insertion Level for Automatic Transcript Embedding', 'what-did-they-say') ?></h3>
  <p><?php _e('If you are using automatic transcript embedding and got a message stating that controls could not be loaded, enable this option.', 'what-did-they-say') ?></p>
  <p><?php _e('This is potentially impolite to other plugins and themes, so if you don\'t need it, disable it.', 'what-did-they-say') ?></p>
  <form method="post">
    <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
    <input type="hidden" name="wdts[module]" value="high-insertion-level" />

    <label>
      <input type="checkbox"
             name="wdts[high_insertion_level]"
             value="yes"
             <?php echo ($options['high_insertion_level'] ? 'checked="checked"' : '') ?> />
      <?php _e('Inject transcripts at level 100 instead of level 15', 'what-did-they-say') ?>
    </label>

    <input type="submit" class="button" value="<?php _e('Change Insertion Level', 'what-did-they-say') ?>" />
  </form>
  
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
