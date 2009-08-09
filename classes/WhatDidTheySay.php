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
      $results = $wpdb->get_results(sprintf("SELECT * FROM %s WHERE post_id = %d", $this->table, $post_id));
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
}

?>
