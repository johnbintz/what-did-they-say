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
               name="wdts[load_default_styles]"
               value="yes"
               <?php echo ($options['load_default_styles'] ? 'checked="checked"' : '') ?> />
        <?php _e('Include default CSS styles for transcripts', 'what-did-they-say') ?>
      </label>

      <label>
        <input type="checkbox"
               name="wdts[transcript_effects]"
               value="yes"
               <?php echo ($options['transcript_effects'] ? 'checked="checked"' : '') ?> />
        <?php _e('Enable fancy transcript open/close transitions. Increases page download size.', 'what-did-they-say') ?>
      </label>

      <label>
        <input type="checkbox"
               name="wdts[use_nl2br]"
               value="yes"
               <?php echo ($options['use_nl2br'] ? 'checked="checked"' : '') ?> />
        <?php _e('Turn transcript line breaks into HTML new lines (uses <a href="http://us2.php.net/nl2br" target="external">nl2br()</a>)', 'what-did-they-say') ?>
      </label>

      <label>
        <input type="checkbox"
               name="wdts[allow_html]"
               value="yes"
               <?php echo ($options['allow_html'] ? 'checked="checked"' : '') ?> />
        <?php _e('Allow HTML in transcripts. If disabled, only short codes are allowed, which is <strong>recommended</strong>. Script, style, and link tags are <strong>always</strong> filtered out.', 'what-did-they-say') ?>
      </label>

      <p><?php _e('By default, transcripts should start hidden on these types of pages:', 'what-did-they-say') ?></p>

      <div style="margin: 0 2em">
        <label>
          <input type="checkbox"
                 name="wdts[hide_transcript][home]"
                 value="yes"
                 <?php echo ($options['hide_transcript']['home'] ? 'checked="checked"' : '') ?> />
          <?php _e('On the home page', 'what-did-they-say') ?>
        </label>

        <label>
          <input type="checkbox"
                 name="wdts[hide_transcript][single]"
                 value="yes"
                 <?php echo ($options['hide_transcript']['single'] ? 'checked="checked"' : '') ?> />
          <?php _e('On individual post pages', 'what-did-they-say') ?>
        </label>
      </div>

      <?php if (!empty($available_filters)) { ?>
        <label>
          <?php _e('Use the selected transcript filter set:', 'what-did-they-say') ?>
          <select name="wdts[filters_to_use]">
            <option value="__default__">(default)</option>
            <?php foreach ($available_filters as $filter_name) { ?>
              <option value="<?php echo $filter_name ?>"<?php echo ($options['filters_to_use'] == $filter_name) ? ' selected="selected"' : '' ?>><?php echo $filter_name ?></option>
            <?php } ?>
          </select>
        </label>
      <?php } ?>
      
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

      <dt><code>the_transcript_format_string($content)</code></dt>
      <dd><?php _e('Output <code>$content</code> contains the sprintf() format string for how transcript headers will be spelled. The default is <code>Transcript: %s</code>.', 'what-did-they-say') ?></dd>

      <dt><code>the_transcript_opener($content)</code></dt>
      <dd><?php _e('Output <code>$content</code> contains three elements: a container (.wdts-transcript-opener), and an opener &amp; closer in the container (.wdts-opener and .wdts-container).', 'what-did-they-say') ?></dd>
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
