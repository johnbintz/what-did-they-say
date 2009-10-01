<input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />

<label>
  <input id="wdts-<?php echo $checkbox_id = md5(rand()) ?>" type="checkbox" name="wdts[allow_on_post]" value="yes" <?php echo ($transcript_options->are_new_transcripts_allowed()) ? 'checked="checked"' : '' ?> />
  <?php _e('Allow new transcripts to be submitted for this post', 'what-did-they-say') ?>
</label>

<?php $approved_editor_id = md5(rand()) ?>

<?php if (!is_admin()) { ?>
  <?php include(dirname(__FILE__) . '/_manage-queued-transcripts.inc') ?>
<?php } ?>

<?php if (is_array($queued_transcripts) && !empty($queued_transcripts)) { ?>
  <p><strong><?php _e('Manage queued transcripts:', 'what-did-they-say') ?></strong></p>
  <?php
    foreach ($queued_transcripts as $transcript) {
      $user = get_userdata($transcript['user_id']);
      if (!empty($user)) { include('_display-queued-transcript.inc'); }
    }
  ?>
<?php } ?>

<div id="wdts-<?php echo $approved_editor_id ?>">
  <?php if (!defined('WP_ADMIN')) { ?>
    <h3 class="wdts"><?php _e('Edit Approved Transcripts:', 'what-did-they-say') ?></h3>
  <?php } ?>
  <div class="wdts-transcript-editor">
    <label>
      <?php _e('Language:', 'what-did-they-say') ?>
      <select name="wdts[language]" class="wdts-transcript-selector">
        <?php foreach ($options['languages'] as $code => $info) { ?>
          <option value="<?php echo $code ?>" <?php echo $info['default'] ? 'selected="selected"' : '' ?>><?php echo $info['name'] ?></option>
        <?php } ?>
      </select>
    </label>

    <div class="wdts-button-holder"></div>
    <?php foreach (array_keys($options['languages']) as $code) {
      $approved_transcript_text = '';
      foreach ($approved_transcripts as $transcript) {
        if ($transcript['language'] == $code) { $approved_transcript_text = $transcript['transcript']; break; }
      } ?>
      <textarea class="edit-transcript" name="wdts[transcripts][<?php echo $code ?>]" style="display: none; width: 99%; height: 200px"><?php echo $approved_transcript_text ?></textarea>
    <?php } ?>
    <input type="button" class="wdts-modify-transcript" value="<?php _e('Update All Transcripts', 'what-did-they-say') ?>" />
    <span class="wdts-update-message"></span>
  </div>
</div>

<script type="text/javascript">
  WhatDidTheySay.setup_transcript_editor('wdts-<?php echo $approved_editor_id ?>');
  <?php
    $allow_new_transcripts_parameters = array("'wdts-${checkbox_id}'");
    if (isset($new_transcript_id)) {
      $allow_new_transcripts_parameters[] = "'wdts-${new_transcript_id}'";
    }
  ?>
  WhatDidTheySay.setup_allow_new_transcripts(<?php echo implode(",", $allow_new_transcripts_parameters) ?>);
</script>
