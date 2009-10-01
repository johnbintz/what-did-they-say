<?php if ((count($queued_transcripts_for_user) > 0) && !current_user_can('approve_transcriptions')) { ?>
  <h3 style="margin-top: 0.5em"><?php _e('Your Submitted Transcripts:', 'what-did-they-say') ?></h3>
  <?php
    foreach ($queued_transcripts_for_user as $transcript) {
      include(dirname(__FILE__) . '/classes/partials/_display-queued-transcript.inc');
    }
  ?>
<?php } ?>
<?php if (current_user_can('submit_transcriptions')) { ?>
  <div id="wdts-<?php echo $new_transcript_id ?>" <?php echo (!$transcript_options->are_new_transcripts_allowed() ? 'style="display:none"' : '') ?>>
    <h3 class="wdts"><?php _e('Submit a new transcript:', 'what-did-they-say') ?></h3>
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
        <input type="submit" value="<?php _e('Submit For Approval', 'what-did-they-say') ?>" />
      </div>
    </form>
    <script type="text/javascript">WhatDidTheySay.setup_transcript_editor('wdts-<?php echo $new_transcript_id ?>')</script>
  </div>
<?php } ?>