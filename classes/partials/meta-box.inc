<input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />
<?php if (is_admin()) { ?>
  <input type="hidden" name="wdts[module]" value="manage-post-transcripts" />
  <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
<?php } ?>

<input id="wdts-<?php echo $checkbox_id = md5(rand()) ?>" type="checkbox" name="wdts[allow_on_post]" value="yes" <?php echo ($transcript_options->are_new_transcripts_allowed()) ? 'checked="checked"' : '' ?> />
<?php _e('Allow new transcripts to be submitted for this post', 'what-did-they-say') ?>

<?php $approved_editor_id = md5(rand()) ?>

<script type="text/javascript">
  var queued_editors_to_attach = [];
</script>

<?php if (!is_admin()) { ?>
  <?php include(dirname(__FILE__) . '/_manage-queued-transcripts.inc') ?>
<?php } ?>

<?php if (is_array($queued_transcripts) && !empty($queued_transcripts)) { ?>
  <?php wdts_header_wrapper(__('Manage Queued Transcripts:', 'what-did-they-say')) ?>
  <?php
    foreach ($queued_transcripts as $transcript) {
      $user = get_userdata($transcript['user_id']);
      if (!empty($user)) { include('_display-queued-transcript.inc'); }
    }
  ?>
<?php } ?>

<div id="wdts-<?php echo $approved_editor_id ?>">
  <?php wdts_header_wrapper(__('Edit Approved Transcripts:', 'what-did-they-say')) ?>
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
    <?php if (!is_admin()) { ?>
      <input type="button" class="button" class="wdts-modify-transcript" value="<?php _e('Update All Transcripts', 'what-did-they-say') ?>" />
      <span class="wdts-update-message"></span>
    <?php } ?>
  </div>
  <div class="wdts-embed-warning"><?php _e('Uh oh...there\'s a problem with automatic embedding. Enable &quot;Inject transcripts at level 100 instead of level 15&quot; on the What Did They Say?!? Misc. Options page.', 'what-did-they-say') ?></div>
</div>

<script type="text/javascript">
  queued_editors_to_attach.each(function(q) {
    WhatDidTheySay.setup_transcript_action_buttons(q, 'wdts-<?php echo $approved_editor_id ?>')
  });

  $$('.wdts-embed-warning').invoke('hide');

  WhatDidTheySay.setup_transcript_editor('wdts-<?php echo $approved_editor_id ?>');
  <?php if (!is_admin()) { ?>
    <?php
      $allow_new_transcripts_parameters = array("'wdts-${checkbox_id}'");
      if (isset($new_transcript_id)) {
        $allow_new_transcripts_parameters[] = "'wdts-${new_transcript_id}'";
      }
    ?>
    WhatDidTheySay.setup_allow_new_transcripts(<?php echo implode(",", $allow_new_transcripts_parameters) ?>);
  <?php } ?>
</script>
