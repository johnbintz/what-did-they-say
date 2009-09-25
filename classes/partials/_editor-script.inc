<script type="text/javascript">
  var ajax_url = '<?php echo $_SERVER['REQUEST_URI'] ?>';
  var nonce = '<?php echo $nonce ?>';

  var messages = {
    'overwrite': '<?php _e('This will overwrite the current transcript. Are you sure?', 'what-did-they-say') ?>',
    'delete': '<?php _e('This will delete the queued transcript. Are you sure?', 'what-did-they-say') ?>',
    'approved': '<?php _e('Transcript approved and posted. You can further edit it below.', 'what-did-they-say') ?>',
    'deleted': '<?php _e('Transcript deleted.', 'what-did-they-say') ?>',
    'scene_heading': '<?php _e('Enter the scene heading:', 'what-did-they-say') ?>',
    'scene_action': '<?php _e('Enter the scene action:', 'what-did-they-say') ?>',
    'dialog_name': '<?php _e('Enter the character name:', 'what-did-they-say') ?>',
    'dialog_direction': '<?php _e('Enter the direction in which the character is speaking:', 'what-did-they-say') ?>',
    'dialog_speech': '<?php _e('Enter what the character is speaking:', 'what-did-they-say') ?>',
  };
</script>