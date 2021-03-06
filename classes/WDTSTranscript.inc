<?php

class WDTSTranscriptManager {
  var $key = null;
  var $search_key = null;
  var $post_id = null;
  var $allow_multiple = false;

  function __construct($post_id = null) {
    if (is_numeric($post_id)) { $this->post_id = $post_id; }
  }

  function WDTSTranscriptManager($post_id = null) {
    $this->__construct($post_id);
  }

  function _get_transcripts_metadata() {
    $transcripts = false;
    if (!is_null($this->key)) {
      $post = get_post($this->post_id);
      if (!empty($post)) {
        $transcripts = get_post_meta($this->post_id, $this->key, true);
        if (!is_array($transcripts)) { $transcripts = array(); }
      }
    }
    return $transcripts;
  }

  /**
   * Save a transcript to a post.
   * @param int $post_id The post to attach the transcript to.
   * @param string $language The language of the transcript.
   * @param string $transcript The transcript content.
   * @return bool True if the transcript was saved, false otherwise.
   */
  function save_transcript($transcript_info) {
    $transcript_info = (array)$transcript_info;
    if (!isset($transcript_info['user_id'])) {
      $user = wp_get_current_user();
      if (!empty($user)) {
        $transcript_info['user_id'] = $user->ID;
      }
    }
    if (isset($transcript_info['user_id'])) {
      unset($transcript_info['key']);

      foreach (array_keys($transcript_info) as $key) {
        if (strpos($key, "_") === 0) { unset($transcript_info[$key]); }
      }

      if (($transcripts = $this->_get_transcripts_metadata()) !== false) {
        $max_key = 0;
        foreach ($transcripts as $transcript) {
          $max_key = max($max_key, $transcript['key']) + 1;
        }
        $transcript_info['key'] = $max_key;

        if ($this->allow_multiple) {
          $transcripts[] = $transcript_info;
        } else {
          $new_transcripts = array();
          $was_added = false;
          foreach ($transcripts as $transcript) {
            if ($transcript['language'] == $transcript_info['language']) {
              $was_added = true;
              $transcript_info['key']--;
              $new_transcripts[] = $transcript_info;
            } else {
              $new_transcripts[] = $transcript;
            }
          }
          if (!$was_added) { $new_transcripts[] = $transcript_info; }
          $transcripts = $new_transcripts;
        }

        $this->_update_search_field($transcripts);

        return update_post_meta($this->post_id, $this->key, $transcripts);
      }
    }
    return false;
  }

  function update_transcript($info) {
    if (isset($info['key'])) {
      if (($transcripts = $this->_get_transcripts_metadata()) !== false) {
        $new_transcripts = array();
        foreach ($transcripts as $transcript) {
          if ($transcript['key'] == $info['key']) {
            foreach (array('transcript') as $field) {
              if (isset($info[$field])) {
                $transcript[$field] = $info[$field];
              }
            }
          }
          $new_transcripts[] = $transcript;
        }

        return update_post_meta($this->post_id, $this->key, $new_transcripts);
      }
    }
  }

  function _update_search_field($transcripts) {
    if (!empty($this->search_key)) {
      $search_lines = array();
      foreach ($transcripts as $transcript) {
        $search_lines[] = preg_replace_callback('#\[[^\]]+\]#', array(&$this, '_update_search_field_callback'), $transcript['transcript']);
      }
      update_post_meta($this->post_id, $this->search_key, implode(" ", $search_lines));
    }
  }

  function _update_search_field_callback($result) {
    $properties = preg_match_all('#[a-z]+="([^\"]+)"#', reset($result), $matches);
    $return = "";
    foreach ($matches[1] as $match) { $return .= $match . " "; }
    return $return;
  }

  function delete_transcript($language = null) {
    return $this->_delete_transcript_by_field('language', $language);
  }

  function delete_transcript_by_key($key = null) {
    return $this->_delete_transcript_by_field('key', $key);
  }

  function _delete_transcript_by_field($field, $value) {
    if (($transcripts = $this->_get_transcripts_metadata()) !== false) {
      $new_transcripts = array();
      $deleted_transcript = false;
      foreach ($transcripts as $transcript) {
        if ($transcript[$field] != $value) {
          $new_transcripts[] = $transcript;
        } else {
          $deleted_transcript = $transcript;
        }
      }

      $this->_update_search_field($new_transcripts);
      
      update_post_meta($this->post_id, $this->key, $new_transcripts);
      return $deleted_transcript;
    }
    return false;
  }

  function get_transcripts() {
    return $this->_get_transcripts_metadata();
  }

  function get_transcripts_for_user($user_id) {
    $user_transcripts = array();
    if (($transcripts = $this->_get_transcripts_metadata()) !== false) {
      foreach ($transcripts as $transcript) {
        if ($transcript['user_id'] == $user_id) { $user_transcripts[] = $transcript; }
      }
    }
    return $user_transcripts;
  }

  function get_transcript_by_key($key) {
    foreach ($this->get_transcripts() as $transcript) {
      if ($transcript['key'] == $key) { return $transcript; }
    }
    return false;
  }

  function get_languages() {
    $languages = array();
    if (($transcripts = $this->_get_transcripts_metadata()) !== false) {
      foreach ($transcripts as $transcript) {
        $languages[$transcript['language']] = true;
      }
    }
    return array_keys($languages);
  }

  function count() {
    return count($this->get_transcripts());
  }
}

?>