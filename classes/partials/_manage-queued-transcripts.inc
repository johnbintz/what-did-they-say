<?php
	$transcripts_to_show = array();

	if (current_user_can('approve_transcriptions')) {
		$transcripts_to_show = $queued_transcript_object->get_transcripts();
		$title = __('All Submitted Transcripts', 'what-did-they-say');
	} else {
		if (!is_admin()) {
	    $user = wp_get_current_user();
	    if (!empty($user)) {
	      $transcripts_to_show = $queued_transcript_object->get_transcripts_for_user($user->ID);
				$title = __('Your Submitted Transcripts', 'what-did-they-say');
	    }
		}
	}
?>

<?php if (current_user_can('submit_transcriptions')) { ?>
  <?php if (!empty($transcripts_to_show)) { ?>
    <h3 style="margin-top: 0.5em"><?php echo $title ?></h3>
    <?php
      foreach ($transcripts_to_show as $transcript) {
        include('_display-queued-transcript.inc');
      }
    ?>
  <?php } ?>

  <div id="wdts-<?php echo $new_transcript_id ?>" style="zoom:1<?php echo (!$transcript_options->are_new_transcripts_allowed() ? ';display:none' : '') ?>">
    <?php wdts_header_wrapper(__('Submit a New Transcript:', 'what-did-they-say')) ?>
    <form method="post">
      <input type="hidden" name="wdts[_nonce]" value="<?php echo wp_create_nonce('what-did-they-say') ?>" />
      <input type="hidden" name="wdts[module]" value="queue-transcript" />
      <input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />

      <div class="wdts-transcript-editor">
        <label>
          <?php _e('Language:', 'what-did-they-say') ?>
          <select name="wdts[language]">
            <?php foreach ($language_options->get_languages() as $code => $info) { ?>
              <option value="<?php echo $code ?>"><?php echo $info['name'] ?></option>
            <?php } ?>
          </select>
        </label>

        <div class="wdts-button-holder"></div>

        <textarea style="height: 200px; width: 99%" name="wdts[transcript]"></textarea>
        <input type="submit" class="button" value="<?php _e('Submit For Approval', 'what-did-they-say') ?>" />
      </div>
    </form>
  </div>
  <script type="text/javascript">WhatDidTheySay.setup_transcript_editor('wdts-<?php echo $new_transcript_id ?>')</script>
<?php } ?>