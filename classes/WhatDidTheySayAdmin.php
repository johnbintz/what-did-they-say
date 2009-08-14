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
    'users' => array()
  );
  
  var $language_file;
  var $all_languages = array();
  var $notices = array();
  
  function WhatDidTheySayAdmin() {
    $this->language_file = dirname(__FILE__) . '/../data/lsr-language.txt';
  }
  
  function init($what_did_they_say) {
    $this->what_did_they_say = $what_did_they_say;
    
    add_action('admin_menu', array(&$this, 'admin_menu'));
    add_action('admin_notices', array(&$this, 'admin_notices'));
    wp_enqueue_script('prototype');
    
    if (isset($_REQUEST['wdts'])) {
      if (isset($_REQUEST['wdts']['_nonce'])) {
        if (wp_verify_nonce($_REQUEST['wdts']['_nonce'], 'what-did-they-say')) {
    
          $this->handle_update($_REQUEST['wdts']);
        }
      } 
    }

    $this->read_language_file();
  }

  function _update_options($which, $value) {
    $options = get_option('what-did-they-say-options');
    $options[$which] = $value;
    update_option('what-did-they-say-options', $options);
  }

  function admin_notices() {
    if (!empty($this->notices)) {
      echo '<div class="updated fade">';
        echo implode("<br />", $this->notices);
      echo '</div>'; 
    }
  }

  function handle_update($info) {
    $result = $this->handle_update_languages($info); 
    if (!empty($result)) {
      $this->notices[] = $result;
    }
  }

  function handle_update_languages($language_info) {
    $options = get_option('what-did-they-say-options');
    $updated = false;
    switch ($language_info['action']) {
      case "delete":
        $updated = sprintf(__('%s deleted.', 'what-did-they-say'), $options['languages'][$language_info['code']]['name']);
        unset($options['languages'][$language_info['code']]);
        break;
      case "add":
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
    update_option('what-did-they-say-options', $options);
    return $updated;
  }
  
  function handle_update_allowed_users($users) {
    $allowed_users = array();
    foreach ($users as $user) {
      if (is_numeric($user)) {
        $user_info = get_userdata($user);
        if (!empty($user_info)) {
          $allowed_users[] = $user;
        }
      }
    }

    $this->_update_options('allowed_users', $allowed_users);
  }

  function handle_update_options($requested_options) {
    $updated_options = array(
      'only_allowed_users' => false
    );

    foreach ($requested_options as $option => $value) {
      switch ($option) {
        case 'only_allowed_users':
          $updated_options['only_allowed_users'] = true;
          break;
      }
    }

    $options = array_merge(get_option('what-did-they-say-options'), $updated_options);
    update_option('what-did-they-say-options', $options);
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
    add_submenu_page(
      'edit-comments.php',
      __('Manage Transcriptions', 'what-did-they-say'),
      __('Transcripts', 'what-did-they-say'),
      'edit_posts',
      'manage-transcriptions-wdts',
      array(&$this, 'manage_transcriptions_admin')
    );
    
    if (current_user_can('edit_posts')) {
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
  
  function manage_transcriptions_admin() {
    $options = get_option('what-did-they-say-options');

    $language_map_pairs = array();
    $basic_languages = $all_languages = array();
    foreach ($this->all_languages as $code => $name) {
      $name = addslashes($name);
      $language_map_pairs[] = "'${code}': '${name}'";
      $all_languages[] = "'$code'";
      if (strlen($code) == 2) {
        $basic_languages[] = "'$code'";
      }
    }

    $nonce = wp_create_nonce('what-did-they-say');
    
    $url = 'edit-comments.php?page=manage-transcriptions-wdts';
    
    $nonce_url = add_query_arg('wdts[_nonce]', $nonce, $url);
    
    include(dirname(__FILE__) . '/admin.inc');
  }
  
  function manage_transcriptions_meta_box() {
    global $post;
    
    var_dump($post->ID);
  }
}

?>