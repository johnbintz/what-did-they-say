=== What Did They Say?!? ===
Contributors: johncoswell
Donate link: http://www.coswellproductions.com/wordpress/wordpress-plugins
Tags: admin, comics, webcomics, transcript, video, audio
Requires at least: 2.8
Tested up to: 2.8.4
Stable tag: 0.9.1

What Did They Say?!? lets your users provide multilingual transcriptions to your media, in their language, quickly and securely.

== Description ==

Run a webcomic site? Post a lot of videos or audio files for your readers to enjoy? Wish you could easily and attractively present
transcripts for your media files...and even let dedicated users manage those transcripts for you? Then What Did They Say?!? is what you need!

**What Did They Say?!?** (WDTS) integrates fully with your WordPress site. You can use it without making any theme modifications, and you can customize
the output as little or as much as you want. Site search also searches transcript content. It's easy to add new languages to support.
And there's a complete transcript workflow for editing, submitting, and approving transcripts, so it's simple and safe to allow others
to provide transcripts. Backups are also easy, as transcript data is stored with each post -- no separate database tables to worry about.

== Installation ==

Download and unzip the latest version of **What Did They Say?!?** to your `wp-content/plugins` directory. If you plan on using custom transcript
filters, create the directory `wp-content/transcript-filters`.

**What Did They Say?!?** puts a menu item under Options. It also adds a meta box to post editing.

== Frequently Asked Questions ==

= How do I activate the automatic site integration? =

On the WDTS *Introduction* page, check **Enable automatic transcript embedding**. Some themes do not play nicely with this embedding
by default, so you may be asked to enable **Inject transcripts at level 100 instead of level 15** on the *Misc. Options* page.

= How do I add transcript excerpts to search results? =

In the appropriate place in your search results Loop, add the following Template Tag:

`<?php if (function_exists('the_matching_transcript_excerpts')) {
  the_matching_transcript_excerpts();
} ?>`

= How do I create my own transcript filters and styles? =

There are two ways to do it:

* If the new filters and styles are for one particilar theme, add your filters and styles to that theme's functions.php.
* If the new filters and styles will go on different themes, or you want to distribute them, add them to the `wp-content/transcript-filters` directory.

If you're using the `wp-content/transcript-filters` directory, use the following process. This requries some technical knowledge:

* Create a new directory for your filter, using only lowercase characters, the numbers 0-9, dashes, and underscores.
* Inside of this directory place one .php/.inc file and/or one .css file.
* If you're overriding filters, create a new class in the PHP file that extends `WDTSDisplayFilters  (defined in `classes/WDTSDisplayFilters.inc`) and override functions as necessary.
* If done correctly, the new filter will be available under the *Styles* tab.

= How does the transcript workflow work? =

Transcripts can be in one of two states: **queued** and **approved**. Users can interact with them depending upon the permissions given to them:

* Queued transcripts are only visible to the user who submitted the transcript and to users that can approve transcripts.
* Approved transcripts are visible to everyone.
* Users who can submit transcripts can create new queued transcripts and edit their queued transcripts.
* Once a transcript is approved, it can only be edited by users who can approve transcripts.
* Users who can approve transcripts can also directly edit the approved transcripts.
* If a user submits a transcript to the queue, they can delete it before it's approved.

Permissions are handed out based on role and is set under the *Capabilities* tab.

= How do short codes work? =

**Short codes** are the recommended way that you create your transcripts. They eliminate the dependency on particular HTML structures and styles, which means
it's trivial to rework how your transcripts look and behave. It also improves security as it's less likely that bad HTML will sneak into your transcripts.
Short codes information can be found under the *Short Codes Info* tab. Each transcript editing area has buttons that allow you to quickly insert the proper
short codes into your transcripts.

== Screenshots ==

1. The **What Did They Say?!?** admin screen, with tabs running across the top.
2. The Manage Transcripts meta box, including the short codes buttons.
3. The Language management screen.

== Credits ==

Language tags are from the subtag registry at [http://www.langtag.net/](http://www.langtag.net/).

Thanks to @mindfaucet, @Frumph, and @ChibiJen for early testing and bug reporting.

== Changelog ==

= 0.9.1 =
* Bugfixes for transcript submission and theme editor global variable name collision.
* Improve transcript submission feedback.
= 0.9 =
* Initial release, primarily for testing purposes.