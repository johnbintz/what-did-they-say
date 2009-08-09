<?php

/**
 * WhatDidTheySay manages transcriptions that are attached as metadata
 * individual posts. Transcriptions can be updated, deleted, and moved to
 * other posts as necessary.
 */
class WhatDidTheySay {
  function save_transcript($post_id, $language, $transcript) {
    $current_transcripts = get_post_meta($post_id, "provided_transcripts", true);
    if (!is_array($current_transcripts)) { $current_transcripts = array(); }
    $current_transcripts[$language] = $transcript;
    return update_post_meta($post_id, "provided_transcripts", $current_transcripts);
  }
}

?>
