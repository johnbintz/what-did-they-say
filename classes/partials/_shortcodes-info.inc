<?php

$sample_transcript   = array();
$sample_transcript[] = __('[scene-heading]Ext. The Old Man\'s House[/scene-heading]', 'what-did-they-say');
$sample_transcript[] = __('[scene-action]John is walking down to the car parked in the driveway.[/scene-action]', 'what-did-they-say');
$sample_transcript[] = __('[dialog name="John" direction="(towards the house)"]Hey, where are the keys?[/dialog]', 'what-did-they-say');

?>
<?php if (current_user_can('submit_transcriptions')) { ?>
  <h3><?php _e('Shortcodes Info', 'what-did-they-say') ?></h3>

  <p>
    <em>(<?php _e('you can easily use these shortcodes with the appropriate buttons above all transcript editors', 'what-did-they-say') ?>)</em>
  </p>
  
  <h4><?php _e('Sample Transcript', 'what-did-they-say') ?></h4>
  <pre><?php echo implode("\n", $sample_transcript) ?></pre>

  <div class="wdts-transcript">
    <?php echo do_shortcode(implode("\n", $sample_transcript)) ?>
  </div>
  
  <h4><?php _e('Shortcode Details', 'what-did-they-say') ?></h4>
  <p>
    <?php _e('Transcripts can be entered either using straight HTML or by using shortcodes built for <strong>What Did They Say?!?</strong>', 'what-did-they-say') ?>
    <?php _e('Using shortcodes will make your transcripts easier to style.', 'what-did-they-say') ?>
    <?php _e('The default styles that come with <strong>What Did They Say?!?</strong> make your transcripts look like a screenplay.', 'what-did-they-say') ?>
  </p>
  <h4><?php _e('Available Shortcodes', 'what-did-they-say') ?></h4>

  <h5><?php _e('[scene-heading]', 'what-did-they-say') ?></h5>
    <p><?php _e('The text that indicates a new scene:', 'what-did-they-say') ?></p>
    <p>
      <?php _e('<code>[scene-heading]Ext. The Old Man\'s House[/scene-heading]</code> becomes:', 'what-did-they-say') ?>
      <span style="text-transform: uppercase; font-weight: bold; font-family: 'Courier New'"><?php _e('Ext. The Old Man\'s House', 'what-did-they-say') ?></span>
    </p>

  <h5><?php _e('[scene-action]', 'what-did-they-say') ?></h5>
    <p><?php _e('The text that indicates action taking place in a scene. One usually occurs underneath of a scene heading:', 'what-did-they-say') ?></p>
    <p>
      <?php _e('<code>[scene-action]John is walking down to the car parked in the driveway.[/scene-action]</code> becomes:', 'what-did-they-say') ?>
      <span style="font-family: 'Courier New'"><?php _e('John is walking down to the car parked in the driveway.', 'what-did-they-say') ?></span>
    </p>

  <h5><?php _e('[dialog]', 'what-did-they-say') ?></h5>
    <p><?php _e('The text for when a character is speaking in a scene:', 'what-did-they-say') ?></p>
    <p>
      <?php _e('<code>[dialog name="John" direction="(towards the house)"]Hey, where are the keys?[/dialog]</code> becomes:', 'what-did-they-say') ?>
    </p>
    <div style="width: 300px; border: solid #333 1px; margin: 10px; text-align: center">
      <div style="font-family: 'Courier New'; text-transform: uppercase"><?php _e('John', 'what-did-they-say') ?></div>
      <div style="font-family: 'Courier New'; margin-bottom: 10px"><?php _e('(towards the house)', 'what-did-they-say') ?></div>
      <div style="font-family: 'Courier New'"><?php _e('Hey, where are the keys?', 'what-did-they-say') ?></div>
    </div>
<?php } ?>
