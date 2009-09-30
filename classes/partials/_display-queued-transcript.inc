<div class="queued-transcript-holder" id="wdts-<?php echo $id = md5(rand()) ?>">
  <?php include(dirname(__FILE__) . '/_queued-transcript-contents.inc') ?>
</div>
<script type="text/javascript">WhatDidTheySay.setup_transcript_action_buttons('wdts-<?php echo $id ?>', 'wdts-<?php echo $approved_editor_id ?>')</script>