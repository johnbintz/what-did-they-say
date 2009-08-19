<?php

class WhatDidTheySayAdmin {
  var $default_options = array(
    'languages' => array(
      array('code' => 'en', 'default' => true),
      'fr',
      'es',
      'it',
      'de'
    ),
    'only_allowed_users' => false,
    'users' => array(),
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
  
  function WhatDidTheySayAdmin($what_did_they_say = null) {
    $this->what_did_they_say = $what_did_they_say;
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

  function the_media_transcript($transcript) {
    return '<div class="transcript">' . $transcript . '</div>'; 
  }
  
  function the_language_name($language) {
    return '<h3>' . $language . '</h3>'; 
  }
  
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
   * user_has_cap filter.
   */
  function user_has_cap($capabilities, $requested_capabilities, $capability_name) {
    $options = get_option('what-did-they-say-options');

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

    return $capabilities;
  }

  /**
   * Show admin notices.
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
   */
  function handle_update($info) {
    foreach (get_class_methods($this) as $method) {
      if (strpos($method, "handle_update_") === 0) {
        $result = $this->{$method}($info);
        if (!empty($result)) { $this->notices[] = $result; }
      } 
    }
  }
  
  function handle_update_queue_transcript($queue_transcript_info) {
    $updated = false;
    if (current_user_can('submit_transcriptions')) {
      switch ($queue_transcript_info['action']) {
        case 'submit_queued_transcript':
          $result = $this->what_did_they_say->add_queued_transcription_to_post($queue_transcript_info['post_id'], $queue_transcript_info);
          if ($result) {
            $updated = __('Transcript added to queue.', 'what-did-they-say'); 
          }
      }
    } 
    return $updated;
  }

  function handle_update_post_transcripts($post_transcript_info) {
    $updated = false;
    if (current_user_can('approve_transcriptions')) {
      $options = get_option('what-did-they-say-options');
  
      switch ($post_transcript_info['action']) {
        case "manage_post_transcripts":
          foreach ($post_transcript_info['transcripts'] as $language => $transcript) {
            $this->what_did_they_say->save_transcript($post_transcript_info['post_id'], $language, $transcript);
          }
          $updated = __('Transcripts updated', 'what-did-they-say');
          break;
      }
    }
    return $updated;
  }

  function handle_update_languages($language_info) {
    $updated = false;
    if (current_user_can('change_languages')) {
      $options = get_option('what-did-they-say-options');
      switch ($language_info['action']) {
        case "delete":
          $updated = sprintf(__('%s deleted.', 'what-did-they-say'), $options['languages'][$language_info['code']]['name']);
          unset($options['languages'][$language_info['code']]);
          break;
        case "add":
          $this->read_language_file();
          if (isset($this->all_languages[$language_info['code']])) {
            $options['languages'][$language_info['code']] = array('name' => $this->all_languages[$language_info['code']]);
            $updated = sprintf(__('%s added.', 'what-did-they-say'), $this->all_languages[$language_info['code']]);
          }
          break;
        case "default":
          if (isset($options['languages'][$language_info['code']])) {
            foreach ($options['languages'] as $code => $info) {
              if ($code == $language_info['code']) {
                $options['languages'][$code]['default'] = true;
                $updated = sprintf(__('%s set as default.', 'what-did-they-say'), $info['name']);
              } else {
                unset($options['languages'][$code]['default']);
              }
            }
          }
          break;
        case "rename":
          if (isset($options['languages'][$language_info['code']])) {
            if (!empty($language_info['name'])) {
              $updated = sprintf(__('%1$s renamed to %2$s.', 'what-did-they-say'), $options['languages'][$language_info['code']]['name'], $language_info['name']);
              $options['languages'][$language_info['code']]['name'] = $language_info['name'];
            }
          }
          break;
      }
      if ($updated !== false) {
        ksort($options['languages']);
        update_option('what-did-they-say-options', $options);
      }
      }
    return $updated;
  }
  
  function handle_update_capabilities($capabilities_info) {
    $updated = false;
    if (current_user_can('edit_users')) {
      $options = get_option('what-did-they-say-options');
      switch ($capabilities_info['action']) {
        case "capabilities":
          if (isset($capabilities_info['capabilities'])) {
            foreach (array_keys($this->default_options['capabilities']) as $capability) {
              if (isset($capabilities_info['capabilities'][$capability])) {
                $options['capabilities'][$capability] = $capabilities_info['capabilities'][$capability];
              }
            }
            $updated = __('User capabilities updated', 'what-did-they-say');
          }
          break;
      }
      if ($updated !== false) {
        update_option('what-did-they-say-options', $options);
      }
    }
    return $updated;
  }

  function read_language_file() {
    if (file_exists($this->language_file)) {
      foreach (file($this->language_file, FILE_IGNORE_NEW_LINES) as $language) {
        list($code, $date_added, $name, $additional) = explode("\t", $language);
        $this->all_languages[$code] = $name;
      } 
    }
    return $this->all_languages;
  }

  function install() {
    $this->read_language_file();
    $options = get_option('what-did-they-say-options');
    if (empty($options)) {
      $this->default_options['languages'] = $this->build_default_languages();
      update_option('what-did-they-say-options', $this->default_options);
    } 
  }

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
  
  function manage_admin() {
    $options = get_option('what-did-they-say-options');
    $nonce = wp_create_nonce('what-did-they-say');
    include(dirname(__FILE__) . '/admin.inc');
  }
  
  function manage_transcriptions_meta_box() {
    global $post;

    $options = get_option('what-did-they-say-options');
    $transcripts = $this->what_did_they_say->get_transcripts($post->ID);
    $nonce = wp_create_nonce('what-did-they-say');
    include(dirname(__FILE__) . '/meta-box.inc');
  }
}

?>