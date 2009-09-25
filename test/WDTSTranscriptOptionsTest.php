<?php

require_once('PHPUnit/Framework.php');
require_once(dirname(__FILE__) . '/../../mockpress/mockpress.php');
require_once(dirname(__FILE__) . '/../classes/WDTSTranscriptOptions.inc');

class WDTSTranscriptOptionsTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    _reset_wp();

    wp_insert_post((object)array('ID' => 1));

    $this->to = new WDTSTranscriptOptions(1);
  }

  function testUpdateOption() {
    update_post_meta(1, "transcript_options", array('test' => false));

    $this->to->_update_option('test', true);

    $this->assertEquals(
      array('test' => true),
      get_post_meta(1, 'transcript_options', true)
    );
  }
}

?>