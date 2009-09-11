<input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
<input type="hidden" name="wdts[module]" value="manage-post-transcripts" />
<input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />
<p>
  <label>
    <input type="checkbox" name="wdts[allow_on_post]" value="yes" <?php echo ($transcripts['_allow']) ? 'checked="checked"' : '' ?> />
    <?php _e('Allow new transcripts to be submitted for this post', 'what-did-they-say') ?>
  </label>
</p>
<?php if (is_array($queued_transcripts) && !empty($queued_transcripts)) { ?>
  <p><strong><?php _e('Manage queued transcripts:', 'what-did-they-say') ?></strong></p>
  <?php foreach ($queued_transcripts as $transcript) {
    $user = get_userdata($transcript->user_id);
    if (!empty($user)) { ?>
  
      <div class="queued-transcription-holder">
        <input type="hidden" name="wdts[queue][<?php echo $transcript->id ?>]" value="keep" />
        <input type="hidden" name="wdts[language][<?php echo $transcript->id ?>]" value="<?php echo $transcript->language ?>" />
        <p><?php
          printf(
            __('From <strong>%s</strong> in <strong>%s</strong>:', 'what-did-they-say'),
            $user->display_name,
            $this->what_did_they_say->get_language_name($transcript->language)
          )
        ?></p>
        <div class="queued-transcription"><?php echo $transcript->transcript ?></div>
        <div class="queued-transcription-raw" style="display:none"><?php echo $transcript->transcript ?></div>
        <div style="padding: 10px 0">
          <a href="#" class="approve-transcript button">Approve</a>
          <a href="#" class="delete-transcript button">Delete</a>
        </div>
      </div>
    <?php } ?>
  <?php } ?>
<?php } ?>
<p>
  <label>
    <?php _e('Edit provided transcript for:', 'what-did-they-say') ?>
    <select name="wdts[language]" id="wdts-language">
      <?php foreach ($options['languages'] as $code => $info) { ?>
        <option value="<?php echo $code ?>" <?php echo $info['default'] ? 'selected="selected"' : '' ?>><?php echo $info['name'] ?></option>
      <?php } ?>
    </select>
  </label>

  <?php foreach (array_keys($options['languages']) as $code) { ?>
    <textarea class="transcript" id="wdts-transcripts-<?php echo $code ?>" name="wdts[transcripts][<?php echo $code ?>]" style="display: none; width: 100%; height: 200px"><?php echo $approved_transcripts[$code] ?></textarea>
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

  $$('.approve-transcript').each(function(b) {
    Event.observe(b, 'click', function(e) {
      Event.stop(e);
      var lang = b.parentNode.parentNode.select("input[name*=[language]]").shift();
      if (lang) {
        lang = lang.value;
        var editor = $('wdts-transcripts-' + lang);

        var raw_transcript = b.parentNode.parentNode.select(".queued-transcription-raw").shift();
        if (raw_transcript && editor) {
          var ok = true;
          if (editor.value.match(/[^ ]/)) {
            ok = confirm('<?php _e('This will overwrite the current transcript. Are you sure?', 'what-did-they-say') ?>');
          }
          if (ok) {
            editor.value = raw_transcript.innerHTML;
            var p = b.parentNode.parentNode;
            new Effect.Fade(p, {
              'afterFinish': function() { p.parentNode.removeChild(p); }
            });
          }
        }
      }
    });
  });

  $$('.delete-transcript').each(function(b) {
    Event.observe(b, 'click', function(e) {
      Event.stop(e);

      if (confirm('<?php _e('This will delete the queued transcript. Are you sure?', 'what-did-they-say') ?>')) {
        var p = b.parentNode.parentNode;
        new Effect.Fade(p, {
          'afterFinish': function() { p.parentNode.removeChild(p); }
        });
      }
    });
  });
</script>