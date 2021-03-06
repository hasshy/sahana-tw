<?php
/* $Id: main.inc,v 1.27.20.11 2008/06/03 10:50:32 ravithb Exp $ */
/**
 * Main Controller of the Missing Person Registry
 *
 * PHP version 4 and 5
 *
 * LICENSE: This source file is subject to LGPL license
 * that is available through the world-wide-web at the following URI:
 * http://www.gnu.org/copyleft/lesser.html
 *
 * @author	   Chamindra de Silva <chamindra@opensource.lk>
 * @author	   Janaka Wickramasinghe <janaka@opensource.lk>
 * @copyright  Lanka Software Foundation - http://www.opensource.lk
 * @package    module
 * @subpackage mpr
 * @license    http://www.gnu.org/copyleft/lesser.html GNU Lesser General Public License (LGPL)
 *
 */
include_once $global['approot'] . "/inc/lib_modules.inc";
include_once $global['approot'] . "/inc/lib_menu.inc";
include_once $global['approot'] . "/inc/lib_form.inc";
include_once $global['approot'] . "/inc/lib_validate.inc";
include_once $global['approot'] . "/inc/lib_errors.inc";
include_once $global['approot'] . 'mod/mpr/tools.inc';
include_once $global['approot'] . '/inc/lib_image.inc';
include_once $global['approot'] . 'mod/mpr/report.inc';
/* {{{ Hack Need to fix */
if (empty($_SESSION['user_pref_ims_incident_id'])) {
    $sql = "SELECT incident_id FROM incident ORDER BY incident_id";
    $rs = $global['db']->GetAll($sql);
    if ($rs) {
        $_SESSION['user_pref_ims_incident_id'] = $rs[0][0];
    } else {
?>
<script>
alert('There are no incidents added. Please use the admin console to add atleast one incident.');
</script>
<?php
    }
}
/* }}} */
/* {{{ Main Menu */
/**
 * This function defines the menu list.
 * @access public
 * @return void
 */
function shn_mpr_mainmenu() {
    #debug($_SESSION,1);
    global $global;
    $module = $global['module'];
    // Create the module menu
    shn_mod_menuopen(_("Missing Person Registry"));
    shn_mod_menuitem("default", _("Home"));
    shn_mod_menuitem("search&type=all", _("Search for a Person"));
    shn_mod_menuitem("search&type=missing", _("Report a Missing Person"));
    shn_mod_menuitem("search&type=missing", _("Edit a Missing Person"));
    shn_mod_menuitem("search&type=found", _("Report a Found Person"));
    //    shn_mod_menuitem("rpt_default&type=found",_("Reports"));
    shn_sub_mod_menuopen(_("Reports"));
    shn_mod_menuitem("list_missing", _("List Missing People"), $module);
    shn_mod_menuitem("list_found", _("List Found People"), $module);
    shn_mod_menuitem("list_injured", _("List Injured People"), $module);
    shn_mod_menuitem("list_deceased", _("List Deceased People"), $module);
    shn_mod_menuitem("list_other", _("List Other People"), $module);
    //  shn_mod_menuitem("org_incident","Organizations by Incident",$module);
    // shn_mod_menuitem("org_sector","Organizations by Sector",$module);
    shn_sub_mod_menuclose();
    shn_mod_menuclose();
    #    shn_mod_menuitem("manimg",_("Manage Images"));
    #    shn_mod_menuitem("test","Test");
    #    shn_mod_menuitem("stats","Statistics");
    //shn_mod_menuclose();
    // include the main menu
    include $global['approot'] . "/inc/handler_mainmenu.inc";
}
/* }}} */
/* {{{ Action: Test */
/**
 *
 * This is a testing function.
 *
 * @todo remove shn_mpr_test
 * @access public
 * @return void
 */
