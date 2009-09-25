<?php

require_once('PHPUnit/Framework.php');
require_once(dirname(__FILE__) . '/../classes/WDTSLanguageOptions.inc');
require_once(dirname(__FILE__) . '/../../mockpress/mockpress.php');

class WDTSLanguageOptionsTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    _reset_wp();
    $this->l = new WDTSLanguageOptions();
  }
  
  function testGetDefaultLanguage() {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array(),
        'de' => array('default' => true)
      )
    ));
    
    $this->assertEquals('de', $this->l->get_default_language());
  }

  function providerTestSetDefaultLanguage() {
    return array(
      array('en', true,  array('en' => array('default' => true), 'de' => array())),
      array('de', false, array('en' => array(), 'de' => array('default' => true))),
      array('fr', false, array('en' => array(), 'de' => array('default' => true))),
    );
  }

  /**
   * @dataProvider providerTestSetDefaultLanguage
   */
  function testSetDefaultLanguage($set_default, $expected_result, $expected_language_info) {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array(),
        'de' => array('default' => true)
      )
    ));

    $this->assertEquals($expected_result, $this->l->set_default_language($set_default));

    $this->assertEquals($expected_language_info, $this->l->get_languages());
  }
  
  function testGetLanguageName() {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array('name' => 'English')
      )
    ));
    
    $this->assertEquals('English', $this->l->get_language_name('en'));
  }

  function testGetLanguages() {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array('name' => 'English'),
        'de' => array('name' => 'German')
      )
    ));
    
    $this->assertEquals(array(
        'en' => array('name' => 'English'),
        'de' => array('name' => 'German')
      ),
      $this->l->get_languages()
    );
  }
  
  function providerTestDeleteLanguage() {
    return array(
      array('de', array('name' => 'German')),
      array('fr', false)
    ); 
  }
  
  /**
   * @dataProvider providerTestDeleteLanguage
   */
  function testDeleteLanguage($code, $expected_result) {
    $check = array(
      'en' => array('name' => 'English'),
      'de' => array('name' => 'German')
    );    

    update_option($this->l->key, array(
      'languages' => $check
    ));
    
    $this->assertEquals(
      $expected_result,
      $this->l->delete_language($code)
    );
    
    if ($expected_result) { unset($check[$code]); }    
    
    $this->assertEquals(
      array('languages' => $check),
      get_option($this->l->key)
    );
  }

  function providerTestAddLanguage() {
    return array(
      array('de', 'German', true),
      array('de', '', false),
      array('', 'German', false),
      array('en', 'English', false),
    ); 
  }

  /**
   * @dataProvider providerTestAddLanguage
   */
  function testAddLanguage($code, $name, $expected_result) {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array('name' => 'English')
      )
    ));
    
    $this->assertEquals(
      $expected_result,
      $this->l->add_language($code, array('name' => $name))
    );
    
    $check = array(    
      'en' => array('name' => 'English')
    );
    
    if ($expected_result) {
      $check[$code] = array('name' => $name); 
    }
    
    $this->assertEquals(array(
        'languages' => $check
      ),
      get_option($this->l->key)
    );
  }
  
  function providerTestRenameLanguage() {
    return array(
      array('en', 'Anglais', true),
      array('de', 'Anglais', false),
      array('en', '', false),
      array('', 'Anglais', false),
    ); 
  }
  
  /**
   * @dataProvider providerTestRenameLanguage
   */
  function testRenameLanguage($code_to_rename, $new_name, $expected_result) {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array('name' => 'English')
      )
    ));

    $this->assertEquals(
      $expected_result,
      $this->l->rename_language($code_to_rename, $new_name)
    );
    
    $check = array(
      'en' => array('name' => 'English')
    );
    if ($expected_result) {
      $check['en']['name'] = $new_name;
    }

    $this->assertEquals(array(
        'languages' => $check
      ),
      get_option($this->l->key)
    );
  }
}

?>