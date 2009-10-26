<?php
/*
Plugin Name: What Did They Say?!?
Plugin URI: http://www.coswellproductions.com/wordpress/wordpress-plugins/
Description: Manage and display text transcriptions of comics, videos, or other media.
Version: 0.9.3
Author: John Bintz
Author URI: http://www.coswellproductions.com/wordpress/
Text Domain: what-did-they-say

Copyright 2009  John Bintz  (email : john@coswellproductions.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

foreach (glob(dirname(__FILE__) . '/classes/*.inc') as $__file) { require_once($__file); }

$__what_did_they_say_admin = new WhatDidTheySayAdmin(&$__what_did_they_say);
$__what_did_they_say_admin->_parent_file = __FILE__;

add_action('init', array(&$__what_did_they_say_admin, 'init'));

// template tags
// please, if you use any of these, wrap them in function_exists() so your site doesn't
// blow up if you disable the plugin! Example:
//
// if (function_exists('the_media_transcript')) { the_media_transcript(); }
//

/**
 * Get the transcript for the current post.
 * @param string $language The language code to use. If not specificed, use the default language.
 * @return string|false The transcript in the requested language for the specified post, or false if no transcript found.
 */
function get_the_media_transcript($language = null) {
  global $post;

  if (!empty($post)) {
    $language_options = new WDTSLanguageOptions();
    if (is_null($language)) { $language = $language_options->get_default_language(); }

    $transcript = false;
    $approved_transcripts = new WDTSApprovedTranscript($post->ID);
    $transcripts = $approved_transcripts->get_transcripts();

    if (!empty($transcripts)) {
      if (isset($transcripts[$language])) { $transcript = $transcripts[$language]; }
    }
    return $transcript;
  }
  return '';
}

/**
 * Show the transcript for the current post in the requested_language.
 * @param string $language The language code to use. If not specificed, use the default language.
 */
function the_media_transcript($language = null) {
  echo end(apply_filters('the_media_transcript', get_the_media_transcript($language)));
}

/**
 * Get the excerpt of all transcripts that match the provided search string.
 */
function get_the_matching_transcripts($search_string = '') {
  global $post;

  if (empty($search_string)) { $search_string = get_query_var('s'); }

  $approved_transcripts = new WDTSApprovedTranscript($post->ID);
  $transcripts = $approved_transcripts->get_transcripts();

  $matching_transcripts = array();
  if (!empty($transcripts)) {
    foreach ($transcripts as $transcript) {
      if (strpos($transcript['transcript'], $search_string) !== false) { $matching_transcripts[] = $transcript; }
    }
  }

  return $matching_transcripts;
}

function the_matching_transcript_excerpts($search_string = '') {
  if (empty($search_string)) { $search_string = get_query_var('s'); }

  echo end(apply_filters('the_matching_transcript_excerpts', get_the_matching_transcripts($search_string), $search_string));
}

/**
 * Get the name of the language specified by the provided language code.
 * @param string $language The language code to use. If not specificed, use the default language.
 * @return string The name of the requested language.
 */
function get_the_language_name($language = null) {
  $language_options = new WDTSLanguageOptions();

  if (is_null($language)) { $language = $language_options->get_default_language(); }  
  return $language_options->get_language_name($language);
}

/**
 * Show the name of the language specified by the provided language code.
 * @param string $language The language code to use. If not specificed, use the default language.
 */
function the_language_name($language = null) {
  echo end(apply_filters('the_language_name', get_the_language_name($language)));
}

/**
 * Display all transcripts for a post, with a dropdown selector for people to select other languages.
 * @param string $dropdown_message If set, the text that appears to the left of the language dropdown.
 * @param string $single_language_message If set, the text that appears when only one transcript exists for this post.
 */