function shn_mpr_test() {
    global $global;
    require ($global['approot'] . 'mod/mpr/test.inc');
}
function shn_mpr_list_missing() {
    //	print 'missing';
    global $global;
    require ($global['approot'] . 'mod/mpr/list_missing.inc');
}
function shn_mpr_list_found() {
    global $global;
    require ($global['approot'] . 'mod/mpr/list_found.inc');
}
function shn_mpr_list_injured() {
    global $global;
    require ($global['approot'] . 'mod/mpr/list_injured.inc');
}
function shn_mpr_list_deceased() {
    global $global;
    require ($global['approot'] . 'mod/mpr/list_deceased.inc');
}
function shn_mpr_list_other() {
    global $global;
    require ($global['approot'] . 'mod/mpr/list_other.inc');
}
/* }}} */
/* {{{ Action: Tmp Manage Image*/
/**
 *
 * This is a testing function.
 *
 * @todo remove shn_mpr_test
 * @access public
 * @return void
 */
function shn_mpr_manimg() {
    global $global;
    require ($global['approot'] . 'mod/mpr/images.inc');
    shn_mpr_manage_images_form('p-77', 'xx');
}
/* }}} */
/* {{{ Action: Default (Home Page) */
/**
 *
 * This function displays the home page of the missing person registry
 *
 * @access public
 * @return void
 */
function shn_mpr_default() {
    global $global;
    require ($global['approot'] . 'mod/mpr/home.inc');
}
/* }}} */
/* {{{ Action: Find */
/**
 * This function displays the basic search form
 * @deprecated
 * @todo shn_mpr_fndmp to be removed
 * @access public
 * @return void
 */
function shn_mpr_fndmp() {
    global $global;
    $header = array('method' => 'POST', 'action' => 'index.php?mod=mpr&act=addmp', 'id' => 'formset');
    shn_form_open($header, false);
?>  <input type="text" value="hello"/>          <?php
    shn_form_close();
}
/* }}} */
/* {{{ Action: Edit */
/**
 * This is the MPR Edit Controller
 * @access public
 * @return void
 */
