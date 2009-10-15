<?php if (current_user_can('edit_users') && is_array($options)) { ?>
  <form method="post">
    <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
    <input type="hidden" name="wdts[module]" value="capabilities" />
    <h3><?php _e('Capabilities', 'what-did-they-say') ?></h3>

    <p><?php _e('Determine who can do what with transcripts by setting the minimum role requirements below.', 'what-did-they-say') ?></p>

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

    <h3><?php _e('Per-user Permissions', 'what-did-they-say') ?></h3>

    <p><?php _e('Give specified users permissions above and beyond their role permissions.', 'what-did-they-say') ?></p>

  <form method="post">
    <input type="hidden" name="wdts[_nonce]" value="<?php echo $nonce ?>" />
    <input type="hidden" name="wdts[module]" value="user-capabilities" />
    <table class="widefat fixed">
      <tr>
        <th width="15%">Login</th>
        <th colspan="3">Capabilities</th>
      </tr>
      <?php foreach ((array)$users as $user) {
        $user_capabilities = get_usermeta($user->ID, 'transcript_capabilities');
        if (!is_array($user_capabilities)) { $user_capabilities = array(); }
        ?>
        <tr> 
          <th scope="row">
            <?php echo get_avatar($user->ID, 20) ?>
            <?php echo $user->user_login ?>
          </th>
          <?php foreach ($this->capabilities as $key => $label) { ?>
            <td>
              <label style="white-space: nowrap">
                <input type="checkbox"
                       name="wdts[user_capabilities][<?php echo $user->ID ?>][<?php echo $key ?>]"
                       value="yes"
                       <?php echo (isset($user_capabilities[$key])) ? 'checked="checked"' : '' ?>
                       />
                <?php echo $label ?>
              </label>
            </td>
          <?php } ?>
        </tr>
      <?php } ?>
      <tr>
        <td>&nbsp;</td>
        <td colspan="3">
          <input class="button" type="submit" value="<?php _e('Update per-user permissions', 'what-did-they-say') ?>" />
        </td>
      </tr>
    </table>
  </form>
<?php } ?>

