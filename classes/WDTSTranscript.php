<?php

class WDTSTranscriptManager { 
  var $key = null;
  var $post_id = null;
  
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
    $user = wp_get_current_user();
    if (!empty($user)) {
      $transcript_info = (array)$transcript_info;          
      $transcript_info['user_id'] = $user->ID;
      
      if (($transcripts = $this->_get_transcripts_metadata()) !== false) {      
        $new_transcripts = array();
        $was_added = false;
        foreach ($transcripts as $transcript) {
          if ($transcript['language'] == $transcript_info['language']) {
            $was_added = true;
            $new_transcripts[] = $transcript_info;
          } else {
            $new_transcripts[] = $transcript; 
          }
        }
        if (!$was_added) { $new_transcripts[] = $transcript_info; }
        
        return update_post_meta($this->post_id, $this->key, $new_transcripts);
      }
    }
    return false;       
  }
  
  function delete_transcript($language = null) {
    if (($transcripts = $this->_get_transcripts_metadata()) !== false) {      
      $new_transcripts = array();
      foreach ($transcripts as $transcript) {
        if ($transcript['language'] != $language) { $new_transcripts[] = $transcript; } 
      }

      return update_post_meta($this->post_id, $this->key, $new_transcripts);
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
  
  function get_languages() {
    $languages = array();
    if (($transcripts = $this->_get_transcripts_metadata()) !== false) {      
      foreach ($transcripts as $transcript) {
        $languages[$transcript['language']] = true; 
      }
    }
    return array_keys($languages);
  }
}

?>