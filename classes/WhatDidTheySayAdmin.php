<?php

/**
 * Administrative functions for What Did They Say?!?
 */
class WhatDidTheySayAdmin {
  var $default_options = array(
    'languages' => array(
      array('code' => 'en', 'default' => true),
      'fr',
      'es',
      'it',
      'de'
    ),
    'capabilities' => array(
      'submit_transcriptions' => 'administrator',
      'approve_transcriptions' => 'administrator',
      'change_languages' => 'administrator'
    )
  );
  
  var $capabilities = array();
  
  var $language_file;
  var $all_languages = array();
  var $notices = array();

  /**
   * Initialize the admin interface.
   * @param WhatDidTheySay $what_did_they_say The WhatDidTheySay object to use for all transcript transactions.
   */
  function WhatDidTheySayAdmin() {
    $this->language_file = dirname(__FILE__) . '/../data/lsr-language.txt';
  }
  
  /**
   * Initialize the object.
   */
  function init() {
    $this->capabilities = array(
      'submit_transcriptions'  => __('Submit transcriptions to a post', 'what-did-they-say'),
      'approve_transcriptions' => __('Approve transcriptions to a post', 'what-did-they-say'),
      'change_languages'      => __('Change the available languages', 'what-did-they-say')
    );
    
    add_action('admin_menu', array(&$this, 'admin_menu'));
    add_action('admin_notices', array(&$this, 'admin_notices'));
    add_action('admin_init', array(&$this, 'admin_init'));

    wp_enqueue_script('prototype');

    add_filter('user_has_cap', array(&$this, 'user_has_cap'), 5, 3);
    add_filter('the_media_transcript', array(&$this, 'the_media_transcript'));
    add_filter('the_language_name', array(&$this, 'the_language_name'));
    
    add_filter('wp_footer', array(&$this, 'wp_footer'));
    
    if (isset($_REQUEST['wdts'])) {
      if (isset($_REQUEST['wdts']['_nonce'])) {
        if (wp_verify_nonce($_REQUEST['wdts']['_nonce'], 'what-did-they-say')) {
          $this->handle_update($_REQUEST['wdts']);
        }
      } 
    }

    $this->read_language_file();
  }

  /**
   * Handle admin_init action.
   */
  function admin_init() {
    wp_enqueue_script('scriptaculous-effects');
  }

  /**
   * Handle the_media_transcript filter.
   * @param string $transcript The transcription text.
   * @return string The processed transcription text.
   */
  function the_media_transcript($transcript) {
    return '<div class="transcript">' . $transcript . '</div>'; 
  }

  /**
   * Handle the_language_name filter.
   * @param string $language The name of the language.
   * @return string The processed language name.
   */
  function the_language_name($language) {
    return '<h3 class="transcript-language">' . $language . '</h3>';
  }

  /**
   * Handle the wp_footer action.
   */
  function wp_footer() { ?>
    <script type="text/javascript">
      $$('.transcript-bundle').each(function(d) {
        var select = d.select("select");
        if (select.length == 1) {
          select = select[0];
          var toggle_transcripts = function() {
            d.select(".transcript-holder").each(function(div) {              
              div.hasClassName($F(select)) ? div.show() : div.hide();
            }); 
          };
          Event.observe(select, 'change', toggle_transcripts);
          Event.observe(window, 'load', toggle_transcripts)
        }
      });
    </script>
  <?php }
  
  /**
   * Handle the user_has_cap filter.
   * @param array $capabilities The capabilities the user already has.
   * @param array $requested_capabilities The capabilities requested by current_user_can.
   * @param object $capability_name
   * @return array The list of capabilities this user now has.
   */
  function user_has_cap($capabilities, $requested_capabilities, $capability_name) {
    $options = get_option('what-did-they-say-options');
    if (is_array($options)) {
      $role_cascade = array('administrator', 'editor', 'author', 'contributor', 'subscriber');
      $allowed_roles = array();
      $capture_roles = false;

      for ($i = 0; $i < count($role_cascade); ++$i) {
        if (in_array($role_cascade, $capabilities)) { $capture_roles = true; }
        if ($capture_roles) { $allowed_roles[] = $role_cascade[$i]; }
      }

      foreach ($requested_capabilities as $requested_capability) {
        if (in_array($options['capabilities'][$requested_capability], $allowed_roles)) {
          $capabilities[$requested_capability] = true;
        }
      }
    }

    return $capabilities;
  }

