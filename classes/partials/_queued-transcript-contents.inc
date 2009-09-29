<input type="hidden" name="wdts[language]" value="<?php echo $transcript['language'] ?>" />
<input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />
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
<div class="transcript-action-links">
  <?php if (current_user_can('approve_transcriptions')) { ?>
    <a href="#" class="approve-transcript button">Approve</a> |
  <?php } ?>
  <a href="#" class="delete-transcript button">Delete</a> |
  <a href="#" class="edit-transcript button">Edit</a>
</div>
