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
  function set_allow_transcripts_for_post($post_id, $allow = true) {
    $current_transcripts = get_post_meta($post_id, "transcript_options", true);
    if (!is_array
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