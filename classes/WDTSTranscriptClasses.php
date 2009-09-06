<?php

require_once('WDTSTranscript.php');

class WDTSQueuedTranscript extends WDTSTranscript { var $key = "queued_transcripts"; }
class WDTSApprovedTranscript extends WDTSTranscript { var $key = "approved_transcripts"; }

?>