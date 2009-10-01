<?php if (current_user_can('edit_themes')) { ?>
  <?php if (is_array($options)) { ?>
    <h3><?php _e('Transcript Style and Display Settings', 'what-did-they-say') ?></h3>
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
        <input type="checkbox"
               name="wdts[default_styles]"
               value="yes"
               <?php echo ($options['load_default_styles'] ? 'checked="checked"' : '') ?> />
        <?php _e('Include default CSS styles for transcripts', 'what-did-they-say') ?>
      </label>

      <label>
        <input type="checkbox"
               name="wdts[use_nl2br]"
               value="yes"
               <?php echo ($options['use_nl2br'] ? 'checked="checked"' : '') ?> />
        <?php _e('Turn transcript line breaks into HTML new lines (nl2br())', 'what-did-they-say') ?>
      </label>
      <input class="button" type="submit" value="<?php _e('Change default styles', 'what-did-they-say') ?>" />
    </form>

    <h3><?php _e('Transcript Line Breaks', 'what-did-they-say') ?></h3>

    <p><?php _e('If you\'re using short codes or HTML to construct your transcripts, you don\'t need to enable transcript line breaks.', 'what-did-they-say') ?></p>

    <h3><?php _e('Default CSS Selectors Information', 'what-did-they-say') ?></h3>

    <p><?php _e('By default, the following CSS selectors are used by <strong>What Did They Say?!?</strong>:', 'what-did-they-say') ?></p>
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

    <h3><?php _e('Overriding Filters', 'what-did-they-say') ?></h3>

    <p>
      <?php _e('If you want to override how transcript HTML is structured, create hooks in your theme that implement the following filters.', 'what-did-they-say') ?>
      <?php _e('All of them return the same parameters that are passed in as an array(), with the return <code>$content</code> containing the filter output.', 'what-did-they-say') ?>
    </p>

    <dl>
      <dt><code>filter_shortcode_dialog($name, $direction, $speech, $content)</code></dt>
      <dd><?php _e('Output <code>$content</code> contains the HTML for dialog blocks.', 'what-did-they-say') ?></dd>

      <dt><code>filter_shortcode_scene_action($description, $content)</code></dt>
      <dd><?php _e('Output <code>$content</code> contains the HTML for scene action blocks.', 'what-did-they-say') ?></dd>

      <dt><code>filter_shortcode_scene_heading($description, $content)</code></dt>
      <dd><?php _e('Ouptut <code>$content</code> contains the HTML for scene heading blocks.', 'what-did-they-say') ?></dd>

      <dt><code>the_media_transcript($transcript, $content)</code></dt>
      <dd><?php _e('Output <code>$content</code> contains the HTML for a single transcript.', 'what-did-they-say') ?></dd>

      <dt><code>the_language_name($language, $content)</code></dt>
      <dd><?php _e('Output <code>$content</code> contains the HTML for a language name.', 'what-did-they-say') ?></dd>

      <dt><code>the_matching_transcript_excerpts($transcripts, $search_string, $content)</code></dt>
      <dd><?php _e('Output <code>$content</code> contains the HTML for all the provided <code>$transcripts</code> that match <code>$search_string</code>.', 'what-did-they-say') ?></dd>
    </dl>

    <p>
      <?php _e('A filter in your theme that would change the display of langauge names would look like the following:', 'what-did-they-say') ?>
    </p>

    <pre>
function my_theme_the_language_name($language, $content) {
  return array($language, "&lt;div class='language-name'&gt;${content}&lt;/div&gt;");
}

add_filter('the_language_name', 'my_theme_the_language_name', 15);</pre>
  <?php } ?>
<?php } ?>
