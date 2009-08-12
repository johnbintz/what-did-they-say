<?php

/**
 * WhatDidTheySay manages transcriptions that are attached as metadata
 * individual posts. Transcriptions can be updated, deleted, and moved to
 * other posts as necessary.
 */
class WhatDidTheySay {
  var $version = "0.1";
  
  function WhatDidTheySay() {
    global $wpdb;
    
    $this->table =  $wpdb->prefix . "provided_transcripts";
  }
  
  function install() {
    global $wpdb;
    
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
      dbDelta($sql);
      
      update_option('what-did-they-say-version', $version);
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
    if ($this->is_user_allowed_to_update()) {
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
    global $wpdb;
    
    if ($this->is_user_allowed_to_update()) {
      $post = get_post($post_id);
      if (!empty($post)) {
        $query = $wpdb->prepare('SELECT * FROM %s WHERE post_id = %d', $this->table, $post_id);
        $results = $wpdb->get_results($query);
        if (!empty($results)) {
          $valid_results = array();
          foreach ($results as $result) {
            $user = get_userdata($result->user_id);
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
    global $wpdb;
    
    if ($this->is_user_allowed_to_update()) {
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
            $user = get_userdata($user_id);
            if (!empty($user)) {
              $query = $wpdb->prepare(
                "INSERT INTO %s (post_id, user_id, language, transcript) VALUES (%d, %d, %s, %s)",
                $this->table, $post_id, $user_id, $language, $transcript
              );
              
              return $wpdb->query($query);
            }
          }
        }
      }
    }
    return false;
  }
  
  function is_user_allowed_to_update() {
    $options = get_option('what-did-they-say-options');
    $user_info = wp_get_current_user();
    
    $ok = false;
    if ($options['only_allowed_users']) {
      $ok = in_array($user_info->ID, $options['allowed_users']);
    } else {
      $ok = true;
      if (!current_user_can('edit_posts')) {
        $ok = in_array($user_info->ID, $options['allowed_users']);
      }
    }
    
    return $ok;
  }
  
  /**
   * Update a queued transcript.
   * @param array $update_info The info on the transcript being updated.
   * @return boolean True if the transcript was updated.
   */
  function update_queued_transcription($update_info) {
    global $wpdb;

    if ($this->is_user_allowed_to_update()) {
      $query = $wpdb->prepare("SELECT * FROM %s WHERE id = %d", $this->table, $update_info['id']);
      $result = $wpdb->get_results($query);
      
      if (is_array($result)) {
        if (count($result) == 1) {
          $result = $result[0];
          foreach (array('language', 'transcript') as $field) {
            $result->{$field} = $update_info[$field];
          }
          $query = $wpdb->prepare(
            "UPDATE %s SET language = %s, transcript = %s WHERE id = %d",
            $this->table, $result->language, $result->transcript, $result->id
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
    
    if ($this->is_user_allowed_to_update()) {
      $query = $wpdb->prepare("SELECT id FROM %s WHERE id = %d", $this->table, $transcription_id);
      if (!is_null($wpdb->get_var($query))) {
        $query = $wpdb->prepare("DELETE FROM %s WHERE id = %d", $this->table, $transcription_id);
        $wpdb->query($query); 
        
        return true;
      }
    }
    return false;
  }
  
  function add_transcription_to_post($transcription_id) {
    global $wpdb;
    
    if ($this->is_user_allowed_to_update()) {
      $query = $wpdb->prepare("SELECT * from %s WHERE id = %d", $this->table, $transcription_id);
      $result = $wpdb->get_results($query);
      if (is_array($result)) {
        if (count($result) == 1) {
          $result = (object)$result[0];

          $post = get_post($result->post_id);
          if (!empty($post)) {
            $this->save_transcript($result->post_id, $result->language, $result->transcript);
            
            $query = $wpdb->prepare("DELETE FROM %s WHERE id = %d", $this->table, $transcription_id);
            $result = $wpdb->query($query);
          } 
        }
      } 
    } 
  }
  
  function delete_transcript($post_id, $language) {
    if ($this->is_user_allowed_to_update()) {
      $post = get_post($post_id);
      if (!empty($post)) {
        $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
        unset($current_transcripts[$language]);
        update_post_meta($post_id, "provided_transcripts", $current_transcripts);
      } 
    }
  }
}

?>
