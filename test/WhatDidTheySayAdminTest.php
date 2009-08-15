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
        array('en' => array('name' => 'English'), 'de' => array('name' => 'German')),
        array('code' => 'en', 'action' => 'delete'),
        array('de' => array('name' => 'German'))
      ),
      array(
        array('de' => array('name' => 'German')),
        array('code' => 'en', 'action' => 'add'),
        array('en' => array('name' => 'English'), 'de' => array('name' => 'German')),
      ),
      array(
        array('en' => array('name' => 'English', 'default' => true), 'de' => array('name' => 'German')),
        array('code' => 'de', 'action' => 'default'),
        array('en' => array('name' => 'English'), 'de' => array('name' => 'German', 'default' => true)),
      ),
      array(
        array('en' => array('name' => 'English'), 'de' => array('name' => 'German')),
        array('code' => 'de', 'action' => 'rename', 'name' => 'Deutsch'),
        array('en' => array('name' => 'English'), 'de' => array('name' => 'Deutsch')),
      ),
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

    update_option('what-did-they-say-options', array('languages' => $original_options));
    
    $admin->handle_update_languages($form_submission);

    $options = get_option('what-did-they-say-options');
    $this->assertEquals($expected_results, $options['languages']);
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

  function testBuildFullDefaultLanguageInfo() {
    $admin = new WhatDidTheySayAdmin();
    $admin->all_languages = array(
      'en' => 'English',
      'de' => 'German',
      'fr' => 'French'
    );

    $admin->default_options = array(
      'languages' => array(
        'en',
        array('code' => 'de', 'default' => true),
        'it'
      )
    );

    $this->assertEquals(
      array(
        'en' => array('name' => 'English'),
        'de' => array('name' => 'German', 'default' => true),
      ), $admin->build_default_languages()
    );
  }

  function testHandleUpdateCapabilities() {
    $admin = new WhatDidTheySayAdmin();
    update_option('what-did-they-say-options', $admin->default_options);

    $admin->handle_update_capabilities(array(
      'action' => 'capabilities',
      'capabilities' => array(
        'submit_transcriptions' => 'contributor',
        'approve_transcriptions' => 'subscriber',
        'change_languages' => 'reader'
      )
    ));

    $result = get_option('what-did-they-say-options');
    $this->assertEquals(array(
      'submit_transcriptions' => 'contributor',
      'approve_transcriptions' => 'subscriber',
      'change_languages' => 'reader'
    ), $result['capabilities']);
  }
}

?>