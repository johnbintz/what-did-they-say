<?php

class WDTSTranscriptOptions {
  var $post_id;
  
  function __construct($post_id = null) {
    if (is_numeric($post_id)) { $this->post_id = $post_id; }
  }
  
  function WDTSTranscriptOptions($post_id = null) { $this->__construct($post_id); }
  
  /**
   * Set whether or not the indicated post can accept new queued transcriptions.
   * @param int $post_id The post ID to affect.
   * @param boolean $allow True if the post can accept new queued transcriptions.
   */
  function set_allow_transcripts($allow = true) { $this->_update_option('allow_transcripts', $allow); }

  /**
   * See if the indicated post is accepting new transcripts.
   * @return boolean True if the post is acceptin new transcripts.
   */
  function are_new_transcripts_allowed() {
    $options = $this->_get_transcript_options();
    return isset($options['allow_transcripts']) ? $options['allow_transcripts'] : false;
  }

  function _get_transcript_options() {
    $current_transcripts = get_post_meta($this->post_id, "transcript_options", true);
    if (!is_array($current_transcripts)) { $current_transcripts = array(); }
    return $current_transcripts;
  }

  function _update_option($option, $value) {
    $current_options = $this->_get_transcript_options();
    $current_transcripts[$option] = $value;
    update_post_meta($this->post_id, "transcript_options", $current_transcripts);
  }
}

?>