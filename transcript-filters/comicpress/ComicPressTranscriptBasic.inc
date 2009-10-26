<?php

class ComicPressTranscriptBasic extends WDTSDisplayFilters {
  /**
   * Filter for dialog short code.
   */
  function filter_shortcode_dialog($name, $direction, $speech, $content) {
    $content  = '<div class="wdts-dialog"><span class="wdts-name">' . $name . '</span>';
    if (!empty($direction)) {
      $content .= ' <span class="wdts-direction">' . $direction . '</span>';
    }
    $content .= ': <span class="wdts-speech">' . $speech . '</span></div>';

    return array($name, $direction, $speech, $content);
  }

  /**
   * Handle the_media_transcript filter.
   * @param string $transcript The transcription text.
   * @return string The processed transcription text.
   */
  function the_media_transcript($transcript, $content = '') {
    $content = $this->_process_transcript_content($transcript);
    return array($transcript, '<div class="wdts-transcript"><div class="wdts-transcript-inner">' . $content . '</div></div>');
  }

  /**
   * Build the opener/closer for transcripts.
   */
  function the_transcript_opener($content = '') {
    ob_start(); ?>
    <div class="wdts-transcript-opener" style="display:none">
      <a href="#" class="wdts-opener">&darr; <?php _e('Transcript') ?></a>
      <a href="#" class="wdts-closer">&uarr; <?php _e('Transcript') ?></a>
    </div>
    <?php return ob_get_clean();
  }
}

?>