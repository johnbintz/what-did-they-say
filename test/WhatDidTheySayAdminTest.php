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

  function providerTestHandleUpdateLanguages() {
    return array(
      array(
        array(
          array('en' => array('name' => 'English'), 'de' => array('name' => 'German')),
          array('code' => 'en', 'action' => 'delete'),
          array('de' => array('name' => 'German'))
        )
      )
    );
  }

  /**
   * @dataProvider providerTestHandleUpdateLanguages
   */
  function testHandleUpdateLanguages($original_options, $form_submission, $expected_results) {
    $admin = new WhatDidTheySayAdmin();
    $admin->all_languages = array(
      'en' => 'English',
      'de' => 'German',
      'fr' => 'French'
    );

    update_option('what-did-they-say-options', array('languages', $original_options));
    
    $admin->handle_update_languages($form_submission);

    $options = get_option('what-did-they-say-options');
    $this->assertEquals($expected_results, $options['languages']);
  }

  function testHandleUpdateAllowedUsers() {
    $admin = new WhatDidTheySayAdmin();

    wp_insert_user((object)array('ID' => 1));
    
    $admin->handle_update_allowed_users(array(1, 2));

    $options = get_option('what-did-they-say-options');
    $this->assertEquals(array(1), $options['allowed_users']);
  }

  function testHandleUpdateOptions() {
    $admin = new WhatDidTheySayAdmin();

    update_option('what-did-they-say-options', array('only_allowed_users' => false));

    $admin->handle_update_options(array(
      'only_allowed_users' => 'yes'
    ));

    $options = get_option('what-did-they-say-options');
    $this->assertTrue($options['only_allowed_users']);
    
  }
}

?>