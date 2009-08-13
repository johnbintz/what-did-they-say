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

?>