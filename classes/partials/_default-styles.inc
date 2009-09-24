<?php if (current_user_can('edit_themes')) { ?>
  <?php if (is_array($options)) { ?>
    <h3><?php _e('Transcript Style and DisplaySettings', 'what-did-they-say') ?></h3>
    <form method="post">
      <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
      <input type="hidden" name="wdts[module]" value="styles" />
      <label>
        <?php _e('Number of excerpt context characters in search results:', 'what-did-they-say') ?>
        <input type="text"
               name="wdts[excerpt_distance]"
               value="<?php echo $options['excerpt_distance'] ?>"
               size="3" />
      </label>
      
      <label>
        <?php _e('Include default CSS styles for transcripts:', 'what-did-they-say') ?>
        <input type="checkbox"
               name="wdts[default_styles]"
               value="yes"
               <?php echo ($options['load_default_styles'] ? 'checked="checked"' : '') ?> /> 
      </label>
      <input class="button" type="submit" value="<?php _e('Change default styles', 'what-did-they-say') ?>" />
    </form>
    <hr />
    <p><?php _e('By default, the following styles are used by <strong>What Did They Say?!?</strong>:', 'what-did-they-say') ?></p>
    <dl>
      <dt><code>.transcript</code></dt>
      <dd><?php _e('The container for the transcript', 'what-did-they-say') ?></dd>
      <dt><code>.dialog</code></dt>
      <dd><?php _e('Character dialog', 'what-did-they-say') ?></dd>
      <dt><code>.name</code></dt>
      <dd><?php _e('The character\'s name', 'what-did-they-say') ?></dd>
      <dt><code>.direction</code></dt>
      <dd><?php _e('The direction the characters is speaking in/from (off-stage, to another character)', 'what-did-they-say') ?></dd>
      <dt><code>.scene-heading</code></dt>
      <dd><?php _e('A scene heading', 'what-did-they-say') ?></dd>
      <dt><code>.scene-action</code></dt>
      <dd><?php _e('Action within a scene', 'what-did-they-say') ?></dd>
    </dl>

    <p><?php _e('To include transcript excerpts in your search results, add the following template tag to your search results Loop:', 'what-did-they-say') ?></p>
    <p><code>&lt;php the_matching_transcript_excerpts() ?&gt;</code></p>
    <p><?php printf(__('The first matching word in each transcript will be <strong>emphasized</strong>, and there will be <strong>%d</strong> additional characters on each side of the word.', 'what-did-they-say'), $options['excerpt_distance']) ?></p>
  <?php } ?>
<?php } ?>
