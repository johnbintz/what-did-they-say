<?php

class WDTSLanguageOptions {
  var $key = "what-did-they-say-options";
  
  /**
   * Get the default transcript language for this blog.
   * @return string The language code representing the default language.
   */
  function get_default_language() {
    $language = false;
    $options = get_option($this->key);
    foreach ($options['languages'] as $code => $info) {
      if (is_null($language)) { $language = $code; }
      if (isset($info['default'])) { 
        if ($info['default']) { $language = $code; break; }
      }
    }
    return $language;
  }

  function set_default_language($language) {
    $options = get_option($this->key);

    if (isset($options['languages'][$language])) {
      $ok = true;
      if (isset($options['languages'][$language]['default'])) {
        $ok = !$options['languages'][$language]['default'];
      }
      if ($ok) {
        $updated_languages = array();
        foreach ($options['languages'] as $code => $info) {
          unset($info['default']);
          if ($code == $language) { $info['default'] = true; }
          $updated_languages[$code] = $info;
        }
        $options['languages'] = $updated_languages;
        update_option($this->key, $options);

        return true;
      }
    }
    return false;
  }

  /**
   * Get the name of a language from the language code.
   * @param string $language The language code to search for.
   * @return string|false The name of the language as defined in the options, or false if the language was not found.
   */
  function get_language_name($language) {
    $options = get_option($this->key);

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
    $options = get_option($this->key);

    return $options['languages'];
  }
  
  function delete_language($code_to_delete) {
    $options = get_option($this->key);

    $did_delete = false;
    if (isset($options['languages'][$code_to_delete])) {
      $did_delete = $options['languages'][$code_to_delete];
      unset($options['languages'][$code_to_delete]);
      ksort($options['languages']);
    }
    
    update_option($this->key, $options);
    return $did_delete;
  }
  
  function add_language($code, $info) {
    $options = get_option($this->key);   
    
    $result = false;
    if (!empty($code) && is_array($info)) {
      if (!isset($options['languages'][$code])) {
        if (!empty($info['name'])) {
          $options['languages'][$code] = $info;
          ksort($options['languages']);
          $result = true;
        }        
      }
    }

    update_option($this->key, $options);
    return $result;
  }
  
  function rename_language($code_to_rename, $new_name) {
    $options = get_option($this->key);

    $found = false;
    if (!empty($code_to_rename) && !empty($new_name)) {
      $new_languages = array();
      foreach ($options['languages'] as $code => $info) {
        if ($code == $code_to_rename) {
          $info['name'] = $new_name;
          $found = true;
        }
        $new_languages[$code] = $info;
      }
      $options['languages'] = $new_languages;
      ksort($options['languages']);
      
      update_option($this->key, $options);
    }
    
    return $found;
  }
}

?>
