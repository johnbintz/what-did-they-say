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