  /**
   * Handle show_admin action.
   */
  function admin_notices() {
    if (!empty($this->notices)) {
      echo '<div class="updated fade">';
        foreach ($this->notices as $notice) { echo "<p>" . $notice . "</p>"; }
      echo '</div>'; 
    }
  }

  /**
   * Handle an update to options.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   */
  function handle_update($info) {
    foreach (get_class_methods($this) as $method) {
      if (strpos($method, "handle_update_") === 0) {
        $result = $this->{$method}($info);
        if (!empty($result)) { $this->notices[] = $result; }
      } 
    }
  }

  /**
   * Handle updates to queued transcripts.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   * @return string|false A string if a message is to be displayed, or false if no message.
   */
  function handle_update_queue_transcript($info) {
    $updated = false;
    if (current_user_can('submit_transcriptions')) {
      $queued_transcript = new WDTSTranscriptOptions($info['post_id']));
      
      if ($this->what_did_they_say->get_allow_transcripts_for_post() {
        switch ($info['action']) {
          case 'submit_queued_transcript':
            $result = $this->what_did_they_say->add_queued_transcription_to_post($info['post_id'], $info);
            if ($result) {
              $updated = __('Transcript added to queue.', 'what-did-they-say');
            }
        }
      }
    }
    return $updated;
  }

  /**
   * Handle updates to post transcripts.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   * @return string|false A string if a message is to be displayed, or false if no message.
   */
  function handle_update_post_transcripts($info) {
    $updated = false;
    if (current_user_can('approve_transcriptions')) {
      $options = get_option('what-did-they-say-options');

      switch ($info['action']) {
        case "manage_post_transcripts":
          foreach ($info['transcripts'] as $language => $transcript) {
            switch ($language) {
              case "_allow":
                $allow = true;
                break;
              default:
                $this->what_did_they_say->save_transcript($info['post_id'], $language, $transcript);
                break;
            }
          }

          $this->what_did_they_say->set_allow_transcripts_for_post($info['post_id'], isset($info['allow_on_post']));

          $queued_transcriptions = $this->what_did_they_say->get_queued_transcriptions_for_post($info['post_id']);
          if (is_array($queued_transcriptions)) {
            $transcriptions_to_delete = array();

            foreach ($queued_transcriptions as $transcription) { $transcriptions_to_delete[$transcription->id] = true; }
            if (isset($post_transcript_info['queue'])) {
              foreach ($post_transcript_info['queue'] as $id => $keep) { unset($transcriptions_to_delete[$id]); }
            }

            foreach (array_keys($transcriptions_to_delete) as $id) {
              $this->what_did_they_say->delete_queued_transcription($id);
            }
          }

          $updated = __('Transcripts updated.', 'what-did-they-say');
          break;
      }
    }
    return $updated;
  }

  /**
   * Handle updates to languages.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   * @return string|false A string if a message is to be displayed, or false if no message.
   */
  function handle_update_languages($info) {
    $updated = false;
    if (current_user_can('change_languages')) {
      $language_options = new WDTSLanguageOptions();
      
      switch ($info['action']) {
        case "delete":
          if ($result = $language_options->delete_language($info['code'])) {
            $updated = sprintf(__('%s deleted.', 'what-did-they-say'), $result['name']);
          }
          break;
        case "add":
          $this->read_language_file();
          if (isset($this->all_languages[$info['code']])) {
            
            
            $options['languages'][$info['code']] = array('name' => $this->all_languages[$info['code']]);
            $updated = sprintf(__('%s added.', 'what-did-they-say'), $this->all_languages[$info['code']]);
          }
          break;
        case "default":
          if (isset($options['languages'][$info['code']])) {
            foreach ($options['languages'] as $code => $lang_info) {
              if ($code == $info['code']) {
                $options['languages'][$code]['default'] = true;
                $updated = sprintf(__('%s set as default.', 'what-did-they-say'), $lang_info['name']);
              } else {
                unset($options['languages'][$code]['default']);
              }
            }
          }
          break;
        case "rename":
          if (isset($options['languages'][$info['code']])) {
            if (!empty($info['name'])) {
              $updated = sprintf(__('%1$s renamed to %2$s.', 'what-did-they-say'), $options['languages'][$info['code']]['name'], $info['name']);
              $options['languages'][$info['code']]['name'] = $info['name'];
            }
          }
          break;
      }
    }
    return $updated;
  }

