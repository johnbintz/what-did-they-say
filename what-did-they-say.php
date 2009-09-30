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

foreach (glob(dirname(__FILE__) . '/classes/*.inc') as $file) { require_once($file); }

$what_did_they_say_admin = new WhatDidTheySayAdmin(&$what_did_they_say);
$what_did_they_say_admin->_parent_file = __FILE__;

add_action('init', array(&$what_did_they_say_admin, 'init'));

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
function transcripts_display($dropdown_message = null, $single_language_message = null) {
  global $post;
  
  if (is_null($dropdown_message)) { $dropdown_message = __('Select a language:', 'what-did-they-say'); }
  if (is_null($single_language_message)) { $single_language_message = __('%s transcript:', 'what-did-they-say'); }
  
  $output = array();
  
  $transcripts = array();

  $approved_transcripts = new WDTSApprovedTranscript($post->ID);
  $post_transcripts = $approved_transcripts->get_transcripts();

  if (!empty($post_transcripts)) {
    foreach ($post_transcripts as $transcript) {
      extract($transcript, EXTR_PREFIX_ALL, "transcript");
      $transcript_transcript = trim($transcript_transcript);
      if (!empty($transcript_transcript)) {
        $transcripts[$transcript_language] = $transcript_transcript;
      }
    }

    $language_options = new WDTSLanguageOptions();

    if (count($transcripts) > 0) {
      $default_language = $language_options->get_default_language();

      $output[] = '<div class="transcript-bundle">';
      
      if (count($transcripts) == 1) {
        list($code, $transcript) = each($transcripts);
        $output[] = end(apply_filters('the_language_name', get_the_language_name($code)));
        $output[] = end(apply_filters('the_media_transcript', $transcript));
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
          $language_name = end(apply_filters('the_language_name', get_the_language_name($code)));
          $transcript    = end(apply_filters('the_media_transcript', $transcript));

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
  global $post;

  if (current_user_can('submit_transcriptions')) {
    $queued_transcripts_for_user = false;

    $queued_transcripts = new WDTSQueuedTranscript($post->ID);

    $user = wp_get_current_user();
    if (!empty($user)) {
      $queued_transcripts_for_user = $queued_transcripts->get_transcripts_for_user($user->ID);
    }

    $language_options = new WDTSLanguageOptions();

    $transcript_options = new WDTSTranscriptOptions($post->ID);    

    $options = get_option('what-did-they-say-options');

    foreach (array('Approved', 'Queued') as $name) {
      $var_name = strtolower($name);
      $class_name = "WDTS${name}Transcript";
      ${"${var_name}_transcript_manager"} = new $class_name($post->ID);
      ${"${var_name}_transcripts"} = ${"${var_name}_transcript_manager"}->get_transcripts();
    }
    
    $nonce = wp_create_nonce('what-did-they-say');

    ?>
    <?php if (current_user_can('approve_transcriptions')) { ?>
      <h3 class="wdts"><?php _e('Manage Transcripts:', 'what-did-they-say') ?></h3>
      <form method="post" class="transcript-editor">
        <?php include(dirname(__FILE__) . '/classes/partials/meta-box.inc') ?>
        <input type="submit" value="Modify Transcript" />
      </form>
    <?php } ?>
		<?php if ((count($queued_transcripts_for_user) > 0) && !current_user_can('approve_transcriptions')) { ?>
      <h3 style="margin-top: 0.5em"><?php _e('Your Submitted Transcripts:', 'what-did-they-say') ?></h3>
      <?php
        foreach ($queued_transcripts_for_user as $transcript) {
          include(dirname(__FILE__) . '/classes/partials/_display-queued-transcript.inc');
        }
      ?>
		<?php } ?>
    <?php if (current_user_can('submit_transcriptions')) { ?>
      <?php if ($transcript_options->are_new_transcripts_allowed()) { ?>
        <h3 class="wdts"><?php _e('Submit a new transcript:', 'what-did-they-say') ?></h3>
        <form method="post" id="<?php echo $id = "wdts-" . md5(rand()) ?>">
          <input type="hidden" name="wdts[_nonce]" value="<?php echo wp_create_nonce('what-did-they-say') ?>" />
          <input type="hidden" name="wdts[module]" value="queue-transcript" />
          <input type="hidden" name="wdts[post_id]" value="<?php echo $post->ID ?>" />
          
          <div class="wdts-transcript-editor">
            <label>
              <?php _e('Language:', 'what-did-they-say') ?>
              <select name="wdts[language]">
                <?php foreach ($language_options->get_languages() as $code => $info) { ?>
                  <option value="<?php echo $code ?>"><?php echo $info['name'] ?></option>
                <?php } ?>
              </select>
            </label>
            
            <div class="wdts-button-holder"></div>

            <textarea style="height: 200px; width: 99%" name="wdts[transcript]"></textarea>
            <input type="submit" value="<?php _e('Submit For Approval', 'what-did-they-say') ?>" />
          </div>
        </form>
        <script type="text/javascript">WhatDidTheySay.setup_transcript_editor('<?php echo $id ?>')</script>
      <?php } ?>
    <?php } ?>
  <?php }
}

?>