<?php

/**
 * Administrative functions for What Did They Say?!?
 */
class WhatDidTheySayAdmin {
  var $default_options = array(
    'languages' => array(
      array('code' => 'en', 'default' => true),
      'fr', 'es', 'it', 'de'
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
    'use_nl2br' => false,
    'high_insertion_level' => false,
    'hide_transcript' => array(
      'home' => true,
      'single' => false
    ),
    'transcript_effects' => false,
    'allow_html' => false,
    'filters_to_use' => 'simple'
  );
  
  var $capabilities = array();
  
  var $language_file;
  var $all_languages = array();
  var $notices = array();
  var $override_filter_info = false;

  var $is_ajax = false;

  /**
   * Initialize the admin interface.
   * @param WhatDidTheySay $what_did_they_say The WhatDidTheySay object to use for all transcript transactions.
   */
  function WhatDidTheySayAdmin() {
    $this->language_file = dirname(__FILE__) . '/../data/lsr-language.txt';
  }

  // just in case things need to be unit tested...
  function get_filters_dir() { return WP_CONTENT_DIR . '/transcript-filters'; }
  function _get_abspath()    { return ABSPATH; }
  function _glob($pattern)   {
    $result = glob($pattern);
    if ($result === false) { $result = array(); }
    return $result;
  }
  
  /**
   * Initialize the object.
   */
  function init() {
		global $wpdb;
		
    // set up capabilities
    $this->capabilities = array(
      'submit_transcriptions'  => __('Submit transcriptions to a post', 'what-did-they-say'),
      'approve_transcriptions' => __('Approve transcriptions to a post', 'what-did-they-say'),
      'change_languages'      => __('Change the available languages', 'what-did-they-say')
    );

    // does this copy of wdts?!? reference new options?
    $options = get_option('what-did-they-say-options');
    if (!is_array($options)) {
      $this->install();
    } else {
      $any_changes = false;
      foreach ($this->default_options as $key => $value) {
        if (!isset($options[$key])) {
          $any_changes = true; $options[$key] = $value;
        }
      }
      if ($any_changes) { update_option('what-did-they-say-options', $options); }
    }

    // sessions
    if (!session_id()) { session_start(); }

    // pages
    add_action('admin_menu', array(&$this, 'admin_menu'));
    add_action('admin_notices', array(&$this, 'admin_notices'));
    add_action('template_redirect', array(&$this, 'template_redirect'));

    // capabilities
    add_filter('user_has_cap', array(&$this, 'user_has_cap'), 5, 3);

    // display effects
    $filters = new WDTSDisplayFilters();
    $reset_filter_value = true;

    $filter_to_use = false;
    if (is_string($options['filters_to_use'])) { $filter_to_use = $options['filters_to_use']; }
    if (current_user_can('submit_transcriptions')) {
      if (isset($_REQUEST['wdts'])) {
        if (isset($_REQUEST['wdts']['filter_preview'])) {
          $filter_to_use = $_REQUEST['wdts']['filter_preview'];
        }
      }
    }

    if ($filter_to_use) {
      $all_filters = $this->_get_available_override_filters();

      $filter_to_use = preg_replace('#[^a-z0-9_-]#', '', strtolower($filter_to_use));

      if (isset($all_filters[$filter_to_use])) {
        foreach ($this->_glob($all_filters[$filter_to_use] . '/*') as $file) {
          if (preg_match('#\.(php|inc)$#', $file) > 0) { $this->override_filter_info['php'] = $file; }
          if (preg_match('#\.(css)$#', $file) > 0)     { $this->override_filter_info['css'] = $file; }
        }

        if (isset($this->override_filter_info['php'])) {
          $class_name = preg_replace('#\..*$#', '', basename($this->override_filter_info['php']));
          require_once($this->override_filter_info['php']);

          if (class_exists($class_name)) {
            $filters = new $class_name();
            $reset_filter_value = false;
          }
        }
      }
    }

    // short codes
    foreach (array(
      array('the_media_transcript', 2),
      array('the_language_name', 2),
      array('the_matching_transcript_excerpts', 3),
      array('the_transcript_language_name', 3),
      array('the_transcript_format_string', 1),
      array('the_transcript_opener', 1),
      array('the_transcript_transition_effect', 2),
      array('filter_shortcode_dialog', 4),
      array('filter_shortcode_scene_action', 2),
      array('filter_shortcode_scene_heading', 2)
    ) as $filter_info) {
      list($method, $parameters) = $filter_info;

      if (method_exists($filters, $method)) {
        add_filter($method, array(&$filters, $method), 10, $parameters);
      }
    }

    foreach (get_class_methods($this) as $method) {
      if (strpos($method, "shortcode_") === 0) {
        $shortcode_name = str_replace("_", "-", str_replace("shortcode_", "", $method));
        add_shortcode($shortcode_name, array(&$this, $method));
      }
    }

    // search transcripts, too?
    if ($options['search_integration']) {
      add_filter('posts_where', array(&$this, 'posts_where'));
      add_filter('posts_join', array(&$this, 'posts_join'));
    }

    // try to automatically embed editors?
    if ($options['automatic_embedding']) {
      add_filter('the_content', array(&$this, 'the_content_automatic_embedding'), $options['high_insertion_level'] ? 100 : 15);
    }

    // add extra script bits if this user can edit
    if (current_user_can('submit_transcriptions')) {
      add_action('wp_footer', array(&$this, 'wp_footer'));
    }

		// sidebar widget
	  wp_register_sidebar_widget('what-did-they-say', __('What Did They Say?!? Transcript', 'what-did-they-say'), array(&$this, 'transcript_sidebar_widget'), array('description' => __('Show the transcript for the current post.', 'what-did-they-say')));

    // handle form submission
    if (isset($_REQUEST['wdts'])) {
      if (isset($_REQUEST['wdts']['script'])) {
        $language_options = new WDTSLanguageOptions();
        
        include('partials/_editor-script.inc');
        exit(0);
      }

      if (isset($_REQUEST['wdts']['filter_preview'])) {
        $styles_to_include = array(
          plugin_dir_url(dirname(__FILE__)) . 'css/wdts-defaults.css'
        );

        if (isset($this->override_filter_info['css'])) {
          if (($path = realpath($this->override_filter_info['css'])) !== false) {
            $styles_to_include[] = str_replace(realpath($this->_get_abspath()), '', $path);
          }
        }

        include('partials/_sample-transcript.inc');
        include('partials/_show-sample-transcript.inc');
        exit(0);
      }

      if (isset($_REQUEST['wdts']['_nonce'])) {
        if (wp_verify_nonce($_REQUEST['wdts']['_nonce'], 'what-did-they-say')) {
					if (($_REQUEST['wdts']['post_id'] <= 0) && ($_POST['post_ID'] > 0)) {
						$_REQUEST['wdts']['post_id'] = $_POST['post_ID'];
					}

          $this->handle_update($_REQUEST['wdts']);
          
          if ($this->is_ajax) { exit(0); }
        }
      }
    }

    if (is_admin()) {
      $this->transcript_count = (int)$wpdb->get_var($wpdb->prepare("SELECT count($wpdb->postmeta.meta_key) FROM $wpdb->postmeta WHERE $wpdb->postmeta.meta_key = 'approved_transcripts'"));
    }
	}

	/** Widget **/

	function transcript_sidebar_widget() {
		transcripts_display();
	}

  /** Actions **/

  function wp_footer() {
    $nonce = wp_create_nonce('what-did-they-say');
    
    echo '<script type="text/javascript">';
      echo 'WhatDidTheySay.ajax_url = "' . addslashes($_SERVER['REQUEST_URI']) . '";';
      echo 'WhatDidTheySay.nonce = "' . $nonce . '";';
    echo '</script>';

    if (isset($_SESSION['what-did-they-say'])) {
      if (isset($_SESSION['what-did-they-say']['updated'])) {
        echo '<div class="wdts-updated"><div>' . $_SESSION['what-did-they-say']['updated'] . '</div></div>';
      }
    }

    // clear out our session variable, we only need it briefly
    unset($_SESSION['what-did-they-say']);
  }

  /**
   * Action for when a non-admin page is displayed.
   */
  function template_redirect() {
    wp_enqueue_script('wdts-script', get_bloginfo('url') . '?wdts[script]=true', array('prototype'));
    wp_enqueue_script('toggle-transcript', plugin_dir_url(dirname(__FILE__)) . 'js/toggle-transcript.js', array('prototype', 'wdts-script'), false, true);

    if (current_user_can('submit_transcriptions')) {
      wp_enqueue_script('scriptaculous-effects');
      wp_enqueue_script('edit-transcripts', plugin_dir_url(dirname(__FILE__)) . 'js/edit-transcripts.js', array('scriptaculous-effects', 'wdts-script'));
      wp_enqueue_style('wdts-admin', plugin_dir_url(dirname(__FILE__)) . 'css/wdts-admin.css');
    }

    $options = get_option('what-did-they-say-options');
    if (!empty($options['load_default_styles'])) {
      wp_enqueue_style('wdts-defaults', plugin_dir_url(dirname(__FILE__)) . 'css/wdts-defaults.css');
    }

    if (isset($this->override_filter_info['css'])) {
      if (($path = realpath($this->override_filter_info['css'])) !== false) {
        wp_enqueue_style('wdts-override', str_replace(realpath($this->_get_abspath()), '', $path));
      }
    }

    if ($options['transcript_effects']) {
      wp_enqueue_script('scriptaculous-effects');
    }
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
   * Handle admin_menu action.
   */
  function admin_menu() {
    global $pagenow, $plugin_page;

    wp_enqueue_script('wdts-script', get_bloginfo('url') . '?wdts[script]=true', array('prototype'));

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

				if ($this->transcript_count == 0) {
					$this->notices[] = __('Welcome to <strong>What Did They Say?!?</strong> To get started, read the <strong>Introduction</strong>, and then <strong>How It Works</strong>.', 'what-did-they-say');
				} elseif ($this->transcript_count < 10) {
					$this->notices[] = __('Is <strong>What Did They Say?!?</strong> working out for you? Let John know via email (<a href="mailto:john@coswellproductions.com">john@coswellproductions.com</a>) or Twitter (<a href="http://twitter.com/JohnBintz">@JohnBintz</a>).', 'what-did-they-say');
				}
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
        if (isset($this->override_filter_info['css'])) {
          wp_enqueue_style('wdts-override', str_replace(realpath($this->_get_abspath()), '', realpath($this->override_filter_info['css'])));
        }

        wp_enqueue_script('scriptaculous-effects');
        wp_enqueue_script('edit-transcripts', plugin_dir_url(dirname(__FILE__)) . 'js/edit-transcripts.js', array('scriptaculous-effects', 'wdts-script'));

	      add_action('wp_head', array(&$this, 'include_editor_javascript'));
			}
    }
  }

