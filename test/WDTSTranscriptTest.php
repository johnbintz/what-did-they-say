<?php

require_once('PHPUnit/Framework.php');
require_once(dirname(__FILE__) . '/../../mockpress/mockpress.php');
require_once(dirname(__FILE__) . '/../classes/WDTSTranscript.php');

class WDTSTranscriptTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    _reset_wp();
    
    wp_insert_user(array('ID' => 1)); 
    wp_set_current_user(1);
    wp_insert_post(array('ID' => 1)); 
    
    $this->w = new WDTSTranscriptManager(1);
    $this->w->key = "test";
  }
  
  function testSaveTranscript() {
    $this->w->save_transcript(array(
      'language' => 'en',
      'transcript' => 'this is a transcript'
    ));
    
    $this->assertEquals(
      array(
        array(
          'language' => 'en',
          'transcript' => 'this is a transcript',
          'user_id' => 1
        ) 
      ),
      get_post_meta(1, $this->w->key, true)
    );

    $this->w->save_transcript(array(
      'language' => 'en',
      'transcript' => 'this is another transcript'
    ));

    $this->assertEquals(
      array(
        array(
          'language' => 'en',
          'transcript' => 'this is another transcript',
          'user_id' => 1
        ) 
      ),
      get_post_meta(1, $this->w->key, true)
    );

    $this->w->save_transcript(array(
      'language' => 'fr',
      'transcript' => "il s'agit d'une nouvelle transcription"
    ));

    $this->assertEquals(
      array(
        array(
          'language' => 'en',
          'transcript' => 'this is another transcript',
          'user_id' => 1
        ), 
        array(
          'language' => 'fr',
          'transcript' => "il s'agit d'une nouvelle transcription",
          'user_id' => 1
        ), 
      ),
      get_post_meta(1, $this->w->key, true)
    );
  } 
  
  function testDeleteTranscript() {
    update_post_meta(1, $this->w->key, array(
      array(
        'language' => 'en',
        'transcript' => 'this is another transcript',
        'user_id' => 1
      ), 
      array(
        'language' => 'fr',
        'transcript' => "il s'agit d'une nouvelle transcription",
        'user_id' => 1
      ), 
    ));
    
    $this->w->delete_transcript('en');

    $this->assertEquals(array(
      array(
        'language' => 'fr',
        'transcript' => "il s'agit d'une nouvelle transcription",
        'user_id' => 1
      ), 
    ), get_post_meta(1, $this->w->key, true));
  }
  
  function testGetLanguages() {
    update_post_meta(1, $this->w->key, array(
      array(
        'language' => 'en',
        'transcript' => 'this is another transcript',
        'user_id' => 1
      ), 
      array(
        'language' => 'fr',
        'transcript' => "il s'agit d'une nouvelle transcription",
        'user_id' => 1
      ), 
    ));

    $this->assertEquals(array('en', 'fr'), $this->w->get_languages());    
  }
}

?>