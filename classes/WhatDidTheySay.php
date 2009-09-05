<?php

/**
 * WhatDidTheySay manages transcriptions that are attached as metadata
 * individual posts. Transcriptions can be updated, deleted, and moved to
 * other posts as necessary.
 */
class WhatDidTheySay {
  /**
   * Constructor.
   */
  function WhatDidTheySay() {}

  /**
   * Get the default transcript language for this blog.
   * @return string The language code representing the default language.
   */
  function get_default_language() {
    $language = false;
    $options = get_option('what-did-they-say-options');
    foreach ($options['languages'] as $code => $info) {
      if (is_null($language)) { $language = $code; }
      if ($info['default']) { $language = $code; break; }
    }
    return $language;
  }

  /**
   * Get the name of a language from the language code.
   * @param string $language The language code to search for.
   * @return string|false The name of the language as defined in the options, or false if the language was not found.
   */
  function get_language_name($language) {
    $options = get_option('what-did-they-say-options');

    if (isset($options['languages'][$language])) {
      return $options['languages'][$language]['name'];
    } else {
      return false; 
    }
  }

  /**
   * Get all available languages.
   * @return array An array of languages.
   */
  function get_languages() {
    $options = get_option('what-did-they-say-options');

    return $options['languages'];
  }

  /**
   * Set whether or not the indicated post can accept new queued transcriptions.
   * @param int $post_id The post ID to affect.
   * @param boolean $allow True if the post can accept new queued transcriptions.
   */
  function set_allow_transcripts_for_post($post_id, $allow = true) {
    $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
    $current_transcripts['_allow'] = $allow;
    update_post_meta($post_id, "provided_transcripts", $current_transcripts);
  }

  /**
   * See if the indicated post is accepting new transcripts.
   * @return boolean True if the post is acceptin new transcripts.
   */
  function get_allow_transcripts_for_post($post_id) {
    $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
    return $current_transcripts['_allow'];
  }
}

?>