  /** Display Filters **/

  /**
   * Attempt to automatically embed transcripts in posts.
   */
  function the_content_automatic_embedding($content) {
    ob_start();
    transcripts_display();
    the_media_transcript_queue_editor();
    return $content . ob_get_clean();
  }


  /** Transcript Search Filters **/

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


  /** Short codes **/

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
   * Scene action short code.
   */
  function shortcode_scene_action($atts, $description) {
    extract(shortcode_atts(array(), $atts));
    
    return end(apply_filters('filter_shortcode_scene_action', $description, ""));
  }

  /**
   * Scene heading short code.
   */
  function shortcode_scene_heading($atts, $description) {
    extract(shortcode_atts(array(), $atts));
    
    return end(apply_filters('filter_shortcode_scene_heading', $description, ""));
  }


  /** Capabilities **/

  /**
   * Handle the user_has_cap filter.
   * @param array $capabilities The capabilities the user already has.
   * @param array $requested_capabilities The capabilities requested by current_user_can.
   * @param object $capability_name
   * @return array The list of capabilities this user now has.
   */
  function user_has_cap($capabilities, $requested_capabilities, $args) {
    list($capability_name, $user_id) = $args;
    
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

    $user_capabilities = get_usermeta($user_id, 'transcript_capabilities');

    if (is_array($user_capabilities)) {
      foreach ($requested_capabilities as $requested_capability) {
        if (isset($user_capabilities[$requested_capability])) {
          $capabilities[$requested_capability] = true;
        }
      }
    }

    return $capabilities;
  }