  /**
   * Handle updates to user capabilities.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   * @return string|false A string if a message is to be displayed, or false if no message.
   */
  function handle_update_capabilities($info) {
    $updated = false;
    if (current_user_can('edit_users')) {
      $options = get_option('what-did-they-say-options');
      switch ($info['action']) {
        case "capabilities":
          if (isset($info['capabilities'])) {
            foreach (array_keys($this->default_options['capabilities']) as $capability) {
              if (isset($info['capabilities'][$capability])) {
                $options['capabilities'][$capability] = $info['capabilities'][$capability];
              }
            }
            $updated = __('User capabilities updated.', 'what-did-they-say');
          }
          break;
      }
      if ($updated !== false) {
        update_option('what-did-they-say-options', $options);
      }
    }
    return $updated;
  }

  /**
   * Handle resettings what-did-they-say-options.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   * @return string|false A string if a message is to be displayed, or false if no message.
   */
  function handle_update_reset_options($info) {
    $updated = false;
    if (current_user_can('manage_options')) {
      switch ($info['action']) {
        case "reset_options":
          delete_option('what-did-they-say-options');
          $this->install();
          $updated = __('<strong>What Did They Say?!?</strong> options reset.', 'what-did-they-say');
          break;
      }
    }
    return $updated;
  }

  /**
   * Read a data file containing all the known languages on Earth.
   * The data originally came from http://www.langtag.net/, specifically http://www.langtag.net/registries/lsr-language.txt.
   * The data file format is tab-delimited, with the following fields:
   *   language_code date_added name_of_language additional_information
   * @return array The list of all known languages on Earth as code => language.
   */
  function read_language_file() {
    if (file_exists($this->language_file)) {
      foreach (file($this->language_file) as $language) {
        $language = trim($language);
        list($code, $date_added, $name, $additional) = explode("\t", $language);
        $this->all_languages[$code] = $name;
      } 
    }
    return $this->all_languages;
  }

  /**
   * Handle plugin installation.
   */
  function install() {
    $this->read_language_file();
    $options = get_option('what-did-they-say-options');
    if (empty($options)) {
      $this->default_options['languages'] = $this->build_default_languages();
      ksort($this->default_options['languages']);
      update_option('what-did-they-say-options', $this->default_options);
    }
  }

  /**
   * From $this->default_options, fill in the language details from the language file.
   * @return array The language info will all details filled in.
   */
  function build_default_languages() {
    $full_default_language_info = array();
    foreach ($this->default_options['languages'] as $info) {
      $code = null;
      if (is_string($info)) {
        $code = $info;
        $default = false;
      }
      if (is_array($info)) {
        extract($info);
      }
      if (isset($this->all_languages[$code])) {
        $full_default_language_info[$code] = array('name' => $this->all_languages[$code]);
        if (!empty($default)) {
          $full_default_language_info[$code]['default'] = true;
        }
      }
    }
    return $full_default_language_info;
  }

  /**
   * Handle admin_menu action.
   */
  function admin_menu() {
    if (current_user_can('edit_users')) {
      add_options_page(
        __('What Did They Say?!? Settings', 'what-did-they-say'),
        __('What Did They Say?!?', 'what-did-they-say'),
        'manage_options',
        'manage-wdts',
        array(&$this, 'manage_admin')
      );
    }
    
    if (current_user_can('approve_transcriptions')) {
      add_meta_box(
        'manage-transcriptions',
        __('Manage Transcriptions', 'what-did-they-say'),
        array(&$this, 'manage_transcriptions_meta_box'),
        'post',
        'normal',
        'low'
      );
    }
  }

  /**
   * Show the admin page.
   */
  function manage_admin() {
    $options = get_option('what-did-they-say-options');

    $nonce = wp_create_nonce('what-did-they-say');
    include(dirname(__FILE__) . '/admin.inc');
  }

  /**
   * Show the Manage Transcriptions meta box.
   */
  function manage_transcriptions_meta_box() {
    global $post;

    $options = get_option('what-did-they-say-options');

    $transcripts = $this->what_did_they_say->get_transcripts($post->ID);
    $queued_transcriptions = $this->what_did_they_say->get_queued_transcriptions_for_post($post->ID);
    
    $nonce = wp_create_nonce('what-did-they-say');
    include(dirname(__FILE__) . '/meta-box.inc');
  }
}

?>