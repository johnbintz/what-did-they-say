<input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
<input type="hidden" name="wdts[action]" value="manage_post_transcripts" />
<input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />
<p>
  <label>
    <input type="checkbox" name="wdts[allow_on_post]" value="yes" <?php echo ($transcripts['_allow']) ? 'checked="checked"' : '' ?> />
    <?php _e('Allow new transcripts to be submitted for this post', 'what-did-they-say') ?>
  </label>
</p>
<p>
  <label>
    <?php _e('Edit transcript for:', 'what-did-they-say') ?>
    <select name="wdts[language]" id="wdts-language">
      <?php foreach ($options['languages'] as $code => $info) { ?>
        <option value="<?php echo $code ?>" <?php echo $info['default'] ? 'selected="selected"' : '' ?>><?php echo $info['name'] ?></option>
      <?php } ?>
    </select>
  </label>

  <?php foreach (array_keys($options['languages']) as $code) { ?>
    <textarea class="transcript" id="wdts-transcripts-<?php echo $code ?>" name="wdts[transcripts][<?php echo $code ?>]" style="display: none; width: 100%; height: 200px"><?php echo $transcripts[$code] ?></textarea>
  <?php } ?>
</p>
<script type="text/javascript">
  function switch_transcript() {
    $$('.transcript').each(function(t) {
      (t.id == "wdts-transcripts-" + $F('wdts-language')) ? t.show() : t.hide();
    });
  }
  switch_transcript();
  Event.observe($('wdts-language'), 'change', switch_transcript);
</script>