function shn_mpr_editmp() {
    global $global;
    require ($global['approot'] . 'mod/mpr/edit.inc');
    require_once ($global['approot'] . 'mod/mpr/add.inc');
    //validation
    $seq = isset($_REQUEST['seq']) ? $_REQUEST['seq'] : '';
    switch ($seq) {
        case 'entry':
            echo '<h1 align="center">' . _('Edit Missing Person Entry') . '</h1>';
            if (shn_mpr_editmp_validate()) {
                shn_edit_show_trackers($_SESSION['mpr_edit']['entry']['p_uuid']);
            } else shn_mpr_editmp_entry(true);
            break;
        case 'trackers':
            echo '<h1 align="center">' . _('Edit Missing Person Entry') . '</h1>';
            shn_mpr_editmp_confirm();
            break;
        case 'commit':
            if (isset($_POST['yes'])) {
                shn_mpr_editmp_commit();
                //Send Back to the Main Page
                shn_mpr_default();
            } else {
                shn_mpr_editmp_entry();
            }
            break;
        case 'add_tracker':
            echo '<h1 align="center">' . _('Edit Missing Person Entry') . '</h1>';
            $_POST = null;
            $_SESSION['mpr_add']['add_tracker'] = null;
            echo '<h1 align="center">' . _('Add a tracker to') . ' ' . $_SESSION['mpr_edit']['entry']['full_name'] . ' </h1>';
            $_SESSION['mpr_add']['add_tracker']['p_uuid'] = $_SESSION['mpr_edit']['entry']['p_uuid'];
            // todo: check the p_uuid exists or not *** VERY URGENT
            shn_mpr_addmp_reporter();
            break;
        case 'report_person':
            echo '<h1 align="center">' . _('Edit Missing Person Entry') . '</h1>';
            if (shn_mpr_addmp_reporter_validate() || isset($_GET['offset'])) {
                if ($_POST['reported_before'] == 'yes' || isset($_GET['offset'])) {
                    require_once ($global['approot'] . 'mod/mpr/search.inc');
                    if (isset($_GET['offset'])) {
                        $arr = shn_mpr_search_result_vw($_SESSION['mpr']['search']['subquery'], $_GET['offset'], true);
                        shn_mpr_search_select($arr);
                    } else {
                        $search_sql = new SearchQuery();
                        $search_sql->set_name($_POST['rep_full_name']);
                        $search_sql->set_address($_POST['rep_address']);
                        $search_sql->set_phone($_POST['rep_phone']);
                        $search_sql->istracker = true;
                        $arr = shn_mpr_search_result_vw($search_sql->get_sql(), -1, true);
                        shn_mpr_search_select($arr);
                    }
                } else {
                    shn_mpr_addmp_insert_reporter($_SESSION['mpr_add']['add_tracker']['p_uuid']);
                    shn_edit_show_trackers($_SESSION['mpr_edit']['entry']['p_uuid']);
                    $_SESSION['mpr_add']['add_tracker'] = null;
                }
            } else {
                echo '<h1 align="center">' . _('Add a tracker to') . ' ' . $_SESSION['mpr_add']['add_tracker']['name'] . ' </h1>';
                shn_mpr_addmp_reporter(true);
            }
            break;
        case 'tracker_select':
            shn_add_person_to_report($_SESSION['mpr_edit']['entry']['p_uuid'], $_GET['tracker_id'], $_SESSION['mpr_add']['report_person']['rep_relation']);
            $_SESSION['mpr_add']['add_tracker'] = null;
            shn_edit_show_trackers($_SESSION['mpr_edit']['entry']['p_uuid']);
            break;
        case 'del_tracker':
            echo '<h1 align="center">' . _('Edit Missing Person Entry') . '</h1>';
            //todo : Clean the input
            $tracker_uuid = $_POST['tracker_id'];
            if (isset($_POST['yes'])) {
                shn_edit_del_tracker($_SESSION['mpr_edit']['entry']['p_uuid'], $tracker_uuid);
            }
            shn_edit_show_trackers($_SESSION['mpr_edit']['entry']['p_uuid']);
            break;
        case 'con_del_tracker':
            echo '<h1 align="center">' . _('Edit Missing Person Entry') . '</h1>';
            shn_form_fopen("editmp", null, array('req_message' => false, 'id' => 'del'));
            shn_form_fsopen('Confirm');
            shn_form_hidden(array('seq' => 'del_tracker', 'tracker_id' => $_GET['tracker_id']));
            echo sprintf(_('Are you sure you want to delete tracker %s from tracker list'), $_GET['tracker_name']);
            echo '<br/>';
            echo '<br/>';
            shn_form_submit("Yes", 'name="yes"');
            shn_form_submit("No", 'name="no"');
            shn_form_fsclose();
            shn_form_fclose();
            shn_edit_show_trackers($_SESSION['mpr_edit']['entry']['p_uuid']);
            break;
        default:
            $_SESSION['mpr_edit']['entry'] = null;
            if (isset($_GET['id'])) {
                shn_mpr_editmp_load($_GET['id']); //load details
            }
            
            if ($_SESSION['mpr_edit']['entry']['opt_status'] == 'mis') {
                echo '<h1 align="center">' . _('Edit Missing Person Entry') . '</h1>';
            } else {
                echo '<h1 align="center">' . _('Edit Found Person Entry') . '</h1>';
            }
            shn_mpr_editmp_entry();
            break;
        }
}
/* }}} */
/* {{{ Action: View */
/**
 * This is the MPR View Controller
 * @access public
 * @return void
 */
function shn_mpr_viewmp() {
    global $global;
    require ($global['approot'] . 'mod/mpr/view.inc');
    echo '<h1 align="center">' . _('View Person') . '</h1>';
    shn_mpr_view($_GET['id']);
}
/* }}} */
/* {{{ Action: Add Tracker */
/**
 * This is the MPR Add Tracker Controller
 * @access public
 * @return void
 */
