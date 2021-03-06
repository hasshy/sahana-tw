<?php
function _shn_ims_config_form($errors = false, $updated = false) {
    global $conf;
    if ($errors) display_errors();
    shn_form_fopen("ims_config", null);
    if ($updated) {
        shn_form_fsopen("");
?>		<p align="center">Updated. <a href="<?=$_SERVER['PHPSELF']; ?>">Click to see the menu change</a></p>
<?php
        shn_form_fsclose();
    }
    shn_form_hidden(array('seq' => 'entry'));
    shn_form_fsopen(_("Activate/Deactivate"));
    if ($conf['mod_admin_ims_active'] == true) {
        $extra_opt['value']= '1';
    } else {
        $extra_opt['value']= false;
    }
    $extra_opt['help'] = _('This will activate / deactivate incident management and the incident hierarchy.');
    shn_form_checkbox(_("Activate"), 'frm_active', null, $extra_opt);
    shn_form_fsclose();

    if($conf['mod_admin_ims_active'] == true) {
        shn_form_fsopen(_("Grouping"));
        shn_form_text(_("Level1"), "mod_admin_ims_level1", null, array('value' => $conf['mod_admin_ims_level1'], 'req' => true));
        shn_form_text(_("Level2"), "mod_admin_ims_level2", null, array('value' => $conf['mod_admin_ims_level2'], 'req' => true));
        shn_form_text(_("Level3"), "mod_admin_ims_level3", null, array('value' => $conf['mod_admin_ims_level3'], 'req' => true));
        shn_form_fsclose();
    }
    shn_form_submit(_('Update'));
    shn_form_fclose();
}
function _shn_ims_config_validate() {
    global $conf;
    $error_flag = false;
    //clean the post
    shn_tools_clean_post(&$local_post);
    //anything entered?
    if (empty($local_post)) {
        add_error(_("You have not completed the form"));
        return false;
    }
    if (!isset($local_post['mod_admin_ims_level1']) && $conf['mod_admin_ims_active'] == true) {
        add_error(_("Please enter the name for Level1"));
        $error_flag = true;
    }
    if (!isset($local_post['mod_admin_ims_level2']) && $conf['mod_admin_ims_active'] == true) {
        add_error(_("Please enter the name for Level2"));
        $error_flag = true;
    }
    if (!isset($local_post['mod_admin_ims_level3']) && $conf['mod_admin_ims_active'] == true) {
        add_error(_("Please enter the name for Level3"));
        $error_flag = true;
    }
    if ($error_flag) return false;
    else {
        $enabled = (isset($_POST['frm_active']))? 1: 0;
        $conf['mod_admin_ims_active'] = $enabled;
        shn_config_database_update('mod_admin_ims_active',$enabled, 'admin');
        shn_db_config_update('admin', 'ims_level1', $local_post['mod_admin_ims_level1']);
        shn_db_config_update('admin', 'ims_level2', $local_post['mod_admin_ims_level2']);
        shn_db_config_update('admin', 'ims_level3', $local_post['mod_admin_ims_level3']);
        return true;
    }
}
?>
