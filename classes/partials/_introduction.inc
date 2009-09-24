<?php if (current_user_can('submit_transcriptions')) { ?>
  <h3><?php _e('Introduction', 'what-did-they-say') ?></h3>

  <p>
    <?php _e('<strong>What Did They Say?!?</strong> gives you great control over adding and managing transcripts for your comics, videos, or audio.', 'what-did-they-say') ?>
    <?php _e('Adding transcripts to your posts gives you better search engine visibility and helps disabled users enjoy your content.', 'what-did-they-say') ?>
  </p>

  <h3><?php _e('Getting Started', 'what-did-they-say') ?></h3>

  <p>
    <?php _e('<strong>What Did They Say?!?</strong> can attempt to embed excerpts into your posts automatically:', 'what-did-they-say') ?>
    <form method="post">
      <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
      <input type="hidden" name="wdts[module]" value="automatic-transcripts" />
        <label>
        <input type="checkbox"
               name="wdts[automatic_embedding]"
               value="yes"
               <?php echo $options['automatic_embedding'] ? 'checked="checked"' : '' ?> />
          <?php _e('Enable automatic transcript embedding', 'what-did-they-say') ?>
      </label>
      <input type="submit" class="button" value="<?php _e('Submit', 'what-did-they-say') ?>" />
    </form>
  </p>

  <p>
    <?php _e('If the above method doesn\'t work or you desire more control, you can add the following Template Tags to the appropriate location in your posts Loop:', 'what-did-they-say') ?>
  </p>

  <pre>
&lt;?php
  if (function_exists('transcripts_display') &&
      function_exists('the_media_transcript_queue_editor')) {
  transcripts_display(); the_media_transcript_queue_editor();
} ?&gt;</pre>

  <h3><?php _e('Search Results', 'what-did-they-say') ?></h3>

  <p>
    <?php _e('When someone searches your site, <strong>What Did They Say?!?</strong> will search your transcripts, too.', 'what-did-they-say') ?>
    <?php _e('If you want transcript excerpts to appear in your search results, add the following Template Tag to your search results Loop:', 'what-did-they-say') ?>
  </p>

  <pre>
&lt;php if (function_exists('the_matching_transcript_excerpts')) {
  the_matching_transcript_excerpts();
} ?&gt;</pre>

  <p>
    <?php
      $link = '<a href="#" onclick="make_active($(\'wdts-tab-default-styles\')); return false;">' . __('Styles', 'what-did-they-say') . '</a>';
      printf(__('You can set the number of characters to show in context in search results on the %s page.', 'what-did-they-say'), $link);
    ?>
  </p>
<?php } ?>