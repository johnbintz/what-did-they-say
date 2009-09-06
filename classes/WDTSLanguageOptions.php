<?php

class WDTSLanguageOptions {
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
}

?>
