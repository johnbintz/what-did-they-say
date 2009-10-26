<html>
  <head>
    <?php foreach ($styles_to_include as $style) { ?>
      <link rel="stylesheet" href="<?php echo $style ?>" type="text/css" />
    <?php } ?>
  </head>
  <body>
    <?php echo end(apply_filters('the_media_transcript', array('transcript' => implode("\n", $sample_transcript), 'user_id' => 1), '')) ?>
  </body>
</html>