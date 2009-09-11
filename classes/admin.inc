<div class="wrap">
  <h2><?php _e('What Did They Say?!? Settings', 'what-did-they-sahy') ?></h2>
  
  <?php if (current_user_can('edit_users') && is_array($options)) { ?>
    <form method="post">
      <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
      <input type="hidden" name="wdts[module]" value="capabilities" />
      <h3><?php _e('Capabilities', 'what-did-they-say') ?></h3>
      
      <table class="widefat fixed">
        <tr class="thead">
          <th scope="col" class="manage-col"><?php _e('Capability', 'what-did-they-say') ?></th>
          <th scope="col" class="manage-col"><?php _e('Role Needed', 'what-did-they-say') ?></th>        
        </tr>
        <?php foreach ($this->capabilities as $key => $label) { ?>
          <tr>
            <th scope="col"><?php echo $label ?></th>
            <td>
              <select name="wdts[capabilities][<?php echo $key ?>]">
                <?php wp_dropdown_roles($options['capabilities'][$key]) ?>
              </select>
            </td>
          </tr>
        <?php } ?>
        <tr>
          <td>&nbsp;</td>
          <td>
            <input class="button" type="submit" value="<?php _e('Change capabilities', 'what-did-they-say') ?>" />          
          </td>
        </tr>
      </table>      
    </form>
  <?php } ?>

  <?php if (is_array($options)) { ?>
    <h3><?php _e('Languages', 'what-did-they-say') ?></h3>

    <table class="widefat fixed">
      <tr class="thead">
        <th scope="col" class="manage-col" width="35%"><?php _e('Language', 'what-did-they-say') ?></th>
        <th scope="col" class="manage-col"><?php _e('Rename?', 'what-did-they-say') ?></th>
        <th scope="col" class="manage-col"><?php _e('Make default?', 'what-did-they-say') ?></th>
        <th scope="col" class="manage-col"><?php _e('Delete?', 'what-did-they-say') ?></th>
      </tr>
      <?php foreach ($options['languages'] as $code => $info) {
        $default = isset($info['default']);
        $name = $info['name'];
        ?>
        <tr>
          <th scope="row">
            <span><?php echo $name ?></span>
            <?php if ($default) { _e('(default)', 'what-did-they-say'); } ?>
            <form method="post" style="display: none">
              <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
              <input type="hidden" name="wdts[code]" value="<?php echo $code ?>" />
              <input type="hidden" name="wdts[action]" value="rename" />
              <input type="hidden" name="wdts[module]" value="languages" />
              <input type="text" name="wdts[name]" value="<?php echo $name ?>" style="width: 50%" />
              <input type="submit" class="button" value="Rename" />
            </form>
          </th>
          <td style="vertical-align: inherit">
            <a class="rename button" href="#"><?php _e('Rename', 'what-did-they-say') ?></a>
          </td>
          <td>
            <form method="post">
              <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
              <input type="hidden" name="wdts[code]" value="<?php echo $code ?>" />
              <input type="hidden" name="wdts[action]" value="default" />
              <input type="hidden" name="wdts[module]" value="languages" />
              <input
                <?php echo ($default ? 'disabled="disabled"' : '') ?>
                type="submit"
                class="button"
                value="<?php _e('Default', 'what-did-they-say') ?>" />
            </form>
          </td>
          <td>
            <form method="post" class="verify">
              <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
              <input type="hidden" name="wdts[code]" value="<?php echo $code ?>" />
              <input type="hidden" name="wdts[action]" value="delete" />
              <input type="hidden" name="wdts[module]" value="languages" />
              <input type="submit" class="button" value="<?php _e('Delete', 'what-did-they-say') ?>" />
            </form>
          </td>
        </tr>
      <?php } ?>
      <tr>
        <th scope="row"><?php _e('Add new:', 'what-did-they-say') ?></th>
        <td colspan="3">
          <form method="post">
            <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
            <input type="hidden" name="wdts[action]" value="add" />
            <input type="hidden" name="wdts[module]" value="languages" />
            <select name="wdts[code]">
              <option value="">-- select --</option>
              <?php foreach ($this->all_languages as $code => $language) { ?>
                <option value="<?php echo $code ?>"><?php echo $language ?></option>
              <?php } ?>
            </select>
            <input type="submit" value="<?php _e('Add New Language', 'what-did-they-say') ?>" class="button" />
          </form>
        </td>
      </tr>
    </table>
  <?php } ?>
  <?php if (current_user_can('manage_options')) { ?>
    <h3><?php _e('Reset Settings to Default', 'what-did-they-say') ?></h3>
    <p><?php _e('Click the button below to reset capabilities and languages to their defaults. This will not affect any transcriptions you have already created, but some transcriptions may become inaccessible if you don\'t redefine the original language.', 'what-did-they-say') ?></p>
    <form method="post" id="reset-wdts">
      <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
      <input type="hidden" name="wdts[module]" value="reset-options" />
      <input type="submit" class="button" value="<?php _e('Reset What Did They Say?!?', 'what-did-they-say') ?>" />
    </form>
  <?php } ?>
</div>

<script type="text/javascript">
  $$('.verify').each(function(f) {
    Event.observe(f, 'submit', function(e) {
      if (!confirm("<?php _e('You are about to delete this language from the available list. Continue?', 'what-did-they-say') ?>")) {
        Event.stop(e);
      }
    });
  });
  
  $$('a.rename').each(function(a) {
    Event.observe(a, 'click', function(e) {
      Event.stop(e);
      var th = a.parentNode.parentNode.select('th')[0];
      if (th) {
        th.select('span')[0].hide();        
        th.select('form')[0].show();        
      }
    });
  });

  if ($('reset-wdts')) {
    Event.observe($('reset-wdts'), 'submit', function(e) {
      if (!confirm('<?php _e('Are you sure? You could leave transcriptions inaccessible.', 'what-did-they-say') ?>')) {
        Event.stop(e);
      }
    });
  }
</script>