function shn_mpr_add_tracker() {
    global $global;
    require ($global['approot'] . 'mod/mpr/add.inc');
    //Controller
    $seq = isset($_REQUEST['seq']) ? $_REQUEST['seq'] : '';
    switch ($seq) {
        case 'report_person':
            if (shn_mpr_addmp_reporter_validate()) {
                if ($_POST['reported_before'] == 'yes') {
                    require_once ($global['approot'] . 'mod/mpr/search.inc');
                    $search_sql = new SearchQuery();
                    $search_sql->set_name($_POST['rep_full_name']);
                    $search_sql->set_address($_POST['rep_address']);
                    $search_sql->set_phone($_POST['rep_phone']);
                    $search_sql->istracker = true;
                    $arr = shn_mpr_search_result_vw($search_sql->get_sql(), -1, -1, true);
                    shn_mpr_search_select($arr);
                } else {
                    shn_mpr_addmp_insert_reporter($_SESSION['mpr_add']['add_tracker']['p_uuid']);
                    // todo: redirect back to search results (that'll be great)
                    shn_mpr_default();
                    $_SESSION['mpr_add']['add_tracker'] = null;
                }
            } else {
                echo '<h1 align="center">' . _('Add a tracker to') . ' ' . $_SESSION['mpr_add']['add_tracker']['name'] . ' </h1>';
                shn_mpr_addmp_reporter(true);
            }
        break;
        case 'tracker_select':
            shn_add_person_to_report($_SESSION['mpr_add']['add_tracker']['p_uuid'], $_GET['tracker_id'], $_SESSION['mpr_add']['report_person']['rep_relation']);
            shn_mpr_default();
            $_SESSION['mpr_add']['add_tracker'] = null;
        break;
        default:
            //clean the $_POST to remove the conflicts
            $_POST = null;
            $_SESSION['mpr_add']['add_tracker'] = null;
            if(isset($_GET['name'])) {
                echo '<h1 align="center">' . _('Add a tracker to') . ' ' . $_GET['name'] . ' </h1>';
                $_SESSION['mpr_add']['add_tracker']['p_uuid'] = $_GET['id'];
                $_SESSION['mpr_add']['add_tracker']['name'] = $_GET['name'];
            } else {
                $_SESSION['mpr_add']['add_tracker']['p_uuid'] = '';
                $_SESSION['mpr_add']['add_tracker']['name'] = '';
            }
            // todo: check the p_uuid exists or not *** VERY URGENT
            shn_mpr_addmp_reporter();
        break;
    }
}
/* }}} */
/* {{{ Action: Delete Tracker */
/**
 * This is the MPR Delete Tracker Controller
 * @access public
 * @return void
 */
function shn_mpr_del_tracker() {
    global $global;
    if (shn_mpr_addmp_reporter_validate()) {
        shn_mpr_addmp_insert_reporter($_SESSION['mpr_add']['add_tracker']['p_uuid']);
        // todo: rediddrect back to search results (that'll be great)
        #shn_mpr_default();
        $_SESSION['mpr_add']['add_tracker'] = null;
    } else {
        echo '<h1 align="center">' . _('Add a tracker to') . ' ' . $_SESSION['mpr_add']['add_tracker']['name'] . ' </h1>';
        shn_mpr_addmp_reporter(true);
    }
}
/* }}} */
/* {{{ Action: Add */
/**
 * This is the MPR Add Controller
 * @access public
 * @return void
 */
