<?php

class WDTSDisplayFilters {
  /**
   * Build the opener/closer for transcripts.
   */
  function the_transcript_opener($content = '') {
    ob_start(); ?>
    <div class="wdts-transcript-opener" style="display:none"> [
      <a href="#" class="wdts-opener"><?php _e('Show transcripts') ?></a>
      <a href="#" class="wdts-closer"><?php _e('Hide transcripts') ?></a>
    ] </div>
    <?php return ob_get_clean();
  }

  /**
   * Handle the_media_transcript filter.
   * @param string $transcript The transcription text.
   * @return string The processed transcription text.
   */
  function the_media_transcript($transcript, $content = "") {
    $content = $this->_process_transcript_content($transcript);
    return array($transcript, '<div class="wdts-transcript">' . $content . '</div>');
  }

  function the_approved_credit($user, $content = '') {
    return '<div class="wdts-approved-credit">' . sprintf(__('Written by %1$s <strong>%2$s</strong>', 'what-did-they-say'), get_avatar($user->ID, 20), $user->display_name) . '</div>';
  }

  function _process_transcript_content($transcript) {
    $content = do_shortcode($transcript['transcript']);
    $options = get_option('what-did-they-say-options');
    if ($options['use_nl2br']) { $content = nl2br($content); }

    if ($options['show_approved_credit']) {
      $user = get_userdata($transcript['user_id']);
      if (!empty($user)) {
        $content .= apply_filters('the_approved_credit', $user, '');
      }
    }

    return $content;
  }

  /**
   * Handle the_language_name filter.
   * @param string $language The name of the language.
   * @return string The processed language name.
   */
  function the_language_name($language, $content = "") {
    return array($language, '<h3 class="wdts-transcript-language">' . sprintf(apply_filters('the_transcript_format_string'), $language) . '</h3>');
  }

  /**
   * Handle showing the header above a bundle of live transcripts.
   */
  function the_transcript_language_name($language_format, $code, $content) {
    if (is_null($language_format)) { $language_format = apply_filters('the_transcript_format_string', ''); }

    $language_options = new WDTSLanguageOptions();

    return array($language_format, $code, '<h3 class="wdts-transcript-language">' . sprintf($language_format, $language_options->get_language_name($code)) . '</h3>');
  }

  /**
   * The format string used to display ither a single or multiple language transcript header.
   */
  function the_transcript_format_string($content) {
    return __('Transcript: %s', 'what-did-they-say');
  }

  /**
   * The script.aculo.us effects to use when fancy effects are enabled.
   */
  function the_transcript_transition_effect($is_opening = true, $content) {
    if ($is_opening) {
      return array(true, "function(t) { new Effect.BlindDown(t, { duration: 0.25 }); }");
    } else {
      return array(false, "function(t) { new Effect.BlindUp(t, { duration: 0.25 }); }");
    }
  }

  /**
   * Handle the_matching_transcript_excerpts.
   */
  function the_matching_transcript_excerpts($transcripts, $search_string = '', $content = "") {
    $options = get_option('what-did-they-say-options');
    ob_start();
    if ($options['search_integration']) {
      if (!empty($search_string)) {
        $language_options = new WDTSLanguageOptions();
        $options = get_option('what-did-they-say-options');

        foreach ($transcripts as $transcript) {
          if (($pos = strpos($transcript['transcript'], $search_string)) !== false) {
            $l = strlen($transcript['transcript']) - 1;
            echo '<div class="transcript-match">';
              echo '<h4>' . sprintf(__("%s transcript excerpt:", 'what-did-they-say'), $language_options->get_language_name($transcript['language'])) . '</h4>';
              echo '<p>';
                $start_ellipsis = $end_ellipsis = true;
                foreach (array(
                  'start' => -1,
                  'end'   => 1
                ) as $variable => $direction) {
                  ${$variable} = $pos + ($options['excerpt_distance'] * $direction);

                  if ($variable == "end") { ${$variable} += strlen($search_string); }

                  if (${$variable} < 0) { ${$variable} = 0; $start_ellipsis = false; }
                  if (${$variable} > $l) { ${$variable} = $l; $end_ellipsis = false; }
                }

                $output = "";
                if ($start_ellipsis) { $output .= "..."; }
                $output .= str_replace($search_string, "<strong>" . $search_string . "</strong>", trim(substr($transcript['transcript'], $start, $end - $start)));
                if ($end_ellipsis) { $output .= "..."; }

                echo $output;
              echo '</p>';
            echo '</div>';
          }
        }
      }
    }
    return array($transcripts, $search_string, ob_get_clean());
  }

  /**
   * Filter for dialog short code.
   */
  function filter_shortcode_dialog($name, $direction, $speech, $content) {
    $content  = '<div class="wdts-dialog"><span class="wdts-name">' . $name . '</span>';
    if (!empty($direction)) {
      $content .= ' <span class="wdts-direction">' . $direction . '</span>';
    }
    $content .= ' <span class="wdts-speech">' . $speech . '</span></div>';

    return array($name, $direction, $speech, $content);
  }

  /**
   * Filter for scene heading short code.
   */
  function filter_shortcode_scene_heading($description, $content) {
    return array($description, '<div class="wdts-scene-heading">' . $description . '</div>');
  }

  /**
   * Filter for scene action short code.
   */
  function filter_shortcode_scene_action($description, $content) {
    return array($description, '<div class="wdts-scene-action">' . $description . '</div>', );
  }
}

?>