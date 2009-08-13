<div class="wrap">
  <h2>Manage Transcripts</h2>
  
  <h3>Queued Transcripts</h3>
  
  <h3>Available Languages</h3>
  
  <ul id="language-holder">
    <?php foreach ($languages as $language) {
      $default = false;
      if (is_string($language)) { $name = $language; }
      if (is_array($language)) {
        $name = $language['name'];
        $default = $language['default']; 
      }
      $key = md5($name);
      ?>
      <li>
        <input type="text" name="wdts[language][<?php echo $key ?>]" value="<?php echo $name ?>" />
        <input type="radio" name="wdts[language][_default]" value="<?php echo $key ?>" 
      </li>
    <?php } ?>
  </ul>
</div>