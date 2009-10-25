<?php

?><div class="wrap">
  <h2><?php _e('Import Transcripts', 'what-did-they-say') ?></h2>
  <p>
    <?php _e('Import transcripts from your current setup into <strong>What Did They Say?!?</strong>', 'what-did-they-say') ?>
    <?php _e('Currently, the import supports pulling transcripts from the &quot;transcript&quot; metadata field.', 'what-did-they-say') ?>
    <?php _e('This field is used by some webcomic themes to store and display transcripts.', 'what-did-they-say') ?>
  </p>
  <?php if ($legacy_transcript_count == 0) { ?>
    <p>
      <?php _e('<strong>You have no legacy transcripts!</strong>', 'what-did-they-say') ?>
    </p>
  <?php } else { ?>
    <p>
      <?php printf(__('<strong>You have %d legacy transcripts.</strong< Click the button below to import them into <strong>What Did They Say?!?</strong>', 'what-did-they-say'), $legacy_transcript_count) ?>
      <?php _e('<strong>Make sure you\'ve backed up your database first!</strong>', 'what-did-they-say') ?>
      <?php _e('<strong>This will also overwrite any WDTS? transcripts that are already in the chosen language for each modified post!</strong>', 'what-did-they-say') ?>
    </p>
    
    <form>
      <label>
        <?php _e('Language:', 'what-did-they-say') ?>
        <select id="wdts-language" name="wdts[language]" class="wdts-transcript-selector">
          <?php foreach ($options['languages'] as $code => $info) { ?>
            <option value="<?php echo $code ?>" <?php echo $info['default'] ? 'selected="selected"' : '' ?>><?php echo $info['name'] ?></option>
          <?php } ?>
        </select>
      </label>
      
      <button id="wdts-<?php echo $id = md5(rand()) ?>">
        <?php _e('Import transcripts', 'what-did-they-say') ?>
      </button>
      <span class="wdts-updater" style="display:none" id="wdts-updater-<?php echo $id ?>"></span>
    </form>
    <style type="text/css">
      .wdts-updater {
        border: solid #500 1px;
        padding: 2px;
        background-color: #eab
      }
    </style>
    <script type="text/javascript">
      var total_transcripts = <?php echo $legacy_transcript_count ?>;
      var total_imported    = 0;
      var use_language;
      
      var do_transcript_import;
      do_transcript_import = function() {
        var updater = $('wdts-updater-<?php echo $id ?>');
        updater.update(total_imported + '/' + total_transcripts + ' <?php _e('imported...', 'what-did-they-say') ?>');
        if (!updater.visible()) {
          new Effect.BlindDown(updater, { duration: 0.5 });
        }
        
        new Ajax.Request('<?php echo addslashes($_SERVER['REQUEST_URI']) ?>', {
          method: 'post',
          parameters: {
            'wdts[_nonce]': '<?php echo $nonce ?>',
            'wdts[module]': 'import-legacy',
            'wdts[language]': use_language
          },
          onSuccess: function(r) {
            var count = r.headerJSON;
            if (count == 'false') {
              updater.update('<?php _e('There was a problem during import!', 'what-did-they-say') ?>');
            } else {
              count = Number(count);
              if (count > 0) {
                total_imported += count;
                do_transcript_import();
              } else {
                updater.update('<?php _e('Import complete! Total number imported:', 'what-did-they-say') ?> ' + total_imported);
              }
            }
          }
        });
      };
      
      $('wdts-<?php echo $id ?>').observe('click', function(e) {
        Event.stop(e);
        use_language = $F('wdts-language')
        do_transcript_import();
        e.target.disabled = true;
      });
    </script>
  <?php } ?>
</div>