  /**
   * Check if the current user can edit the specified transcript.
   */
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


  /** Handle data updates **/

  /**
   * Handle an update to options.
   * @param array $info The part of the $_POST array for What Did They Say?!?
   */
  function handle_update($info) {
    if (isset($info['module'])) {
      $method_name = "handle_update_" . str_replace("-", "_", $info['module']);
      if (method_exists($this, $method_name)) {
        $info = $this->_clean_child($info);
      
        $result = $this->{$method_name}($info);
        if (!empty($result)) { $this->notices[] = $result; }
      }
    }
  }

  function _clean_child($node) {
    if (is_array($node)) {
      $new_nodes = array();
      foreach ($node as $key => $n) {
        $new_nodes[$key] = $this->_clean_child($n);
      }
      return $new_nodes;
    } else {
      $options = get_option('what-did-they-say-options');

      $node = (string)$node;
      foreach (array('script', 'style', 'link') as $tag) { $node = preg_replace("#<${tag}.*/${tag}>#", '', $node); }
      if (!$options['allow_html']) { $node = strip_tags($node); }
      
      return $node;
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

    if (!is_admin()) {
      $_SESSION['what-did-they-say'] = array(
        'action'  => 'queue-transcript',
        'post_id' => $info['post_id'],
        'updated' => $updated
      );
      header('Location: ' . $_SERVER['HTTP_REFERER']);
      exit(0);
    } else {
      return $updated;
    }
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

      foreach (array('load_default_styles', 'use_nl2br', 'transcript_effects', 'allow_html') as $field) { $options[$field] = isset($info[$field]);  }
      $options['excerpt_distance'] = !empty($info['excerpt_distance']) ? $info['excerpt_distance'] : 30;
      foreach (array_keys($options['hide_transcript']) as $type) {
        $options['hide_transcript'][$type] = isset($info['hide_transcript'][$type]);
      }

      if (isset($info['filters_to_use'])) {
        $options['filters_to_use'] = preg_replace('#[^a-z0-9_-]#', '', strtolower($info['filters_to_use']));
      }

      update_option('what-did-they-say-options', $options);
      $updated = __('Default styles option updated.', 'what-did-they-say');
    }
    return $updated;
  }
    
