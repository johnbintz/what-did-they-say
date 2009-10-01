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
    ),
    'load_default_styles' => true,
    'automatic_embedding' => true,
    'search_integration' => true,
    'excerpt_distance' => 30,
    'use_nl2br' => false
  );
  
  var $capabilities = array();
  
  var $language_file;
  var $all_languages = array();
  var $notices = array();

  var $is_ajax = false;

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
    
    $options = get_option('what-did-they-say-options');
    if (!is_array($options)) {
      $this->install();
    }
    
    add_action('admin_menu', array(&$this, 'admin_menu'));
    add_action('admin_notices', array(&$this, 'admin_notices'));

    add_filter('user_has_cap', array(&$this, 'user_has_cap'), 5, 3);
    add_filter('the_media_transcript', array(&$this, 'the_media_transcript'), 10, 2);
    add_filter('the_language_name', array(&$this, 'the_language_name'), 10, 2);
    add_filter('the_matching_transcript_excerpts', array(&$this, 'the_matching_transcript_excerpts'), 10, 3);
    
    add_filter('template_redirect', array(&$this, 'template_redirect'));

    if ($options['search_integration']) {
      add_filter('posts_where', array(&$this, 'posts_where'));
      add_filter('posts_join', array(&$this, 'posts_join'));
    }

    if ($options['automatic_embedding']) {
      add_filter('the_content', array(&$this, 'the_content_automatic_embedding'), 15);
    }

    foreach (get_class_methods($this) as $method) {
      if (strpos($method, "shortcode_") === 0) {
        $shortcode_name = str_replace("_", "-", str_replace("shortcode_", "", $method));
        add_shortcode($shortcode_name, array(&$this, $method));
      }
    }

    add_filter('filter_shortcode_dialog', array(&$this, 'filter_shortcode_dialog'), 10, 4);
    add_filter('filter_shortcode_scene_action', array(&$this, 'filter_shortcode_scene_action'), 10, 2);
    add_filter('filter_shortcode_scene_heading', array(&$this, 'filter_shortcode_scene_heading'), 10, 2);

    if (isset($_REQUEST['wdts'])) {
      if (isset($_REQUEST['wdts']['_nonce'])) {
        if (wp_verify_nonce($_REQUEST['wdts']['_nonce'], 'what-did-they-say')) {
          $this->handle_update($_REQUEST['wdts']);
          
          if ($this->is_ajax) { exit(0); }
        }
      }
    }
  }

  function include_editor_javascript() {
    $nonce = wp_create_nonce('what-did-they-say');
    
    include(dirname(__FILE__) . '/partials/_editor-script.inc');
  }

  /**
   * Attempt to automatically embed transcripts in posts.
   */
  function the_content_automatic_embedding($content) {
    ob_start();
    transcripts_display();
    the_media_transcript_queue_editor();
    return $content . ob_get_clean();
  }

  /**
   * Filter for WP_Query#get_posts to add searching for transcripts.
   */
  function posts_where($where) {
    global $wpdb;
    
    $search = get_query_var('s');
    if (!empty($search)) {
      $query = $wpdb->prepare(" OR ($wpdb->postmeta.meta_key = %s ", 'approved_transcripts_words');
      $search = addslashes_gpc($search);
      $query .= " AND $wpdb->postmeta.meta_value LIKE '%$search%') ";

      $exact = get_query_var('exact');
      $n = !empty($exact) ? '' : '%';

      $where = preg_replace("#(\($wpdb->posts.post_title LIKE '{$n}{$search}{$n}'\))#", '\1' . $query, $where);
    }

    return $where;
  }

  /**
   * Filter for WP_Query#get_posts to add searching for transcripts.
   */
  function posts_join($join) {
    global $wpdb;
    
    $search = get_query_var('s');
    if (!empty($search)) {
      $join .=  " JOIN $wpdb->postmeta ON ($wpdb->posts.ID = $wpdb->postmeta.post_id) ";
    }
    
    return $join;
  }

  /**
   * Action for when a non-admin page is displayed.
   */
  function template_redirect() {
    wp_enqueue_script('toggle-transcript', plugin_dir_url(dirname(__FILE__)) . 'js/toggle-transcript.js', array('prototype'), false, true);
    if (current_user_can('submit_transcriptions')) {
      wp_enqueue_script('scriptaculous-effects');
      wp_enqueue_script('edit-transcripts', plugin_dir_url(dirname(__FILE__)) . 'js/edit-transcripts.js', array('scriptaculous-effects'));
      wp_enqueue_style('wdts-admin', plugin_dir_url(dirname(__FILE__)) . 'css/wdts-admin.css');

      add_action('wp_head', array(&$this, 'include_editor_javascript'));
    }
    
    $options = get_option('what-did-they-say-options');
    if (!empty($options['load_default_styles'])) {
      wp_enqueue_style('wdts-defaults', plugin_dir_url(dirname(__FILE__)) . 'css/wdts-defaults.css');
    }
  }

  /**
   * Dialog short code.
   */
  function shortcode_dialog($atts, $speech) {
    extract(shortcode_atts(array(
      'name' => 'Nobody',
      'direction' => ''
    ), $atts));
    
    return end(apply_filters('filter_shortcode_dialog', $name, $direction, $speech, ""));
  }

  /**
   * Filter for dialog short code.
   */
  function filter_shortcode_dialog($name, $direction, $speech, $content) {
    $content  = '<div class="dialog"><span class="name">' . $name . '</span>';
    if (!empty($direction)) {
      $content .= ' <span class="direction">' . $direction . '</span>';
    }
    $content .= ' <span class="speech">' . $speech . '</span></div>';
    
    return array($name, $direction, $speech, $content);
  }

  /**
   * Scene action short code.
   */
  function shortcode_scene_action($atts, $description) {
    extract(shortcode_atts(array(), $atts));
    
    return end(apply_filters('filter_shortcode_scene_action', $description, ""));
  }

  /**
   * Filter for scene action short code.
   */
  function filter_shortcode_scene_action($description, $content) {
    return array($description, '<div class="scene-action">' . $description . '</div>', );
  }

  /**
   * Scene heading short code.
   */
  function shortcode_scene_heading($atts, $description) {
    extract(shortcode_atts(array(), $atts));
    
    return end(apply_filters('filter_shortcode_scene_heading', $description, ""));
  }

  /**
   * Filter for scene heading short code.
   */
  function filter_shortcode_scene_heading($description, $content) {
    return array($description, '<div class="scene-heading">' . $description . '</div>');
  }

  /**
   * Handle the_media_transcript filter.
   * @param string $transcript The transcription text.
   * @return string The processed transcription text.
   */
  function the_media_transcript($transcript, $content = "") {
    $content = do_shortcode($transcript);
    $options = get_option('what-did-they-say-options');
    if ($options['use_nl2br']) { $content = nl2br($content); }
    return array($transcript, '<div class="transcript">' . $content . '</div>');
  }

  /**
   * Handle the_language_name filter.
   * @param string $language The name of the language.
   * @return string The processed language name.
   */
  function the_language_name($language, $content = "") {
    return array($language, '<h3 class="transcript-language">' . $language . '</h3>');
  }

  /**
   * Handle the_matching_transcript_excerpts.
   */
  function the_matching_transcript_excerpts($transcripts, $search_string = '', $content = "") {
    $options = get_option('what-did-they-say-options');
    ob_start();
    if ($options['search_integration']) {
      if (!empty($search_string)) {
        $language_options = new WDTSLanguageOptions();
        $options = get_option('what-did-they-say-options');

        foreach ($transcripts as $transcript) {
          if (($pos = strpos($transcript['transcript'], $search_string)) !== false) {
            $l = strlen($transcript['transcript']) - 1;
            echo '<div class="transcript-match">';
              echo '<h4>' . sprintf(__("%s transcript excerpt:", 'what-did-they-say'), $language_options->get_language_name($transcript['language'])) . '</h4>';
              echo '<p>';
                $start_ellipsis = $end_ellipsis = true;
                foreach (array(
                  'start' => -1,
                  'end'   => 1
                ) as $variable => $direction) {
                  ${$variable} = $pos + ($options['excerpt_distance'] * $direction);

                  if ($variable == "end") { ${$variable} += strlen($search_string); }

                  if (${$variable} < 0) { ${$variable} = 0; $start_ellipsis = false; }
                  if (${$variable} > $l) { ${$variable} = $l; $end_ellipsis = false; }
                }

                $output = "";
                if ($start_ellipsis) { $output .= "..."; }
                $output .= str_replace($search_string, "<strong>" . $search_string . "</strong>", trim(substr($transcript['transcript'], $start, $end - $start)));
                if ($end_ellipsis) { $output .= "..."; }

                echo $output;
              echo '</p>';
            echo '</div>';
          }
        }
      }
    }
    return array($transcripts, $search_string, ob_get_clean());
  }
  
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
      $role_cascade = array('subscriber', 'contributor', 'author', 'editor', 'administrator');
      $allowed_roles = array();

      for ($i = 0; $i < count($role_cascade); ++$i) {
        if (isset($capabilities[$role_cascade[$i]])) {
          $allowed_roles = array_slice($role_cascade, 0, $i + 1); break;
        }
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
    if (isset($info['module'])) {
      $method_name = "handle_update_" . str_replace("-", "_", $info['module']);
      if (method_exists($this, $method_name)) {
        $result = $this->{$method_name}($info);
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
      $transcript_options = new WDTSTranscriptOptions($info['post_id']);
      
      if ($transcript_options->are_new_transcripts_allowed()) {
        $queued_transcript_manager = new WDTSQueuedTranscript($info['post_id']);
        
        if ($queued_transcript_manager->save_transcript($info)) {
          $updated = __('Transcript added to queue.', 'what-did-they-say');
        } else {
          $updated = __('Transcript not added to queue.', 'what-did-they-say');          
        }
      }
    }
    return $updated;
  }

  function handle_update_update_queued_transcript($info) {
    $this->is_ajax = true;

    if ($this->current_user_can_edit_this_transcript($info['post_id'], $info['key'])) {
      $queued_transcript_manager  = new WDTSQueuedTranscript($info['post_id']);
      $queued_transcript_manager->update_transcript($info);

      $post = get_post($info['post_id']);

      $language_options = new WDTSLanguageOptions();
      $transcript_options = new WDTSTranscriptOptions($post->ID);

      $transcript = $queued_transcript_manager->get_transcript_by_key($info['key']);
      $user = wp_get_current_user();

      include(dirname(__FILE__) . '/partials/_queued-transcript-contents.inc');
      return;
    }
    header('HTTP/1.1 401 Unauthorized');
  }

  /**
   * Handle updates to post transcripts.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   * @return string|false A string if a message is to be displayed, or false if no message.
   */
  function handle_update_manage_post_transcripts($info) {
    if (!is_admin()) { $this->is_ajax = true; }
    
    $updated = false;
    if (current_user_can('approve_transcriptions')) {
      $approved_transcript_manager = new WDTSApprovedTranscript($info['post_id']);

      foreach ($info['transcripts'] as $language => $transcript) {
        $approved_transcript_manager->save_transcript(array(
          'language' => $language,
          'transcript' => $transcript
        ));
      }

      $transcript_options = new WDTSTranscriptOptions($info['post_id']);
      $transcript_options->set_allow_transcripts(isset($info['allow_on_post']));

      $queued_transcript_manager = new WDTSQueuedTranscript($info['post_id']);
      $queued_transcripts = $queued_transcript_manager->get_transcripts();

      if (is_array($queued_transcriptions)) {
        $transcripts_to_delete = array();

        foreach ($queued_transcriptions as $transcription) { $transcripts_to_delete[$transcription->id] = true; }
        if (isset($info['queue'])) {
          foreach ($info['queue'] as $id => $keep) { unset($transcripts_to_delete[$id]); }
        }

        foreach (array_keys($transcripts_to_delete) as $id) { $queued_transcripts->delete_transcript($id); }
      }

      if (is_admin()) {
        $updated = __('Transcripts updated.', 'what-did-they-say');
      } else {
        return;
      }
    }
    if (is_admin()) {
      return $updated;
    } else {
      header('HTTP/1.1 401 Unauthorized');
    }
  }

  /**
   * Handle transcript approval.
   */
  function handle_update_approve_transcript($info) {
    $this->is_ajax = true;

    if (current_user_can('approve_transcriptions')) {
      $approved_transcript_manager = new WDTSApprovedTranscript($info['post_id']);
      $queued_transcript_manager  = new WDTSQueuedTranscript($info['post_id']);
      
      if (($transcript = $queued_transcript_manager->delete_transcript_by_key($info['key'])) !== false) {
        $approved_transcript_manager->save_transcript($transcript);
        return;
      }
      header('HTTP/1.1 500 Internal Server Error');
      return;
    }
    header('HTTP/1.1 401 Unauthorized');
  }

  function current_user_can_edit_this_transcript($post_id, $transcript_id) {
    if (current_user_can('approve_transcriptions')) {
      $ok = true;
    } else {
      $ok = false;
      if (current_user_can('submit_transcriptions')) {
        $queued_transcript_manager  = new WDTSQueuedTranscript($post_id);
        $user = wp_get_current_user();

        $transcripts = $queued_transcript_manager->get_transcripts_for_user($user->ID);
        foreach ($transcripts as $transcript) {
          if ($transcript['key'] == $transcript_id) {
            $ok = true; break;
          }
        }
      }
    }
    return $ok;
  }

  /**
   * Handle transcript deletion.
   */
  function handle_update_delete_transcript($info) {
    $this->is_ajax = true;

    if ($this->current_user_can_edit_this_transcript($info['post_id'], $info['key'])) {
      $queued_transcript_manager  = new WDTSQueuedTranscript($info['post_id']);
      
      if (($transcript = $queued_transcript_manager->delete_transcript_by_key($info['key'])) !== false) {
        return;
      }
      header('HTTP/1.1 500 Internal Server Error');
      return;
    }
    header('HTTP/1.1 401 Unauthorized');    
  }

  /**
   * Handle updating default styles.
   */
  function handle_update_styles($info) {
    $updated = false;
    if (current_user_can('edit_themes')) {
      $options = get_option('what-did-they-say-options');

      $options['load_default_styles'] = isset($info['default_styles']);
      $options['use_nl2br'] = isset($info['use_nl2br']);
      $options['excerpt_distance'] = !empty($info['excerpt_distance']) ? $info['excerpt_distance'] : 30;
      
      update_option('what-did-they-say-options', $options);
      $updated = __('Default styles option updated.', 'what-did-they-say');
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
          } else {
            $updated = sprintf(__('Language not deleted!', 'what-did-they-say'));
          }
          break;
        case "add":
          $this->read_language_file();
          if (isset($this->all_languages[$info['code']])) {
            if ($language_options->add_language($info['code'], array('name' => $this->all_languages[$info['code']]))) {
              $updated = sprintf(__('%s added.', 'what-did-they-say'), $this->all_languages[$info['code']]);
            } else {
              $updated = sprintf(__('Language not added!', 'what-did-they-say'));
            }
          }
          break;
        case "default":
          if ($language_options->set_default_language($info['code'])) {
            $updated = sprintf(__('%s set as default.', 'what-did-they-say'), $language_options->get_language_name($info['code']));
          } else {
            $updated = sprintf(__('Language not set as default!', 'what-did-they-say'));
          }
          break;
        case "rename":
          $original_language_name = $language_options->get_language_name($info['code']);
          if ($language_options->rename_language($info['code'], $info['name'])) {
            $updated = sprintf(__('%1$s renamed to %2$s.', 'what-did-they-say'), $original_language_name, $info['name']);
          } else {
            $updated = sprintf(__('Language not renamed!', 'what-did-they-say'));
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
      if (isset($info['capabilities'])) {
        foreach (array_keys($this->default_options['capabilities']) as $capability) {
          if (isset($info['capabilities'][$capability])) {
            $options['capabilities'][$capability] = $info['capabilities'][$capability];
          }
        }
        $updated = __('User capabilities updated.', 'what-did-they-say');
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
      delete_option('what-did-they-say-options');
      $this->install();
      $updated = __('<strong>What Did They Say?!?</strong> options reset.', 'what-did-they-say');
    }
    return $updated;
  }

  /**
   * Handle resettings what-did-they-say-options.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   * @return string|false A string if a message is to be displayed, or false if no message.
   */
  function handle_update_core_features($info) {
    $updated = false;
    if (current_user_can('manage_options')) {
      $options = get_option('what-did-they-say-options');
      foreach (array('automatic_embedding', 'search_integration') as $field) {
        $options[$field] = isset($info[$field]);
      }
      update_option('what-did-they-say-options', $options);

      $updated = __('<strong>What Did They Say?!?</strong> core options changed.', 'what-did-they-say');
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
        list($code, $date_added, $name) = explode("\t", $language);
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
    global $pagenow, $plugin_page;

    if (current_user_can('submit_transcriptions')) {
      add_options_page(
        __('What Did They Say?!?', 'what-did-they-say'),
        __('What Did They Say?!?', 'what-did-they-say'),
        'submit_transcriptions',
        'manage-wdts',
        array(&$this, 'manage_admin')
      );
      
      if ($plugin_page == "manage-wdts") {
        $this->read_language_file();
        wp_enqueue_style('wdts-admin', plugin_dir_url(dirname(__FILE__)) . 'css/wdts-admin.css');
        wp_enqueue_style('wdts-defaults', plugin_dir_url(dirname(__FILE__)) . 'css/wdts-defaults.css');

        $this->plugin_data = get_plugin_data($this->_parent_file);
      }
  
      wp_enqueue_script('scriptaculous-effects');

      add_action('wp_head', array(&$this, 'include_editor_javascript'));
    }

    if (current_user_can('approve_transcriptions')) {
      if (strpos($pagenow, "post") === 0) {
        add_meta_box(
          'manage-transcriptions',
          __('Manage Transcripts', 'what-did-they-say'),
          array(&$this, 'manage_transcriptions_meta_box'),
          'post',
          'normal',
          'low'
        );

        wp_enqueue_style('wdts-admin', plugin_dir_url(dirname(__FILE__)) . 'css/wdts-admin.css');
        wp_enqueue_style('wdts-defaults', plugin_dir_url(dirname(__FILE__)) . 'css/wdts-defaults.css');
        wp_enqueue_script('scriptaculous-effects');
        wp_enqueue_script('edit-transcripts', plugin_dir_url(dirname(__FILE__)) . 'js/edit-transcripts.js', array('scriptaculous-effects'));
      }
    }
  }

  /**
   * Show the admin page.
   */
  function manage_admin() {
    global $wpdb;
    
    $options = get_option('what-did-they-say-options');

    $nonce = wp_create_nonce('what-did-they-say');

    $suggested_amount = 20 + ((int)$wpdb->get_var($wpdb->prepare("SELECT count($wpdb->postmeta.meta_key) FROM $wpdb->postmeta WHERE $wpdb->postmeta.meta_key = 'approved_transcripts'")) * 0.1);
    
    include('partials/admin.inc');
  }

  /**
   * Show the Manage Transcriptions meta box.
   */
  function manage_transcriptions_meta_box() {
    global $post;

    $options = get_option('what-did-they-say-options');

    foreach (array('Approved', 'Queued') as $name) {
      $var_name = strtolower($name);
      $class_name = "WDTS${name}Transcript";
      ${"${var_name}_transcript_manager"} = new $class_name($post->ID);
      ${"${var_name}_transcripts"} = ${"${var_name}_transcript_manager"}->get_transcripts();
    }

    $transcript_options = new WDTSTranscriptOptions($post->ID);    

    $language_options = new WDTSLanguageOptions();

    $nonce = wp_create_nonce('what-did-they-say');
    include('partials/_editor-script.inc');
    include('partials/meta-box.inc');
  }
}

?>