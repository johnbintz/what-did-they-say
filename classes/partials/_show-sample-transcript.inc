<html>
  <head>
    <?php foreach ($styles_to_include as $style) { ?>
      <link rel="stylesheet" href="<?php echo $style ?>" type="text/css" />
    <?php } ?>
  </head>
  <body>
    <div class="wdts-transcript">
      <?php echo do_shortcode(implode("\n", $sample_transcript)) ?>
    </div>
  </body>
</html>