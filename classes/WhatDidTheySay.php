<?php

/**
 * WhatDidTheySay manages transcriptions that are attached as metadata
 * individual posts. Transcriptions can be updated, deleted, and moved to
 * other posts as necessary.
 */
class WhatDidTheySay {
  var $version = "0.1";

  /**
   * Constructor.
   */
  function WhatDidTheySay() {
    global $wpdb;
    
    $this->table =  $wpdb->prefix . "provided_transcripts";
  }

  /**
   * Handle plugin activation.
   */
  function install() {
    if (get_option('what-did-they-say-version') !== $this->version) {
      $sql = "CREATE TABLE %s (
              id int NOT NULL AUTO_INCREMENT,
              post_id int NOT NULL,
              user_id int NOT NULL,
              language char(10) NOT NULL,
              transcript mediumtext,
              UNIQUE KEY id (id)
             );";
             
      require_once(ABSPATH . 'wp-admin/includes/upgrade.php');
      dbDelta(sprintf($sql, $this->table));
      
      update_option('what-did-they-say-version', $this->version);
    }
  }
  
  /**
   * Save a transcript to a post.
   * @param int $post_id The post to attach the transcript to.
   * @param string $language The language of the transcript.
   * @param string $transcript The transcript content.
   * @return bool True if the transcript was saved, false otherwise.
   */
  function save_transcript($post_id, $language, $transcript) {
    if (current_user_can('submit_transcriptions')) {
      $post = get_post($post_id);
      if (!empty($post)) {
        $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
        if (!is_array($current_transcripts)) { $current_transcripts = array(); }
        $current_transcripts[$language] = $transcript;
        return update_post_meta($post_id, "provided_transcripts", $current_transcripts);
      }
      return false;
    }
  }

  /**
   * Get all transcripts for the indicated post.
   * @param int $post_id The post ID to search.
   * @return array|false The provided transcripts for the post, or false if no transcripts found.
   */
  function get_transcripts($post_id) {
    return get_post_meta($post_id, 'provided_transcripts', true);
  }
  
  /**
   * Get the languages that the approved transcripts for the post are written in.
   * @param int $post_id The post ID to check for transcripts.
   * @return array|false The languages for the transcripts, or false if none found.
   */
  function get_transcript_languages($post_id) {
    $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
    if (is_array($current_transcripts)) {
      return array_keys($current_transcripts); 
    }
    return false;
  }
  
  /**
   * Get the queued transcriptions for the provided post.
   * @param int $post_id The post to search for transcripts.
   * @return array|false The array of transcripts for the post, or false if the post is invalid.
   */
  function get_queued_transcriptions_for_post($post_id) {
    if (current_user_can('submit_transcriptions')) {
      $post = get_post($post_id);
      if (!empty($post)) {
        $results = get_post_meta($post_id, "queued_transcripts", true);
        if (!empty($results)) {
          $valid_results = array();
          foreach ($results as $result) {
            $user = get_userdata($result['user_id']);
            if (!empty($user)) {
              $valid_results[] = $result; 
            }
          }
          return $valid_results;
        }
      }
    }
    return false; 
  }
  
  /**
   * Queue a transcription to a post.
   * @param int $post_id The post to attach the transcription to.
   * @param array $transcript_info The new transcript's info.
   */
  function add_queued_transcription_to_post($post_id, $transcript_info) {
    if (current_user_can('submit_transcriptions')) {
      $post = get_post($post_id);
      if (!empty($post)) {
        $transcript_info = (array)$transcript_info;
        if (!empty($transcript_info)) {
          $ok = true;
          foreach (array('language', 'transcript') as $field) {
            if (empty($transcript_info[$field])) { $ok = false; break; }
          }
          if ($ok) {
            extract($transcript_info);
            $user = wp_get_current_user();
            if (!empty($user)) {
              $current_transcriptions = get_post_meta($post_id, 'queued_transcripts', true);
              $transcript_info['user_id'] = $user->ID;
              $current_transcriptions[] = $transcript_info;
              update_post_meta($post_id, 'queued_transcripts', $current_transcriptions);
              
              return true;
            }
          }
        }
      }
    }
    return false;
  }
  
  /**
   * Update a queued transcript.
   * @param array $update_info The info on the transcript being updated.
   * @return boolean True if the transcript was updated.
   */
  function update_queued_transcription($update_info) {
    global $wpdb;

    if (current_user_can('submit_transcriptions')) {
      $query = $wpdb->prepare("SELECT * FROM " . $this->table . " WHERE id = %d", $update_info['id']);
      $result = $wpdb->get_results($query);
      
      if (is_array($result)) {
        if (count($result) == 1) {
          $result = $result[0];
          foreach (array('language', 'transcript') as $field) {
            $result->{$field} = $update_info[$field];
          }
          $query = $wpdb->prepare(
            "UPDATE " . $this->table . " SET language = %s, transcript = %s WHERE id = %d",
            $result->language, $result->transcript, $result->id
          );
          $wpdb->query($query);
          return true;
        } 
      }
    }
    return false;
  }
  
  /**
   * Delete a queued transcript.
   * @param int $transcription_id The transcription to delete.
   * @return boolean True if the transcript was deleted.
   */
  function delete_queued_transcription($transcription_id) {
    global $wpdb;
    
    if (current_user_can('submit_transcriptions')) {
      $query = $wpdb->prepare("DELETE FROM " . $this->table . " WHERE id = %d", $transcription_id);
      return $wpdb->query($query);
    }
    return false;
  }

  /**
   * Add a queued transcription to its associated post.
   * @param int $transcription_id The transcription ID to approve.
   */
  function add_transcription_to_post($transcription_id) {
    global $wpdb;
    
    if (current_user_can('approve_transcriptions')) {
      $query = $wpdb->prepare("SELECT * from " . $this->table . " WHERE id = %d", $transcription_id);
      $result = $wpdb->get_results($query);
      if (is_array($result)) {
        if (count($result) == 1) {
          $result = (object)$result[0];

          $post = get_post($result->post_id);
          if (!empty($post)) {
            $this->save_transcript($result->post_id, $result->language, $result->transcript);
            $this->delete_queued_transcription($transcription_id);
          }
        }
      } 
    } 
  }

  /**
   * Get all the queued transcriptions for a particular user and post.
   * @param int $user_id The user ID to search for.
   * @param int $post_id The post ID to search for.
   * @return array|false The queued transcriptions for this user, or false if they don't have permissions to see queued transcriptions.
   */
  function get_queued_transcriptions_for_user_and_post($user_id, $post_id) {
    global $wpdb;

    if (current_user_can('submit_transcriptions')) {
      $query = $wpdb->prepare("SELECT * FROM " . $this->table . " WHERE user_id = %d AND post_id = %d", $user_id, $post_id);
      return $wpdb->get_results($query);
    }
    return false;
  }

  /**
   * Delete a transcript for a particular language from a post.
   * @param int $post_id The post to search.
   * @param string $language The language code to search for.
   * @return boolean True if the transcript was deleted.
   */
  function delete_transcript($post_id, $language) {
    if (current_user_can('approve_transcriptions')) {
      $post = get_post($post_id);
      if (!empty($post)) {
        $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
        unset($current_transcripts[$language]);
        update_post_meta($post_id, "provided_transcripts", $current_transcripts);
        return true;
      }
    }
    return false;
  }

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
