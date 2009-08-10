<?php

/**
 * WhatDidTheySay manages transcriptions that are attached as metadata
 * individual posts. Transcriptions can be updated, deleted, and moved to
 * other posts as necessary.
 */
class WhatDidTheySay {
  function WhatDidTheySay() {
    global $wpdb;
    
    $this->table =  $wpdb->prefix . "provided_transcripts";
  }
  
  function save_transcript($post_id, $language, $transcript) {
    $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
    if (!is_array($current_transcripts)) { $current_transcripts = array(); }
    $current_transcripts[$language] = $transcript;
    return update_post_meta($post_id, "provided_transcripts", $current_transcripts);
  }
  
  function get_transcript_languages($post_id) {
    $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
    if (is_array($current_transcripts)) {
      return array_keys($current_transcripts); 
    }
    return false;
  }
  
  function get_queued_transcriptions_for_post($post_id) {
    global $wpdb;
    
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
    return false; 
  }
  
  function add_queued_transcription_to_post($post_id, $transcript_info) {
    global $wpdb;
    
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
    return false;
  }
}

?>
