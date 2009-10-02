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
<div class="queued-transcript wdts-transcript"><?php echo do_shortcode($transcript['transcript']) ?></div>
<pre class="queued-transcript-raw" style="display:none"><?php echo $transcript['transcript'] ?></pre>
<div class="queued-transcript-actions"></div>