function transcripts_display($language_format = null, $show_transcripts_string = null, $hide_transcripts_string = null) {
  global $post;
  
  $output = array();
  
  $transcripts = array();

  $approved_transcripts = new WDTSApprovedTranscript($post->ID);
  $post_transcripts = $approved_transcripts->get_transcripts();

  if (!empty($post_transcripts)) {
    foreach ($post_transcripts as $transcript) {
      $transcript_text = trim($transcript['transcript']);
      if (!empty($transcript_text)) {
        $transcripts[$transcript['language']] = $transcript;
      }
    }

    $language_options = new WDTSLanguageOptions();
    $options = get_option('what-did-they-say-options');

    if (count($transcripts) > 0) {
      $default_language = $language_options->get_default_language();

      $do_hide = false;
      if (is_home() || is_single()) {
        foreach ($options['hide_transcript'] as $type => $do_hide) {
          if ($do_hide && call_user_func("is_${type}")) {
            $do_hide = true; break;
          }
        }
      }

      $output[] = '<div class="wdts-transcript-container' . ($do_hide ? " wdts-start-hidden" : "") . '">';
        $output[] = apply_filters('the_transcript_opener', '');

        $output[] = '<div class="wdts-transcript-bundle' . ($do_hide ? ' wdts-hide-transcript' : '') . '">';

        foreach ($transcripts as $code => $transcript) {
          $transcript_text = end(apply_filters('the_media_transcript', $transcript, ''));

          $output[] = end(apply_filters('the_transcript_language_name', $language_format, $code, ''));

          $output[] = '<div class="transcript-holder ' . $code . '">' . $transcript_text . '</div>';
        }
        $output[] = '</div>';
      $output[] = '</div>';
    }
  }

  echo apply_filters('transcripts_display', implode("\n", $output));
}

/**
 * If you're allowing users to submit transcripts to the post transcript queue, use this tag in your Loop.
 */
function the_media_transcript_queue_editor() {
  global $post, $wpdb;

  if (current_user_can('submit_transcriptions')) {
    $queued_transcript_object = new WDTSQueuedTranscript($post->ID);

    $language_options = new WDTSLanguageOptions();

    $transcript_options = new WDTSTranscriptOptions($post->ID);    

    $options = get_option('what-did-they-say-options');

    $users = $wpdb->get_results("SELECT ID, user_login from $wpdb->users ORDER BY user_login");

    foreach (array('Approved', 'Queued') as $name) {
      $var_name = strtolower($name);
      $class_name = "WDTS${name}Transcript";
      ${"${var_name}_transcript_manager"} = new $class_name($post->ID);
      ${"${var_name}_transcripts"} = ${"${var_name}_transcript_manager"}->get_transcripts();
    }
    
    $nonce = wp_create_nonce('what-did-they-say');
    $new_transcript_id = md5(rand());

    $show_editor = false;
    if (current_user_can('submit_transcriptions')) {
      if (current_user_can('approve_transcriptions')) {
        $show_editor = true;
      } else {
        $show_editor = $transcript_options->are_new_transcripts_allowed();
      }
    }
    
    if ($show_editor) {
      ?>
      <div style="display:none">
        <span id="wdts-opener-<?php echo $id = md5(rand()) ?>">[
          <a href="#"><?php _e('Edit/Add Transcripts', 'what-did-they-say') ?></a>
          <?php if (current_user_can('approve_transcriptions')) {
            if (($queued_count = $queued_transcript_object->count()) > 0) { ?>
              (<strong><?php printf(__('%d queued', 'what-did-they-say'), $queued_count) ?></strong>)
            <?php } 
          } ?>
        ]</span>
      </div>
      <noscript>
        <p>JavaScript is required to edit transcripts.</p>
      </noscript>
      <div id="wdts-<?php echo $id ?>" style="display:none">
        <?php include(dirname(__FILE__) . '/classes/partials/meta-box.inc') ?>
      </div>
      <script type="text/javascript">
        $($('wdts-opener-<?php echo $id ?>').parentNode).show();

        $('wdts-opener-<?php echo $id ?>').select('a').pop().observe('click', function(e) {
          Event.stop(e);

          var target = $('wdts-<?php echo $id ?>');
          if (target.visible()) {
            new Effect.BlindUp(target, { duration: 0.25 });
          } else {
            new Effect.BlindDown(target, { duration: 0.25 });
          }
        });

        <?php
          if (isset($_SESSION['what-did-they-say'])) {
            if (isset($_SESSION['what-did-they-say']['post_id'])) {
              if ($post->ID == $_SESSION['what-did-they-say']['post_id']) { ?>
                $('wdts-<?php echo $id ?>').show();
                $('wdts-<?php echo $id ?>').scrollIntoView();
              <?php }
            }
          }
        ?>
      </script>
    <?php }
  }
}

function wdts_header_wrapper($text) {
  if (is_admin()) {
    echo '<p><strong>' . $text . '</strong></p>';
  } else {
    echo '<h3 class="wdts">' . $text . '</h3>';
  }
}

?>
