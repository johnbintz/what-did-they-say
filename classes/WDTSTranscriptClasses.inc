<?php

require_once('WDTSTranscript.inc');

class WDTSQueuedTranscript extends WDTSTranscriptManager {
  var $key = "queued_transcripts";
  var $allow_multiple = true;
}

class WDTSApprovedTranscript extends WDTSTranscriptManager {
  var $key = "approved_transcripts";
  var $search_key = "approved_transcripts_words";
}

?>