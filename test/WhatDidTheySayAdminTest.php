<?php

require_once('PHPUnit/Framework.php');
require_once('MockPress/mockpress.php');

require_once(dirname(__FILE__) . '/../classes/WhatDidTheySayAdmin.inc');
require_once(dirname(__FILE__) . '/../classes/WDTSTranscriptClasses.inc');

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

  /**
   * Integration.
   */
  function testPerformImport() {
    $admin = $this->getMock('WhatDidTheySayAdmin', array('_get_transcript_posts'));
    $admin->_import_chunk_size = 1;

    wp_insert_user(array('ID' => 1));
    wp_set_current_user(1);

    _set_user_capabilities('approve_transcriptions');

    for ($i = 1; $i <= 2; ++$i) {
      wp_insert_post(array('ID' => $i));
      update_post_meta($i, "transcript", "this is my transcript");
    }

    $admin->expects($this->once())->method('_get_transcript_posts')->will($this->returnValue(array(1,2)));
    $this->assertEquals(1, $admin->import_transcripts('en'));

    $this->assertEquals('', get_post_meta(1, "transcript", true));
    $this->assertEquals('this is my transcript', get_post_meta(2, "transcript", true));

    $this->assertEquals(array(
      array(
        'language' => 'en',
        'transcript' => 'this is my transcript',
        'user_id' => 1,
        'key' => 0
      )
    ), get_post_meta(1, 'approved_transcripts', true));

    delete_post_meta(2, 'transcript');

    $admin = $this->getMock('WhatDidTheySayAdmin', array('_get_transcript_posts'));
    $admin->_import_chunk_size = 1;
    
    $admin->expects($this->once())->method('_get_transcript_posts')->will($this->returnValue(array()));

    $this->assertEquals(0, $admin->import_transcripts('en'));
  }

  function providerTestCleanChild() {
    return array(
      array(
        array(
          'script' => '<script>remove</script>test',
          'style' => '<style>remove</style>test',
          'link' => '<link>remove</link>test',
          'keep' => '<b>remove</b>test'
        ),
        array(
          'allow_html' => true
        ),
        array(
          'script' => 'test',
          'style' => 'test',
          'link' => 'test',
          'keep' => '<b>remove</b>test'
        ),
      ),
      array(
        array(
          'remove' => '<b>remove</b>test',
          'children' => array(
            'alsoremove' => '<i>remove</i>test'
          )
        ),
        array(
          'allow_html' => false
        ),
        array(
          'remove' => 'removetest',
          'children' => array(
            'alsoremove' => 'removetest',
          )
        ),
      ),
    );
  }

  /**
   * @dataProvider providerTestCleanChild
   */
  function testCleanChild($nodes, $options, $expected_result) {
    $admin = new WhatDidTheySayAdmin();
    
    update_option('what-did-they-say-options', $options);

    $this->assertEquals($expected_result, $admin->_clean_child($nodes));
  }

  function testApproveTranscript() {
    wp_insert_post(array('ID' => 1));
    update_post_meta(1, "queued_transcripts", array(
      array(
        'language' => 'en',
        'transcript' => 'This is a test transcript',
        'user_id' => 5,
        'key' => 1
      )
    ));

    $admin = new WhatDidTheySayAdmin();

    $this->assertTrue($admin->_approve_transcript(1, 1));

    $this->assertEquals(
      array(
        array(
          'language' => 'en',
          'transcript' => 'This is a test transcript',
          'user_id' => 5,
          'key' => 0
        )
      ),
      get_post_meta(1, "approved_transcripts", true)
    );

    $this->assertEquals(
      array(), get_post_meta(1, "queued_transcripts", true)
    );
  }
}

?>