function shn_mpr_addmp() {
    global $global;
    require ($global['approot'] . 'mod/mpr/add.inc');
    if (isset($_GET['type']) && $_GET['type'] == 'found') {
        $_SESSION['mpr_add']['entry_type'] = 'found';
        $heading = '<h1 align="center">' . _('Found Person Entry') . '</h1>';
    } else {
        /*
         * if ($_GET['type'] == 'missing')
         */
        $_SESSION['mpr_add']['entry_type'] = 'missing';
        $heading = '<h1 align="center">' . _('Missing Person Entry') . '</h1>';
    }
    //Controller
    $seq = !empty($_REQUEST['seq']) ? $_REQUEST['seq'] : '';
    switch ($seq) {
        case 'entry':
            if (shn_mpr_addmp_validate()) {
                //clean the $_POST to remove the conflicts
                $_POST = null;
                echo $heading;
                echo '<h2 align="center">(' . _('Reporting Person') . ')</h2>';
                shn_mpr_addmp_reporter();
            } else {
                echo $heading;
                echo '<h2 align="center">(' . _('Person Details') . ')</h2>';
                shn_mpr_addmp_entry(true);
            }
        break;
        case 'tracker_back':
            echo $heading;
            echo '<h2 align="center">(' . _('Reporting Person') . ')</h2>';
            shn_mpr_addmp_reporter();
        break;
        case 'tracker_select':
            $_SESSION['mpr_add']['entry']['tracker_id'] = $_GET['tracker_id'];
            shn_mpr_addmp_commit();
            echo $heading;
            echo '<h2 align="center">(' . _('The ' . $_SESSION['mpr_add']['entry']['type'] . ' person have been added') . ')</h2>';
            shn_mpr_addmp_confirm();
            //shn_mpr_addmp_commit();
            
        break;
        case 'report_person':
            if (shn_mpr_addmp_reporter_validate() || isset($_GET['offset'])) {
                //if reported before then send to the search
                if ($_POST['reported_before'] == 'yes' || isset($_GET['offset'])) {
                    echo $heading;
                    echo '<h2 align="center">(' . _('Please Select the person who is reporting') . ')</h2>';
                    require_once ($global['approot'] . 'mod/mpr/search.inc');
                    if (isset($_GET['offset'])) {
                        $arr = shn_mpr_search_result_vw($_SESSION['mpr']['search']['subquery'], $_GET['offset'], true);
                        shn_mpr_search_select($arr);
                    } else {
                        $search_sql = new SearchQuery();
                        $search_sql->set_name($_POST['rep_full_name']);
                        $search_sql->set_address($_POST['rep_address']);
                        $search_sql->set_phone($_POST['rep_phone']);
                        $search_sql->istracker = true;
                        $arr = shn_mpr_search_result_vw($search_sql->get_sql(), -1, true);
                        shn_mpr_search_select($arr);
                    }
                } else {
                    shn_mpr_addmp_commit();
                    echo $heading;
                    echo '<h2 align="center">(' . _('The ' . $_SESSION['mpr_add']['entry']['type'] . ' person have been added') . ')</h2>';
                    shn_mpr_addmp_confirm();
                }
            } else {
                echo $heading;
                echo '<h2 align="center">(' . _('Reporting Person') . ')</h2>';
                shn_mpr_addmp_reporter(true);
            }
        break;
        case 'commit':
            shn_mpr_addmp_commit();
        break;
        default:
            echo $heading;
            echo '<h2 align="center">(' . _('Reporting Person') . ')</h2>';
            shn_mpr_addmp_entry();
            $_SESSION['mpr_add'] = null;
        break;
    }
}
/* }}} */
/* {{{ Action: Search */
/**
 * This is the Search Controller
 * @access public
 * @return void
 */
