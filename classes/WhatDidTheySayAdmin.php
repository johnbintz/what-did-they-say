<?php

class WhatDidTheySayAdmin {
  var $default_languages = array(
    array('code' => 'en', 'default' => true),
    'fr',
    'es',
    'it',
    'de'
  );
  
  var $language_file;
  var $all_languages = array();
  
  function WhatDidTheySayAdmin() {
    $this->language_file = dirname(__FILE__) . '/../data/lsr-language.txt';
  }
  
  function init($what_did_they_say) {
    $this->what_did_they_say = $what_did_they_say;
    
    add_action('admin_menu', array(&$this, 'admin_menu'));
    
    if (isset($_POST['wdts'])) {
      if (isset($_POST['wdts']['_nonce'])) {
        if (wp_verify_nonce('what-did-they-say', $_POST['wdts']['_nonce'])) {
          $this->handle_update($_POST['wdts']); 
        }
      } 
    }
  }

  function _update_options($which, $value) {
    $options = get_option('what-did-they-say-options');
    $options[$which] = $value;
    update_option('what-did-they-say-options', $options);
  }

  function handle_update_languages($language_info) {
    $languages = array();
    foreach ($language_info as $code => $info) {
      if (isset($this->all_languages[$code])) {
        $language = $code;
        if (isset($info['default'])) { $language = array('code' => $code, 'default' => true); }
        $languages[] = $language;
      }
    }
    $this->_update_options('languages', $languages);
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
    $languages = get_option('what-did-they-say-languages');
    if (empty($languages)) {
      update_option('what-did-they-say-languages', $this->default_languages); 
    } 
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
    
    include(dirname(__FILE__) . '/admin.inc');
  }
  
  function manage_transcriptions_meta_box() {
    global $post;
    
    var_dump($post->ID);
  }
  
  function handle_update($info) {
    
  }
}

?>