<input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
<input type="hidden" name="wdts[module]" value="manage-post-transcripts" />
<input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />
<p>
  <label>
    <input type="checkbox" name="wdts[allow_on_post]" value="yes" <?php echo ($transcript_options->are_new_transcripts_allowed()) ? 'checked="checked"' : '' ?> />
    <?php _e('Allow new transcripts to be submitted for this post', 'what-did-they-say') ?>
  </label>
</p>
<?php if (is_array($queued_transcripts) && !empty($queued_transcripts)) { ?>
  <p><strong><?php _e('Manage queued transcripts:', 'what-did-they-say') ?></strong></p>
  <?php foreach ($queued_transcripts as $transcript) {
    $user = get_userdata($transcript['user_id']);
    if (!empty($user)) { ?>
  
      <div class="queued-transcription-holder">
        <input type="hidden" name="wdts[language]" value="<?php echo $transcript['language'] ?>" />
        <input type="hidden" name="wdts[key]" value="<?php echo $transcript['key'] ?>" />
        <p><?php
          printf(
            __('From <strong>%s</strong> in <strong>%s</strong>:', 'what-did-they-say'),
            $user->display_name,
            $language_options->get_language_name($transcript['language'])
          )
        ?></p>
        <div class="queued-transcription transcript"><?php echo do_shortcode($transcript['transcript']) ?></div>
        <div class="queued-transcription-raw" style="display:none"><?php echo $transcript['transcript'] ?></div>
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

  <div id="wdts-shorttags">
    <button class="wdts-create" id="wdts-scene-heading">Scene Heading</button>
    <button class="wdts-create" id="wdts-scene-action">Scene Action</button>
    <button class="wdts-create" id="wdts-dialog">Dialog</button>

    <?php foreach (array_keys($options['languages']) as $code) {
      $approved_transcript_text = '';
      foreach ($approved_transcripts as $transcript) {
        if ($transcript['language'] == $code) { $approved_transcript_text = $transcript['transcript']; break; }
      } ?>
      <textarea class="edit-transcript" id="wdts-transcripts-<?php echo $code ?>" name="wdts[transcripts][<?php echo $code ?>]" style="display: none; width: 99%; height: 200px"><?php echo $approved_transcript_text ?></textarea>
    <?php } ?>
  </div>
</p>