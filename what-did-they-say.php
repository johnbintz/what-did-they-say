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

$what_did_they_say =& new WhatDidTheySay();
$what_did_they_say_admin =& new WhatDidTheySayAdmin(&$what_did_they_say);

add_action('init', array(&$what_did_they_say_admin, 'init'));
register_activation_hook(__FILE__, array(&$what_did_they_say, 'install'));
register_activation_hook(__FILE__, array(&$what_did_they_say_admin, 'install'));

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

function the_media_transcript($language = null) {
  $transcript = apply_filters('the_media_transcript', get_the_media_transcript());
  echo $transcript;
}

function get_the_language_name($language = null) {
  global $what_did_they_say;
  
  if (is_null($language)) { $language = $what_did_they_say->get_default_language(); }  
  return $what_did_they_say->get_language_name($language);
}

function the_language_name($language = null) {
  $name = apply_filters('the_language_name', get_the_language_name($language));
  echo $name; 
}

function transcripts_display($dropdown_message = null, $single_language_message = null) {
  global $post, $what_did_they_say;
  
  if (is_null($dropdown_message)) { $dropdown_message = __('Select a language:', 'what-did-they-say'); }
  if (is_null($single_language_message)) { $single_language_message = __('%s transcript:', 'what-did-they-say'); }
  
  $output = array();
  
  $transcripts = array();
  $post_transcripts = $what_did_they_say->get_transcripts($post->ID);
  if (!empty($post_transcripts)) {
    foreach ($post_transcripts as $code => $transcript) {
      $transcript = trim($transcript);
      if (!empty($transcript)) {
        $transcripts[$code] = $transcript;
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

function the_media_transcript_queue_editor() {
  global $post, $what_did_they_say;
  
  if (current_user_can('submit_transcriptions')) { ?>
    <form method="post">
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
  <?php }
}

?>