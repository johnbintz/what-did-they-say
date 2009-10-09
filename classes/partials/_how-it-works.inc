<?php if (current_user_can('submit_transcriptions')) { ?>
  <h3><?php _e('How It Works', 'what-did-they-say') ?></h3>

  <p>
    <?php _e('<strong>What Did They Say?!?</strong> uses a wspecial workflow to handle the submission and approval of transcripts.', 'what-did-they-say') ?>
    <?php _e('Below are the typical usage patterns of <strong>What Did They Say?!?</strong>:', 'what-did-they-say') ?>
  </p>

  <hr />

	<h4><?php _e('A single user updating their own transcripts', 'what-did-they-say') ?></h4>

	<p>
    <?php _e('If you\'re the only user of the site, you would edit the <strong>Approved transcripts</strong> of each post. You don\'t need to enable transcript submissions for any of your posts.', 'what-did-they-say') ?>
    <?php _e('All users need to have enough permissions to <strong>Approve Transcripts</strong>.', 'what-did-they-say') ?>
	</p>

  <hr />

	<h4><?php _e('Multiple users who directly update transcripts on posts', 'what-did-they-say') ?></h4>

	<p>
    <?php _e('If you trust the other users on your site, you all would edit the <strong>Approved transcripts</strong> of each post. Again, you don\'t need to enable transcript submissions for any of your posts.', 'what-did-they-say') ?>
    <?php _e('All users need to have enough permissions to <strong>Approve Transcripts</strong>.', 'what-did-they-say') ?>
	</p>

  <hr />
	
	<h4><?php _e('Multiple users, some of which submit transcripts and some that approve transcripts', 'what-did-they-say') ?></h4>

	<p>
    <?php _e('The users who are allowed to submit transcripts get the <strong>Submit Transcripts</strong> capability.', 'what-did-they-say') ?>
    <?php _e('The users who aprove transcripts get the <strong>Approve Transcripts</strong> capability.', 'what-did-they-say') ?>
  </p>
	
	<p>
    <?php _e('For posts where users are allowed to submit transcripts, a user with Approve Transcripts needs to enable <strong>Allow new transcripts for this post</strong>.', 'what-did-they-say') ?>
  </p>

	<p>
    <?php _e('Transcript submitters need to submit transcripts from the post directly, not from the Admin interface.', 'what-did-they-say') ?>
    <?php _e('Transcript submitters submit their transcripts to the queue, making them <strong>Queued Transcripts</strong>.', 'what-did-they-say') ?>
    <?php _e('Submitters can edit and delete their own transcripts while they\'re in the queue. Once they\'re approved, they can no longer be edited by the submitter.', 'what-did-they-say') ?>
	</p>

	<p>
    <?php _e('Transcript approvers can approve, edit, or delete queued transcripts of any users.', 'what-did-they-say') ?>
    <?php _e('Once a transcript is approved, it\'s removed from the queue and made live on the site.', 'what-did-they-say') ?>
	</p>
<?php } ?>
