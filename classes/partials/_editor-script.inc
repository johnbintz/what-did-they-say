var WhatDidTheySay = Class.create({});

WhatDidTheySay.messages = {
  'overwrite': '<?php _e('This will overwrite the current transcript. Are you sure?', 'what-did-they-say') ?>',
  'delete_message': '<?php _e('This will delete the queued transcript. Are you sure?', 'what-did-they-say') ?>',
  'approved': '<?php _e('Transcript approved and posted. You can further edit it below.', 'what-did-they-say') ?>',
  'deleted': '<?php _e('Transcript deleted.', 'what-did-they-say') ?>',
  'scene_heading': '<?php _e('Enter the scene heading:', 'what-did-they-say') ?>',
  'scene_action': '<?php _e('Enter the scene action:', 'what-did-they-say') ?>',
  'dialog_name': '<?php _e('Enter the character name:', 'what-did-they-say') ?>',
  'dialog_direction': '<?php _e('Enter the direction in which the character is speaking:', 'what-did-they-say') ?>',
  'dialog_speech': '<?php _e('Enter what the character is speaking:', 'what-did-they-say') ?>',
  'transcripts_updated': '<?php _e('Transcripts updated. Reload to see them on page.', 'what-did-they-say') ?>',
  'transcripts_failure': '<?php _e('Transcript update failure!', 'what-did-they-say') ?>',
  'bundle_header': '<?php echo apply_filters('the_transcript_format_string', '') ?>',
  'show_transcripts': '<?php _e('Show transcripts', 'what-did-they-say') ?>',
  'hide_transcripts': '<?php _e('Hide transcripts', 'what-did-they-say') ?>'
};

WhatDidTheySay.button_labels = {
  'scene-heading': '<?php _e('Scene Heading') ?>',
  'scene-action': '<?php _e('Scene Action') ?>',
  'dialog': '<?php _e('Dialog') ?>',
  'approve': '<?php _e('Approve') ?>',
  'delete': '<?php _e('Delete') ?>',
  'edit': '<?php _e('Edit') ?>'
};

WhatDidTheySay.can_approve = <?php echo current_user_can('approve_transcriptions') ? "true" : "false" ?>;

<?php
  $language_output = array();
  foreach ($language_options->get_languages() as $code => $info) {
    $language_output[] = "${code}: '" . addslashes($info['name']) . "'";
  }
?>
WhatDidTheySay.languages = { <?php echo implode(",", $language_output) ?> };

WhatDidTheySay.default_language = '<?php echo $language_options->get_default_language() ?>';