function shn_mpr_search() {
    global $global;
    $type = isset($_REQUEST['type']) ? $_REQUEST['type'] : (
            isset($_SESSION['mpr']['search']['type']) ? $_SESSION['mpr']['search']['type'] : 'all');
    if ($type == 'missing') {
        $_SESSION['mpr']['search']['type'] = 'missing';
        echo '<h1 align="center">' . _("Report a Missing Person") . '</h1>';
    } elseif($type == 'found') {
        $_SESSION['mpr']['search']['type'] = 'found';
        echo '<h1 align="center">' . _("Report a Found Person") . '</h1>';
    } else {
        $_SESSION['mpr']['search']['type'] = 'all';
        echo '<h1 align="center">' . _('Search') . '</h1>';
    }
    require ($global['approot'] . 'mod/mpr/search.inc');
    //Controller for Normal Search
    if ($_GET['act'] == 'search') {
        $seq = !empty($_REQUEST['seq']) ? $_REQUEST['seq'] : '';
        switch ($seq) {
            case 'result':
                shn_mpr_search_default();
                if (isset($_GET['offset'])) shn_mpr_search_result_vw($_SESSION['mpr']['search']['subquery'], $_GET['offset']);
                else {
                    $search_sql = new SearchQuery();
                    if (!empty($_POST['name'])) {
                        $search_sql->set_name($_POST['name']);
                        $_SESSION['mpr']['search']['keyword']['name'] = $_POST['name'];
                    }
                    if (!empty($_POST['idcard'])) {
                        $search_sql->set_idcard_no($_POST['idcard']);
                        $_SESSION['mpr']['search']['keyword']['idcard'] = $_POST['idcard'];
                    }
                    if (!empty($_POST['anycard'])) {
                        $search_sql->set_serial_no($_POST['anycard']);
                        $_SESSION['mpr']['search']['keyword']['anycard'] = $_POST['anycard'];
                    }
                    #$search_sql->isvictim = true;
                    if (!empty($_REQUEST['all_incidents']))
                        $search_sql->incident_list = "'" . implode(shn_db_get_all_incidents($_SESSION['user_pref_ims_incident_id']), "','") . "'";
                    /*
                    if ($_SESSION['mpr']['search']['type'] == 'missing') {
                        $search_sql->missing_only = true;
                        $search_sql->isvictim = true;
                    } elseif($_SESSION['mpr']['search']['type'] == 'found') {
                        $search_sql->found_only = true;
                    }
                     * 
                     */
                    #debug($search_sql->get_sql());
                    $arr = shn_mpr_search_result_vw($search_sql->get_sql());
                }
                break;
            default:
                shn_mpr_search_default();
                break;
            }
            //Controller for Advance Search
            
        } elseif ($_GET['act'] == 'search_adv') {
            switch ($_POST['seq']) {
                case 'result':
                    shn_mpr_search_result();
                break;
                default:
                    shn_mpr_search_advance();
                break;
            }
        }
    }
    /* }}} */
    /* {{{ Action: Advance Search */
    /**
     * This is the Advance Search Controller
     * @todo Add Advance Filters
     * @access public
     * @return void
     */
    function shn_mpr_search_adv() {
        global $global;
        echo '<h1 align="center">Search</h1>';
        require ($global['approot'] . 'mod/mpr/search.inc');
    }
    /* }}} */
    /* {{{ Action: Audit */
    /**
     * This is the Auditing Controller
     * @todo Need to discuss this further
     * @access public
     * @return void
     */
    function shn_mpr_auditmp() {
        global $global;
        require ($global['approot'] . 'mod/mpr/audit.inc');
        shn_audit_show($_GET['id']);
    }
    /* }}} */
    /* {{{ Status */
    /**
     * This is function change the status from missing to alive and well
     * @access public
     * @return void
     */
    function shn_mpr_status() {
        global $global;
        //clean and check
        $update_array['p_uuid'] = shn_db_clean($_GET['id']);
        if (!$global['db']->Execute("SELECT p_uuid FROM person_status WHERE p_uuid = {$update_array['p_uuid']}")) {
            shn_mpr_default();
        } else {
            //flush and refill baby
            $global['db']->Execute("DELETE FROM person_status WHERE p_uuid = {$update_array['p_uuid']}");
            $update_array['p_uuid'] = $_GET['id'];
            //Insert Into person_status mis
            $update_array['opt_status'] = 'ali';
            shn_db_insert($update_array, 'person_status');
            //reset $update_array
            $update_array = null;
            shn_mpr_default();
        }
    }
    /* }}} */
    /**
     * shn_soap_mpr_addmp
     *
     * @access public
     * @return string
     */
    function shn_soap_mpr_addmp() {
    }
    /**
     * shn_soap_mpr_findmp
     *
     * @param mixed $strAnyName
     * @param mixed $strAnyID
     * @param mixed $strIncident
     * @access public
     * @return string
     */
    function shn_soap_mpr_findmp() {
    }
    /**
     * shn_soap_mpr_default
     *
     * @access public
     * @return string
     */
    function shn_soap_mpr_default() {
        return "hello";
    }
    //images
    function shn_image_mpr_addmp_img() {
        global $global;
        //clean clean
        $x_uuid = $_GET['x_uuid'];
        switch ($_GET['stream']) {
            case 'image':
                if ($_GET['session']) {
                    shn_show_image_stream($x_uuid, true, $_SESSION['mpr']['entry'], true);
                } else {
                    if ($_GET['thumbnail']) shn_show_image_stream($x_uuid, true);
                    else shn_show_image_stream($x_uuid, false);
                }
                break;
            }
        }
        /**
         * End the add or edit mpr record flow by clearing the session
         * and redirecting to the default view.
         *
         */
        function shn_mpr_end_flow() {
            unset($_SESSION['mpr_add']);
            unset($_SESSION['mpr']['entry']);
            shn_mpr_default();
        }
?>