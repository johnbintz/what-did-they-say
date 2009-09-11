<?php
/*
Plugin Name: What Did They Say?!?
Plugin URI: http://www.coswellproductions.com/wordpress/wordpress-plugins/
Description: Manage and display text transcriptions of comics, videos, or other media.
Version: 0.1
Author: John Bintz
Author URI: http://www.coswellproductions.com/wordpress/

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

foreach (glob(dirname(__FILE__) . '/classes/*.php') as $file) { require_once($file); }

$what_did_they_say_admin =& new WhatDidTheySayAdmin(&$what_did_they_say);

add_action('init', array(&$what_did_they_say_admin, 'init'));
register_activation_hook(__FILE__, array(&$what_did_they_say, 'install'));
register_activation_hook(__FILE__, array(&$what_did_they_say_admin, 'install'));

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
  global $post, $what_did_they_say;
  
  if (is_null($language)) { $language = $what_did_they_say->get_default_language(); }
  
  $transcript = false;
  $transcripts = $what_did_they_say->get_transcripts($post->ID);
  if (!empty($transcripts)) {
    if (isset($transcripts[$language])) { $transcript = $transcripts[$language]; }
  }
  return $transcript;
}

/**
 * Show the transcript for the current post in the requested_language.
 * @param string $language The language code to use. If not specificed, use the default language.
 */
function the_media_transcript($language = null) {
  $transcript = apply_filters('the_media_transcript', get_the_media_transcript($language));
  echo $transcript;
}

/**
 * Get the name of the language specified by the provided language code.
 * @param string $language The language code to use. If not specificed, use the default language.
 * @return string The name of the requested language.
 */
function get_the_language_name($language = null) {
  global $what_did_they_say;
  
  if (is_null($language)) { $language = $what_did_they_say->get_default_language(); }  
  return $what_did_they_say->get_language_name($language);
}

/**
 * Show the name of the language specified by the provided language code.
 * @param string $language The language code to use. If not specificed, use the default language.
 */
function the_language_name($language = null) {
  $name = apply_filters('the_language_name', get_the_language_name($language));
  echo $name; 
}

/**
 * Display all transcripts for a post, with a dropdown selector for people to select other languages.
 * @param string $dropdown_message If set, the text that appears to the left of the language dropdown.
 * @param string $single_language_message If set, the text that appears when only one transcript exists for this post.
 */
function transcripts_display($dropdown_message = null, $single_language_message = null) {
  global $post;
  
  if (is_null($dropdown_message)) { $dropdown_message = __('Select a language:', 'what-did-they-say'); }
  if (is_null($single_language_message)) { $single_language_message = __('%s transcript:', 'what-did-they-say'); }
  
  $output = array();
  
  $transcripts = array();

  $approved_transcripts = new WDTSApprovedTranscript($post->ID);
  $post_transcripts = $approved_transcripts->get_transcripts();

  if (!empty($post_transcripts)) {
    foreach ($post_transcripts as $code => $transcript) {
      if (substr($code, 0, 1) != "_") {
        $transcript = trim($transcript);
        if (!empty($transcript)) {
          $transcripts[$code] = $transcript;
        }
      }
    }

    if (count($transcripts) > 0) {
      $default_language = $what_did_they_say->get_default_language();

      $output[] = '<div class="transcript-bundle">';
      if (count($transcripts) == 1) {
        list($code, $transcript) = each($transcripts);
        $output[] = apply_filters('the_language_name', get_the_language_name($code));
        $output[] = apply_filters('the_media_transcript', $transcript);
      } else {
        $output[] = $dropdown_message;
        $output[] = '<select>';
          foreach($transcripts as $code => $transcript) {
            $output[] = '<option value="' . $code . '"' . (($code == $default_language) ? ' selected="selected"' : '') . '>'
                      . get_the_language_name($code)
                      . '</option>';
          }
        $output[] = '</select>';
        foreach ($transcripts as $code => $transcript) {
          $language_name = apply_filters('the_language_name', get_the_language_name($code));
          $transcript    = apply_filters('the_media_transcript', $transcript);

          $output[] = '<div '
                    . (($code == $default_language) ? 'style="display:none"' : '')
                    . ' class="transcript-holder ' . $code . '">' . $language_name . $transcript . '</div>';
        }
      }
      $output[] = '</div>';
    }
  }

  echo apply_filters('transcripts_display', implode("\n", $output));
}

/**
 * If you're allowing users to submit transcripts to the post transcript queue, use this tag in your Loop.
 */
function the_media_transcript_queue_editor() {
  global $post, $what_did_they_say;

  if (current_user_can('submit_transcriptions')) {
    $queued_transcripts_for_user = false;

    $queued_transcripts = new WDTSQueuedTranscript($post->ID);

    $user = wp_get_current_user();
    if (!empty($user)) {
      $queued_transcripts_for_user = $queued_transcripts->get_transcripts_for_user($user->ID);
    }

    $transcript_options = new WDTSTranscriptOptions($post->ID);

    if (is_array($queued_transcripts_for_user)) {
      if (count($queued_transcripts_for_user) > 0) { ?>
        <div class="queued-transcriptions">
          <h3><?php _e('Your queued transcriptions', 'what-did-they-say') ?></h3>
          <?php foreach ($queued_transcripts_for_user as $transcript) { ?>
            <h4><?php echo $what_did_they_say->get_language_name($transcript->language) ?></h4>
            <div>
              <?php echo $transcript->transcript ?>
            </div>
          <?php } ?>
        </div>
      <?php } ?>
    <?php } ?>
    <?php if ($transcript_options->are_new_transcripts_allowed()) { ?>
      <form method="post" class="transcript-editor">
        <input type="hidden" name="wdts[_nonce]" value="<?php echo wp_create_nonce('what-did-they-say') ?>" />
        <input type="hidden" name="wdts[action]" value="submit_queued_transcript" />
        <input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />
        <h3><?php _e('Submit a new transcription:', 'what-did-they-say') ?></h3>
        <label>
          <?php _e('Language:', 'what-did-they-say') ?>
          <select name="wdts[language]">
            <?php foreach ($what_did_they_say->get_languages() as $code => $info) { ?>
              <option value="<?php echo $code ?>"><?php echo $info['name'] ?></option>
            <?php } ?>
          </select>
        </label><br />
        <label>
          <?php _e('Transcription:', 'what-did-they-say') ?><br />
          <textarea style="height: 200px; width: 90%" name="wdts[transcript]"></textarea>
        </label>
        <input type="submit" value="<?php _e('Submit New Transcription', 'what-did-they-say') ?>" />
      </form>
    <?php } ?>
  <?php }
}

?>