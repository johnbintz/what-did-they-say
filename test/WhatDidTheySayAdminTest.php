<?php

require_once('PHPUnit/Framework.php');
require_once(dirname(__FILE__) . '/../../mockpress/mockpress.php');
require_once(dirname(__FILE__) . '/../classes/WhatDidTheySayAdmin.php');

class WhatDidTheySayAdminTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    _reset_wp(); 
  }
  
  function testReadLanguageData() {
    $admin = new WhatDidTheySayAdmin();
    
    $this->assertTrue(count($admin->read_language_file()) > 0);
  } 
  
  function testHandleUpdateLanguages() {
    $admin = new WhatDidTheySayAdmin();
    $admin->all_languages = array(
      'en' => 'English',
      'de' => 'German'
    );
    
    $admin->handle_update_languages(array('en' => array(), 'de' => array('default' => 'yes'), 'meow' => array()));
    
    $this->assertEquals(array('en', array('code' => 'de', 'default' => true)), get_option('what-did-they-say-languages'));
  }
}

?>