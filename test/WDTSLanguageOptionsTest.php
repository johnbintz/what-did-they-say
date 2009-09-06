<?php

require_once('PHPUnit/Framework.php');
require_once(dirname(__FILE__) . '/../classes/WDTSLanguageOptions.php');
require_once(dirname(__FILE__) . '/../../mockpress/mockpress.php');

class WDTSLanguageOptionsTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->l = new WDTSLanguageOptions();
  }
  
  function testGetDefaultLanguage() {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array('default' => false),
        'de' => array('default' => true)
      )
    ));
    
    $this->assertEquals('de', $this->l->get_default_language());
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
  
  function testDeleteLanguage() {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array('name' => 'English'),
        'de' => array('name' => 'German')
      )
    ));
    
    $this->l->delete_language('en');
    
    $this->assertEquals(array(
        'languages' => array(
          'de' => array('name' => 'German')
        )
      ),
      get_option($this->l->key)
    );
  }

  function testAddLanguage() {
    update_option($this->l->key, array(
      'languages' => array(
        'en' => array('name' => 'English')
      )
    ));
    
    $this->l->add_language('de', array('name' => 'German'));
    
    $this->assertEquals(array(
        'languages' => array(
          'en' => array('name' => 'English'),
          'de' => array('name' => 'German')
        )
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
      $result = $this->l->rename_language($code_to_rename, $new_name)
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