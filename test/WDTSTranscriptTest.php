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
		$this->w->search_key = "test_search";
  }
  
  function testSaveTranscript() {
    $this->w->allow_multiple = false;
    
    $this->w->save_transcript(array(
      'language' => 'en',
      'transcript' => 'this is a transcript'
    ));
    
    $this->assertEquals(
      array(
        array(
          'language' => 'en',
          'transcript' => 'this is a transcript',
          'user_id' => 1,
          'key' => 0
        ) 
      ),
      get_post_meta(1, $this->w->key, true)
    );

		$this->assertEquals("this is a transcript", get_post_meta(1, $this->w->search_key, true));

    $this->w->save_transcript(array(
      'language' => 'en',
      'transcript' => 'this is another transcript'
    ));

    $this->assertEquals(
      array(
        array(
          'language' => 'en',
          'transcript' => 'this is another transcript',
          'user_id' => 1,
          'key' => 0
        ) 
      ),
      get_post_meta(1, $this->w->key, true)
    );

    $this->assertEquals("this is another transcript", get_post_meta(1, $this->w->search_key, true));

    $this->w->save_transcript(array(
      'language' => 'fr',
      'transcript' => "il s'agit d'une nouvelle transcription"
    ));

    $this->assertEquals(
      array(
        array(
          'language' => 'en',
          'transcript' => 'this is another transcript',
          'user_id' => 1,
          'key' => 0
        ), 
        array(
          'language' => 'fr',
          'transcript' => "il s'agit d'une nouvelle transcription",
          'user_id' => 1,
          'key' => 1
        ), 
      ),
      get_post_meta(1, $this->w->key, true)
    );
    
    $this->assertEquals("this is another transcript il s'agit d'une nouvelle transcription", get_post_meta(1, $this->w->search_key, true));

    $this->w->allow_multiple = true;
    
    $this->w->save_transcript(array(
      'language' => 'en',
      'transcript' => 'this is yet another transcript'
    ));

    $this->assertEquals(
      array(
        array(
          'language' => 'en',
          'transcript' => 'this is another transcript',
          'user_id' => 1,
          'key' => 0
        ), 
        array(
          'language' => 'fr',
          'transcript' => "il s'agit d'une nouvelle transcription",
          'user_id' => 1,
          'key' => 1
        ), 
        array(
          'language' => 'en',
          'transcript' => 'this is yet another transcript',
          'user_id' => 1,
          'key' => 2
        ), 
      ),
      get_post_meta(1, $this->w->key, true)
    );

    $this->assertEquals("this is another transcript il s'agit d'une nouvelle transcription this is yet another transcript", get_post_meta(1, $this->w->search_key, true));
  }
  
  function testDeleteTranscript() {
    $this->w->save_transcript(array(
      'language' => 'en',
      'transcript' => 'this is another transcript'
    ));
    
    $this->w->save_transcript(array(
      'language' => 'fr',
      'transcript' => "il s'agit d'une nouvelle transcription",
    ));

    $this->w->delete_transcript('en');

    $this->assertEquals(array(
      array(
        'language' => 'fr',
        'transcript' => "il s'agit d'une nouvelle transcription",
        'user_id' => 1,
        'key' => 1
      ), 
    ), get_post_meta(1, $this->w->key, true));

    $this->assertEquals("il s'agit d'une nouvelle transcription", get_post_meta(1, $this->w->search_key, true));
  }
  
  function testDeleteTranscriptByKey() {
    $this->w->save_transcript(array(
      'language' => 'en',
      'transcript' => 'this is another transcript'
    ));
    
    $this->w->save_transcript(array(
      'language' => 'fr',
      'transcript' => "il s'agit d'une nouvelle transcription",
    ));
    
    $this->assertEquals(
      $this->w->delete_transcript_by_key(0),
      array(
        'language' => 'en',
        'transcript' => "this is another transcript",
        'user_id' => 1,
        'key' => 0
      )
    );

    $this->assertEquals(array(
      array(
        'language' => 'fr',
        'transcript' => "il s'agit d'une nouvelle transcription",
        'user_id' => 1,
        'key' => 1
      ), 
    ), get_post_meta(1, $this->w->key, true));

    $this->assertEquals("il s'agit d'une nouvelle transcription", get_post_meta(1, $this->w->search_key, true));
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