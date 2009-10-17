<?php

require_once('PHPUnit/Framework.php');
require_once('MockPress/mockpress.php');
require_once(dirname(__FILE__) . '/../classes/WhatDidTheySayAdmin.inc');

class WhatDidTheySayAdminTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    _reset_wp(); 
    _set_user_capabilities('submit_transcriptions', 'approve_transcriptions', 'change_languages');
  }
  
  function testReadLanguageData() {
    $admin = new WhatDidTheySayAdmin();
    
    $this->assertTrue(count($admin->read_language_file()) > 0);
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
    _set_user_capabilities('edit_users');

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
  
  function testUpdateUserPerms() {
    $admin = new WhatDidTheySayAdmin();
    $admin->_set_up_capabilities();

    update_usermeta(1, 'transcript_capabilities', array(
      'submit_transcriptions' => true
    ));

    $admin->_update_user_perms(1, array(
      'approve_transcriptions' => 'yes'
    ));
    
    $this->assertEquals(array(
      'approve_transcriptions' => true
    ), get_usermeta(1, 'transcript_capabilities'));
  }
}

?>