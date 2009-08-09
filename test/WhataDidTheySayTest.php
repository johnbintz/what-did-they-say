<?php

require_once('PHPUnit/Framework.php');
require_once(dirname(__FILE__) . '/../../mockpress/mockpress.php');
require_once(dirname(__FILE__) . '/../classes/WhatDidTheySay.php');

class WhatDidTheySayTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->what = new WhatDidTheySay();
  }

  function testSaveTranscription() {
    $this->what->save_transcript(1, "en", "This is a transcript");
    $this->assertEquals(array("en" => "This is a transcript"), get_post_meta(1, "provided_transcripts", true));
    
    $this->what->save_transcript(1, "en", "this is a new transcript"); 
    $this->assertEquals(array("en" => "this is a new transcript"), get_post_meta(1, "provided_transcripts", true));
    
    $this->what->save_transcript(1, "fr", "il s'agit d'une nouvelle transcription"); 
    $this->assertEquals(array("en" => "this is a new transcript", "fr" => "il s'agit d'une nouvelle transcription"), get_post_meta(1, "provided_transcripts", true));
  }
}

?>