  /**
   * Handle updating the insertion level.
   */
  function handle_update_high_insertion_level($info) {
    $updated = false;
    if (current_user_can('manage_options')) {
      $options = get_option('what-did-they-say-options');

      $options['high_insertion_level'] = isset($info['high_insertion_level']);

      update_option('what-did-they-say-options', $options);
      $updated = __('Insertion level updated.', 'what-did-they-say');
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
   * Update per-user capabilities.
   */
  function handle_update_user_capabilities($info) {
    global $wpdb;

    $updated = false;
    if (current_user_can('edit_users')) {
      $users = $wpdb->get_results("SELECT ID, user_login from $wpdb->users ORDER BY user_login");

      foreach ((array)$users as $user) {
        $user_capabilities = array();

        foreach (array_keys($this->capabilities) as $key) {
          if (isset($info['user_capabilities'][$user->ID])) {
            if (isset($info['user_capabilities'][$user->ID][$key])) {
              $user_capabilities[$key] = true;
            }
          }
        }

        if (!empty($user_capabilities)) {
          update_usermeta($user->ID, 'transcript_capabilities', $user_capabilities);
        } else {
          delete_usermeta($user->ID, 'transcript_capabilities');
        }
      }
      
      $updated = __('Per-user permissions updated.', 'what-did-they-say');
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

  /** Language management **/
  
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

  /** What Did They Say?!? administration screens **/

  function _get_available_override_filters() {
    $available_filters = array();
    $search_path = array(
      realpath(dirname(__FILE__) . '/../transcript-filters'),
      $this->get_filters_dir()
    );

    foreach ($search_path as $target) {
      if (is_dir($target)) {
        foreach ($this->_glob($target . '/*') as $dir) {
          if (is_dir($dir)) {
            if (basename($dir) == preg_replace('#[^a-z0-9_-]#', '', strtolower(basename($dir)))) {
              foreach ($this->_glob($dir . '/*') as $file) {
                if (preg_match('#^(.*)\.(inc|php)$#', basename($file), $matches) > 0) {
                  $available_filters[basename($dir)] = $dir;
                }
              }
            }
          }
        }
      }
    }
    
    return $available_filters;
  }

  /**
   * Show the admin page.
   */
  function manage_admin() {
    global $wpdb;
    
    $options = get_option('what-did-they-say-options');

    $nonce = wp_create_nonce('what-did-they-say');
    
    $available_filters = $this->_get_available_override_filters();
    $suggested_amount = 20 + ($this->transcript_count * 0.1);

    $users = $wpdb->get_results("SELECT ID, user_login from $wpdb->users ORDER BY user_login");

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
    $queued_transcripts = new WDTSQueuedTranscript($post->ID);

    $language_options = new WDTSLanguageOptions();

    $nonce = wp_create_nonce('what-did-they-say');
    include('partials/meta-box.inc');
  }
}

?>