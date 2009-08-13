<?php

class WhatDidTheySayAdmin {
  var $default_languages = array(
    array('name' => 'English', 'default' => true),
    'French',
    'Spanish',
    'Italian',
    'German'
  );
  
  function WhatDidTheySayAdmin() {
    
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
    $languages = get_option('what-did-they-say-languages');
    
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