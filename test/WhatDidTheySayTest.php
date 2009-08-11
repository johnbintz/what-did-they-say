<?php

require_once('PHPUnit/Framework.php');
require_once(dirname(__FILE__) . '/../../mockpress/mockpress.php');
require_once(dirname(__FILE__) . '/../classes/WhatDidTheySay.php');

class WhatDidTheySayTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    global $wpdb;
    $this->what = new WhatDidTheySay();
    _reset_wp();
    $wpdb = null;
  }

  function testSaveTranscription() {
    wp_insert_post(array('ID' => 1)); 

    $this->what->save_transcript(1, "en", "This is a transcript");
    $this->assertEquals(array("en" => "This is a transcript"), get_post_meta(1, "provided_transcripts", true));
    
    $this->what->save_transcript(1, "en", "this is a new transcript"); 
    $this->assertEquals(array("en" => "this is a new transcript"), get_post_meta(1, "provided_transcripts", true));
    
    $this->what->save_transcript(1, "fr", "il s'agit d'une nouvelle transcription"); 
    $this->assertEquals(array("en" => "this is a new transcript", "fr" => "il s'agit d'une nouvelle transcription"), get_post_meta(1, "provided_transcripts", true));
  }
  
  function testGetListOfLanguagesForPost() {
    update_post_meta(1, "provided_transcripts", array('en' => 'this is a new transcript', 'fr' => "il s'agit d'une nouvelle transcription"));    
    $this->assertEquals(array('en', 'fr'), $this->what->get_transcript_languages(1));

    $this->assertEquals(false, $this->what->get_transcript_languages(2));
  }
  
  function testGetQueuedTranscriptionsForPost() {
    global $wpdb;
    
    wp_insert_user(array('ID' => 1, 'first_name' => 'Test', 'last_name' => 'User'));
    wp_insert_post(array('ID' => 1));
    
    $wpdb = $this->getMock('wpdb', array('get_results', 'prepare'));
    
    $expected_query = sprintf("SELECT * FROM '%s' WHERE post_id = '%d'", $this->what->table, 1);
    
    $wpdb->expects($this->once())
         ->method('prepare')
         ->will($this->returnValue($expected_query));
    
    $wpdb->expects($this->once())
         ->method('get_results')
         ->with($expected_query)
         ->will(
           $this->returnValue(
             array(
               (object)array('id' => 1, 'post_id' => 1, 'user_id' => 1, 'language' => 'en', 'transcript' => "This is a transcript"),
               (object)array('id' => 2, 'post_id' => 1, 'user_id' => 2, 'language' => 'fr', 'transcript' => "il s'agit d'une nouvelle transcription"),
             )
           )
         );
         
    $this->assertEquals(
      array(
        (object)array(
          'id' => 1,
          'post_id' => 1,
          'user_id' => 1,
          'language' => 'en',
          'transcript' => 'This is a transcript'
        )
      ),
      $this->what->get_queued_transcriptions_for_post(1)
    );
    
    $this->assertFalse($this->what->get_queued_transcriptions_for_post(2));
  }
  
  function providerTestAddQueuedTranscriptionToPost() {
    return array(
      array(
        array(1, 1, "en", "This is a transcript"), true
      ),
      array(
        array(2, 1, "en", "This is a transcript"), false
      ),
      array(
        array(1, 2, "en", "This is a transcript"), false
      ),
      array(
        array(1, 1, "", "This is a transcript"), false
      ),
      array(
        array(1, 1, "en", ""), false
      ),
    ); 
  }
  
  /**
   * @dataProvider providerTestAddQueuedTranscriptionToPost
   */
  function testAddQueuedTranscriptionToPost($query_settings, $expected_result) {
    global $wpdb;
    
    wp_insert_user(array('ID' => 1, 'first_name' => 'Test', 'last_name' => 'User'));
    wp_insert_post(array('ID' => 1));

    $expected_query = sprintf("INSERT INTO '%s' (post_id, user_id, language, transcript) VALUES ('%d', '%d', '%s', '%s')",
                              $this->what->table,
                              1, 1, "en", "This is a transcript");

    $wpdb = $this->getMock('wpdb', array('query', 'prepare'));

    $wpdb->expects($this->once())
         ->method('prepare')
         ->will($this->returnValue($expected_query));    

    if ($expected_result === true) {
      $wpdb->expects($this->once())
           ->method('query')
           ->with($expected_query)
           ->will($this->returnValue(true));
    }
         
    $this->assertEquals($expected_result, $this->what->add_queued_transcription_to_post(
      1,
      array(
        'user_id' => 1,
        'language' => 'en',
        'transcript' => "This is a transcript"
      )
    ));
  }
  
  function providerTestIsUserAllowedToUpdate() {
    return array(
      array(
        false, array(), array(), 1, false
      ),
      array(
        false, array('edit_posts'), array(), 1, true
      ),      
      array(
        true, array(), array(2), 1, false
      ),
      array(
        true, array(), array(1), 1, true
      ),
    ); 
  }
  
  /**
   * @dataProvider providerTestIsUserAllowedToUpdate
   */
  function testIsUserAllowedToUpdate($only_allowed_users, $current_user_can, $allowed_users, $current_user_id, $expected_result) {
    update_option('what-did-they-say-options', array('allowed_users' => $allowed_users, 'only_allowed_users' => $only_allowed_users)); 
    _set_user_capabilities($current_user_can);
    wp_insert_user(array('ID' => 1, 'first_name' => 'Test', 'last_name' => 'User'));
    wp_set_current_user($current_user_id);
    
    $this->assertEquals($expected_result, $this->what->is_user_allowed_to_update());
  }
  
  function providerTestUpdateQueuedTranscription() {
    return array(
      array(
        array(), array("language" => "en", "transcript" => "This")
      ),
      array(
        array(
          (object)array('ID' => 1)
        ), array("language" => "en", "transcript" => "This")
      ),      
      array(
        array(
          (object)array('ID' => 1)
        ), array("language" => "en", "transcript" => "This", 'id' => 1)
      ),
    ); 
  }
  
  /**
   * @dataProvider providerTestUpdateQueuedTranscription
   */
  function testUpdateQueuedTranscription($valid_transcripts, $update_info) {
    global $wpdb;

    $what = $this->getMock('WhatDidTheySay', array('is_user_allowed_to_update'));
    $what->expects($this->once())
         ->method('is_user_allowed_to_update')
         ->will($this->returnValue(true));    
    
    $wpdb = $this->getMock('wpdb', array('prepare', 'get_results', 'query'));

    $wpdb->expects($this->once())
          ->method('get_results')
          ->will($this->returnValue($valid_transcripts));
    
    $in_array = false;
    foreach ($valid_transcripts as $transcript) {
      if ($transcript->id == $update_info['id']) { $in_array = true; break; }
    }
    
    if ($in_array) {
        $wpdb->expects($this->once())
            ->method('query');
    }

    wp_insert_post(array('ID' => 1)); 
    
    $what->update_queued_transcript($update_info);
  }
}

?>
