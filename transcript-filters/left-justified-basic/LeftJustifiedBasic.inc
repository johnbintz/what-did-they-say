<?php

class LeftJustifiedBasic extends WDTSDisplayFilters {
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
}

?>