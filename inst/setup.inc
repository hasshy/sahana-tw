<?php
/**
 * Sahana web setup. Initially launched to setup the database and sahana configuration
 *
 * PHP version 4 and 5
 *
 * LICENSE: This source file is subject to LGPL license
 * that is available through the world-wide-web at the following URI:
 * http://www.gnu.org/copyleft/lesser.html
 *
 * @package    Sahana - http://sahana.sourceforge.net
 * @author     http://www.linux.lk/~chamindra
 * @author     Pradeeper http://www.linux.lk/~pradeeper
 * @author     Ravindra http://r4vi.org
 * @copyright  Lanka Software Foundation - http://www.opensource.lk
 */
session_start();
if (!function_exists("gettext")) {
    global $global;
    include_once ($global['approot'] . 'inc/lib_locale/lib_gettext.inc');
}
require_once ($global['approot'] . 'inc/lib_security/constants.inc');
require_once ($global['approot'] . 'inc/lib_security/lib_auth.inc');
require_once ($global['approot'] . 'inc/handler_html_head.inc');
require_once ($global['approot'] . 'inc/lib_modules.inc');
require_once ($global['approot'] . 'inc/lib_form.inc');
require_once ($global['approot'] . 'inc/lib_errors.inc');
require_once ($global['approot'] . '3rd/adodb/adodb.inc.php');
require_once ($global['approot'] . 'inc/lib_uuid.inc');
require_once ($global['approot'] . 'inc/lib_validate.inc');
require_once ($global['approot'] . 'inc/lib_config.inc');
//require_once ($global['approot'].'inc/lib_locale/handler_locale.inc');
shn_setup_main();
// main setup switchboard
function shn_setup_main() {
    $action = $_GET['act'];
    $modify = array();
    // display nice header
    shn_setup_header();
    switch ($action) {
        case 'dependancy': // first setup form
            if (shn_setup_check_dependancy()) { // check if all the dependencies are fullfill
                shn_setup_form1();
            } else {
                display_errors();
                shn_setup_form_dependancy();
            }
        break;
        case 'dbsetup': // second setup form
            if (shn_setup_database()) { // check if database writing is a success
                shn_setup_form2();
            } else {
                display_errors();
                shn_setup_form1();
            }
        break;
        case 'cfgsetup': // third setup form
            shn_email_validate(); // try to catch the invalid emailaddresses of "second settup form"
            //shn_chk_root_pwd();// check root passwd validation
            if (!shn_setup_acl()) {
                display_errors();
                shn_setup_form2();
                break;
            }
            if (shn_setup_config_update($_POST['conffile'])) {
                // check if writing the file was a success
                shn_setup_form_conffile();
            } else {
                display_errors();
                shn_setup_form2();
            }
        break;
        default:
            // first database setup form
            shn_setup_form_dependancy();
    }
    shn_setup_footer();
}
function shn_setup_check_dependancy() {
    global $global;
    $continue = true;
    if (!version_compare(phpversion(), '4.1', '>')) $continue = false;
    if (!extension_loaded('gd')) $continue = false;
    if (!extension_loaded('mysql')) $continue = false;
    if (!extension_loaded('gettext')) $continue = false;
    if (!is_writable($global['approot'] . 'www/tmp')) $continue = false;
    return $continue;
}
function shn_email_validate() {
    $email = $_POST['root_email'];
    if ((eregi("^[_a-z0-9-]+(\.[_a-z0-9-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$", $email)) || ($email == "")) {
        // if email is a valid one then do nothing ;-)
        
    } else {
        add_error("<b>$email</b> is an invalid email address! please check the 'Administrator Email' again.");
        display_errors();
        shn_setup_form2();
        break;
    }
} // end of shn_email_validate
/*
function shn_pwd_validate()
{
$pwd=$_POST['root_pwd'];
$re_pwd=$_POST['root_pwd_confirm'];
if($pwd==""){ // if password slot is empty
echo "<b><font color=red>Passwords</font> are empty!</b>";
echo "";
shn_setup_form2();
break;
} elseif (!($pwd==$re_pwd)) { // if password and retyped one are not matched
echo "<b><font color=red>Passwords</font> are not matched!</b>";
echo "";
shn_setup_form2();
break;
}
} // end of shn_root_pwd_validation
*/
function shn_setup_form_dependancy() {
    global $global;
    $isOK = true;
    $tick_img = '<img align="top" src="theme/default/img/tic.png" alt="tick"/>';
    $cross_img = '<img align="top" src="theme/default/img/cross.png" alt="cross"/>';
    $oops_img = '<img align="bottom" src="theme/default/img/oops.png" alt="oops"/>';
?>
<h1><?php echo _('Sahana Web Setup - Step 1') ?></h1>

<p><?php echo _('Welcome to Sahana. Please follow the following steps to setup the Sahana system') ?></p>

<p><?php echo _('First we need to check if all the relevant dependencies for the Sahana system to be install are available on your system') ?></p>
	<?php
    shn_form_fopen(_('dependancy'));
    shn_form_fsopen(_('PHP libraries'));
?>
<p><?php echo _('The following checks if all the relevant Sahana libraries are installed') ?></p>
	<?php
    $value = (version_compare(phpversion(), '4.1', '>'));
    $isOK&= $value;
    //if ($isOK) echo "is OK";
    if ($value) {
        shn_form_label(_('PHP Version'), $tick_img . phpversion() . " is installed");
    } else {
        shn_form_label(_('PHP Version'), $cross_img . phpversion() . " is installed");
        echo "<p><br/>$oops_img Sahana needs PHP version 4.1 or higher. Please upgrate your version of PHP</p>";
    }
    // Check if php-gd is loaded
    $value = (extension_loaded('gd'));
    $isOK&= $value;
    if ($value) {
        shn_form_label(_('PHP-GD Library'), $tick_img . "installed");
    } else {
        shn_form_label(_('PHP-GD Library'), $cross_img . "is not installed");
        echo "<p><br/>$oops_img Sahana needs PHP-GD for image manipulations. Please install PHP-GD and restart the webserver. To find out how to do this for your operating system see <a href=\"http://www.sahana.lk/wiki/doku.php?id=manual:install\">Sahana WIKI</a></p>";
    }
    // Check if php-gettext is loaded
    $value = (extension_loaded('gettext'));
    $isOK&= $value;
    if ($value) {
        shn_form_label(_('PHP-gettext Library'), $tick_img . "installed");
    } else {
        shn_form_label(_('PHP-gettext Library'), $cross_img . "is not installed");
        echo "<p><br/>$oops_img Sahana needs PHP-gettext for to support local languages. Please install PHP-GD and restart the webserver. To find out how to do this for your operating system see <a href=\"http://www.sahana.lk/wiki/doku.php?id=manual:install\">Sahana WIKI</a></p>";
    }
    $value = (extension_loaded('mysql'));
    $isOK&= $value;
    if ($value) {
        shn_form_label(_('MySQL library'), $tick_img . "installed");
    } else {
        shn_form_label(_('MySQL library'), $cross_img . "is not installed");
        echo "<p><br/>$oops_img Sahana needs PHP-MySQL library and MySQL for database operation. Please install PHP-MySQL library and MySQL and restart the webserver. To find out how to do this for your operating system see <a href=\"http://www.sahana.lk/wiki/doku.php?id=manual:install\">Sahana WIKI</a></p>";
    }
    shn_form_fsclose();
    shn_form_fsopen(_('File and Directory Permissions'));
?>
<p><?php echo _('The following checks if certain files and sub-directories have write permissions for Sahana to function,') ?></p>
	<?php
    $tmpWritable = is_writable($global['approot'] . 'www/tmp');
    $isOK&= $tmpWritable;
    if ($tmpWritable) {
        //shn_form_label(_('Sahana www/tmp/ is writable?'),"The Sahana sub-directory writable");
        shn_form_label(_('<i>sahana/www/tmp/</i>'), "is <strong><font color=green>writable</font></strong>!<br>");
    } else {
        //shn_form_label(_('Sahana www/tmp/ is writable?'),"The Sahana sub-directory not writable");
        shn_form_label(_('<i>sahana/www/tmp/</i>'), "is <strong><font color=red>not writable</font></strong>!<br>");
        //echo "<p><br/>$oops_img The directory ".$global['approot']."www/tmp needs to be writable by the web server at least. Please change the permissions of this directory and try again.</p>";
        
    }
    /* if ($value) {
    shn_form_label(_('Sahana www/tmp/ is writable?'), $tick_img."The Sahana sub-directory writable");
    } else {
    shn_form_label(_('Sahana www/tmp/ is writable?'), $cross_img."The Sahana sub-directory not writable");
    echo "<p><br/>$oops_img The directory ".$global['approot']."www/tmp needs to be writable by the web server at least. Please change the permissions of this directory and try again.</p>";
    }
    
    /*   $value = (is_writable($global['approot'].'res/locale'))? 'checked':'';
    
    shn_form_checkbox('Sahana res/locale is writable?','write_locale',$value.'
    disabled="true"',array('value'=>'y','help'=>'hello'));
    */
    $confWritable = is_writable($global['approot'] . 'conf');
    if ($confWritable) {
        //shn_form_label(_('Sahana www/conf/ is writable?'), $tick_img."The Sahana sub-directory writable");
        shn_form_label(_('<i>sahana/conf/</i>'), "is <strong><font color=green>writable</font></strong>!<br>");
    } else {
        //shn_form_label(_('Sahana www/conf/ is writable?'), "The Sahana sub-directory not writable");
        shn_form_label(_('<i>sahana/conf/</i>'), "is <strong><font color=red>not writable</font></strong>!<br>");
    }
    if (!$tmpWritable || !$confWritable) {
        $both = (!$tmpWritable && !$confWritable);
        echo "<p>Note: <font color=gray>" . (($both ? "both " : "")) . ((!$confWritable) ? "<strong>sahana/conf/</strong>" : "") . (($both) ? " and " : " ") . ((!$tmpWritable) ? "<strong>sahana/www/tmp/</strong>" : "") . " need" . ((!$both) ? "s" : "") . " to be writable by the web server at least. Please change the permissions of " . ((!$both) ? "this directory" : "these directories") . " and try again.</font></p>";
    }
    shn_form_fsclose();
    if ($isOK) {
        echo _('<p>All the dependencies are OK and you may move to
                the next step</p>') . "\n";
        shn_form_submit(_('Next >>'));
    } else {
        echo _('<p>Please correct the required dependencies above and</p>') . "\n";
        shn_form_submit(_('Try Again'));
    }
    shn_form_fclose();
}
// the database setup form
function shn_setup_form1() {
?>
<h2><?php echo _('Sahana Database Setup - Step 2') ?></h2>
	<?php
    shn_form_fopen('dbsetup');
    shn_form_fsopen(_('Database'));
    echo "<ul><li>";
    echo _("Either you can use an already created empty database with a user having access to that database. <br/><em>or</em>");
    echo "</li><li>" . _("You can create a new database and a user.");
    echo "</li></ul><br/>";
    shn_form_select(array('mysql' => 'MySQL 4.1 or above', 'pgsql"disabled="disabled' => 'PostgreSQL'), 'Database Types', 'dbtype');
    $engines['MyISAM'] = 'MyISAM';
    $engines['InnoDB'] = 'InnoDB';
    shn_form_select($engines, _("Engine Type"), 'engine_type', $extra_opts);
    shn_form_text(_('Database Host:Port'), 'dbhost', 'size="30"', array('value' => 'localhost', 'req' => true));
    shn_form_fsopen(_('Schemas to Install'));
    // please add somedescription,
    /*
    ?> <p><?php echo _('Check this to create the default categories for aid goods,units.')?></p> <?
    */
    shn_form_checkbox(_('Default Item Categories'), 'dbsample', 'checked', array('value' => 'y'));
    // this 'help' is to be removed :
    //, 'help'=>'Check this to
    //create the default categories for aid goods, units'));
    shn_form_fsclose();
    shn_form_fsopen(_("Use an already created database."));
    echo "<p><strong>" . _("Enter the following details if you are installing sahana on a hosted/shared web environment where you do not have root access to the database.") . "</strong><em>" . _("  * (Recomended for hosting environments)") . "</em></p><br/>";
    shn_form_text(_('Database Name'), 'ex_dbname', 'size="30"', array('req' => true));
    shn_form_text(_('Database User'), 'ex_dbuser', 'size="30"', array('value' => '', 'req' => true));
    $_POST['ex_dbpass'] = '';
    shn_form_password(_('Database Password'), 'ex_dbpass', 'size="30"', array('req' => true));
    shn_form_fsclose();
    shn_form_submit(_('Use Database >>'), "name='use_ex_db'");
    echo "<br/><hr/>";
    shn_form_fsopen(_("Create New Database"));
    echo "<p><strong>" . _("Enter the following details if you are running sahana on a server where you have root access to the mysql database.") . "</strong></p>";
    shn_form_text(_('Database Name'), 'dbname', 'size="30"', array('value' => 'sahana', 'req' => true));
    shn_form_text(_('Database User'), 'dbuser', 'size="30"', array('value' => 'root', 'req' => true));
    $_POST['dbpass'] = '';
    shn_form_password(_('Database Password'), 'dbpass', 'size="30"', array('req' => true));
    shn_form_text(_('System Database User'), 'shndbuser', 'size="30"', array('value' => 'sahana_sys', 'req' => true));
    shn_form_password(_('System Database Password'), 'shndbpass', 'size="30"', array('value' => 'password', 'req' => true));
    shn_form_fsclose();
    shn_form_submit(_('Setup Database >>'), "name='setup_db'");
    shn_form_fsclose();
    shn_form_fclose();
}
/**
 * Last changed: 28-May-2007 - ravindra@opensource.lk
 * Added the admin and normal user account fields
 *
 */
function shn_setup_form2() {
?>
<h1><?php echo _("Sahaha Web Setup - Step 3") ?>
</h2>
<p><?php echo _("Please follow the following steps to setup the Sahana configuration") ?></p>

	<?php
    echo PWDPOLICY . "<br />";
    global $global;
    shn_form_fopen('cfgsetup');
    // pass the previous post varibles into hidden for the configuration file
    shn_form_hidden(array('dbhost' => $_POST['dbhost'], 'dbname' => $_POST['dbname'], 'shndbuser' => $_POST['shndbuser'], 'shndbpass' => $_POST['shndbpass'], 'use_ex_db' => $_POST['use_ex_db'], 'ex_dbname' => $_POST['ex_dbname'], 'ex_dbuser' => $_POST['ex_dbuser'], 'ex_dbpass' => $_POST['ex_dbpass'], 'engine_type' => $_POST['engine_type']));
    shn_form_fsopen(_('Installation Details'));
    echo '<p>The following details are needed to uniquely identify your Sahana installation</p>'; // some spellings corrected by chamath 2007-03-15 09:50
    $help_txt = "The following field is randomly generated to help uniquely identify \
                 this Sahana installation. Please use this unless you have a \
                 convention defined";
    shn_form_text(_('Unique Installation ID'), 'base_uuid', 'size="4" maxlength="4"', array('value' => shn_generate_uuid(4), 'req' => true, 'help' => $help_txt));
    shn_form_text(_('Administrator Name'), 'root_name', 'size="40"');
    shn_form_text(_('Administrator Email'), 'root_email', 'size="40"');
    shn_form_text(_('Administrator Telephone'), 'root_tel', 'size="30"');
    shn_form_fsclose();
    shn_form_fsopen(_('Settings'));
    shn_form_select(array('disable' => _('Disable'), 'enable' => _('Enable')), _('Access Control'), 'acl');
    shn_form_select(array('default' => _('Default Theme'),), ('Theme'), 'theme');
    shn_form_fsclose();
    shn_form_fsopen(_('Administrator Account'));
    shn_form_text(_('Admin User Name'), 'admin_name', 'size="40"', array('req' => true, 'value' => 'admin', 'help' => 'The root user is the administrator
                of the system and has the greatest privilages'));
    $_POST['root_pwd'] = '';
    shn_form_password(_('Password'), 'root_pwd', 'size="30"', array('req' => true));
    $_POST['root_pwd_confirm'] = '';
    shn_form_password(_('Confirm Password'), 'root_pwd_confirm', 'size="30"', array('req' => true));
    shn_form_fsclose();
    shn_form_fsopen(_('Create Normal User Account'));
    shn_form_text(_('Normal User Name'), 'user_name', 'size="40"', array('req' => true, 'value' => 'sahana', 'help' => 'Please login as this user for non-admin work'));
    $_POST['user_pwd'] = '';
    shn_form_password(_('Password'), 'user_pwd', 'size="30"', array('req' => true));
    $_POST['user_pwd_confirm'] = '';
    shn_form_password(_('Confirm Password'), 'user_pwd_confirm', 'size="30"', array('req' => true));
    shn_form_fsclose();
    shn_form_fsopen(_('Configuration File'));
    if (is_writeable($global['approot'] . 'conf')) {
        echo _('<p>The next step is to write the configuration file sysconf.inc in the Sahana /conf directory.</p>');
        shn_form_hidden(array('conffile' => $global['approot'] . 'conf/sysconf.inc'));
    } else {
        echo _('<p>The PHP/Web Server should normally not have access to write
    or modify the configuration file located in Sahana application <b>/conf</b> 
    directory, as this is a security risk. Please specify where to write the temporary file (e.g. /tmp). Subsequently you need to move this file to 
    the Sahana <b>/conf</b> directory.</p>');
        $tmp_filename = tempnam('/tmp', 'sysconf.inc');
        // Need the temporary directory rather than the filename
        $tmp_filename = preg_replace('/sysconf.inc.*$/', 'sysconf.inc', $tmp_filename);
        shn_form_text(_('Location To Write Config File (sysconf.inc)'), 'conffile', 'size="50"', array('value' => "$tmp_filename", 'req' => true));
    }
    shn_form_fsclose();
    shn_form_submit(_('Write Configuration File >>'));
    shn_form_fclose();
}
/*
function shn_chk_root_pwd()
{
clean_errors();
// check whether the password is empty or not
if ($_POST['acl']=='enable'){

if ($_POST['root_pwd']==null) {
add_error("Root password can not be empty!");
display_errors();
shn_setup_form2();
break;
// if it's not empty then check whether those are matching
} elseif (!($_POST['root_pwd']==$_POST['root_pwd_confirm'])) {
add_error("Root passwords do not match! Please re-type the root password and try again.");
display_errors();
shn_setup_form2();
break;
}
}
} // end of shn_chk_root_pwd
*/
function shn_setup_form_conffile() {
    //shn_chk_root_pwd();
    
?>
<h1><?php echo _('Sahaha Web Setup - Step 4') ?></h1>
<p><?php echo _('Configuration file ') ?><?=$_POST['conffile'] ?> <?php echo _(' has been successfully written.') ?></p>

	<?php
    global $global;
    $oops_img = '<img align="bottom" src="theme/default/img/oops.png"/>';
    shn_form_fopen('default', 'admin', array('req_message' => false));
    shn_form_fsopen(_('Starting Sahana'));
    shn_form_hidden(array('conffile' => $_POST['conffile']));
    $sahana_url = 'http://' . $_SERVER['HTTP_HOST'] . preg_replace('/index.php\?.*$/', '', $_SERVER['REQUEST_URI']);
    if (is_writeable($global['approot'] . 'conf')) {
?>
<p><b><u><?php echo $oops_img . _('WARNING: Security') ?></u></b></p>
<p><?php echo _('To secure the configuration you have to remove write 
             permissions from the configuration file sysconf.inc located in 
             the Sahana directory conf/') ?></p>

<p><?php echo _('Click the button below to start Sahana') ?> <?php
    } else {
?>


<p><?php echo _('To finish the configuration follow the steps below') ?></p>
<p><b>1.</b><?php echo _(' Copy the temporary config file created from: ') ?><br />
<u><?=$_POST['conffile'] ?></u> to: <br />
<u><?=$global['approot'] . "conf/sysconf.inc" ?></u></p>
<p><b>2.</b><?php echo _(' For security reasons delete the temporary configuration file at ') ?><u><?=$_POST['conffile'] ?></u></p>
<p><b>3.</b><?php echo _(' Once done click the button below to start Sahana or access it anytime using the url: ') ?><a
	href="<?=$sahana_url ?>"><?=$sahana_url ?></a></p>

	<?php
    }
    shn_form_fsclose();
    shn_form_submit(_('Start Sahana'));
    shn_form_fclose();
    $_SESSION['first_time_run'] = true;
}
// connect to the database and run the scripts specified by the user
function shn_setup_database() {
    global $global;
    global $conf;
    $db = & NewADOConnection('mysql');
    $vals = null;
    $use_ex_db = false;
    $use_ex_db = (isset($_POST['use_ex_db']) && ($_POST['use_ex_db'] != '') && (strlen(trim($_POST['use_ex_db'])) > 0));
    $dbname = '';
    $sysdbuser = '';
    $sysdbpass = '';
    if ($_POST['dbtype'] == 'mysql') {
        // check host name
        if (isset($_POST['dbhost']) && $_POST['dbhost'] != '') {
            // check for patterns.
            if (stripos($_POST['dbhost'], ':') > 0) {
                // contains port
                $vals = explode(':', $_POST['dbhost']);
                if (preg_match('/^\d+$/', $vals[1]) == false) {
                    add_error(_("The port number is invalid."));
                    return false;
                }
            } else {
                $vals = array();
                $vals[0] = $_POST['dbhost'];
            }
        } else {
            add_error(_("The database host:port cannot be empty."));
            return false;
        }
        if ($use_ex_db == true) {
            // existing database name.
            $dbname = $_POST['ex_dbname'];
        } else {
            $dbname = $_POST['dbname'];
        }
        // check sahana dbname.
        if (isset($dbname) == false || $dbname == '') {
            add_error(_("The database name cannot be empty."));
            return false;
        } else {
            if (preg_match('/[!@#$%\^&*()\-\+=~`[\]{}\|\\\\:;"\',\.<>\?\/]/', $dbname)) {
                add_error(_("The database name cannot contain special characters."));
                return false;
            }
        }
        if ($use_ex_db == false) {
            // check whether user exists,
            $db->Connect($_POST['dbhost'], $_POST['dbuser'], $_POST['dbpass'], 'mysql');
            if ($db->IsConnected() == false) {
                add_error(_("Incorrect database username or password."));
                return false;
            }
            $res = $db->Execute("SELECT * FROM `user` WHERE `User`='" . $_POST['shndbuser'] . "'");
            if ($db->IsConnected() && (!($res->EOF))) {
                add_error(_("System database username already exists. Use a different system database username."));
                $db->Close();
                return false;
            }
            if (strlen($_POST['shndbuser']) >= 16) {
                add_error(_("The system database username is too long. Maximum length is 15."));
                return false;
            }
            $db->Connect($_POST['dbhost'], $_POST['dbuser'], $_POST['dbpass']);
            //$db->debug=true;
            // create system user and privileges
            $db->Execute("CREATE USER '" . $_POST['shndbuser'] . "'@'" . $vals[0] . "' IDENTIFIED BY '" . $_POST['shndbpass'] . "'");
            $db->Execute("GRANT SELECT,INSERT,UPDATE,DELETE,CREATE ON " . $_POST['dbname'] . ".* TO '" . $_POST['shndbuser'] . "'@'" . $vals[0] . "'");
            $db->Close();
        } else {
            // Check database existence.
            $db->Connect($_POST['dbhost'], $_POST['ex_dbuser'], $_POST['ex_dbpass'], $dbname);
            if ($db->IsConnected() == false) {
                add_error(_("Incorrect database name, username or password."));
                return false;
            }
        }
        //$tdb->Close();
        
    } else if ($_POST['dbtype'] == 'pgsql') {
        add_error(_('Unsupported database. Please select a valid database type.'));
        return false;
        // check whether user exists,
        //$tdb->Connect($_POST['dbhost'],$_POST['dbuser'],$_POST['dbpass'],'pgsql');
        //$res=$tdb->Execute("SELECT * FROM `user` WHERE `User`='".$_POST['shndbuser']."'");
        //if(!($res->EOF)){
        //	add_error(_("System database username already exists. Use a different system database username."));
        //	return false;
        // create system user and privileges
        //$db->Execute("CREATE USER ".$_POST['shndbuser']." WITH PASSWORD '".$_POST['shndbpass']."'");
        //$db->Execute("GRANT ALL PRIVILEGES ON ".$_POST['dbname']." TO ".$_POST['shndbuser']."@".$_POST['dbhost']);
        
    } else {
        add_error(_('Unsupported database. Please select a valid database type.'));
        return false;
    }
    if ($use_ex_db) {
        // check username for blank
        if (isset($_POST['ex_dbuser']) == false || strlen(trim($_POST['ex_dbuser'])) == 0) {
            add_error(_("The database username cannot be blank."));
            return false;
        }
        $sysdbuser = $_POST['ex_dbuser'];
        $sysdbpass = $_POST['ex_dbpass'];
    } else {
        $sysdbuser = $_POST['dbuser'];
        $sysdbpass = $_POST['dbpass'];
    }
    if ($use_ex_db == false) {
        // create database.
        if (!$db->Connect($_POST['dbhost'], $_POST['dbuser'], $_POST['dbpass'])) {
            add_error(_('Could not connect to the database. Please check the setting and try again'));
            return false;
        }
        // create the main database
        $db->Execute("CREATE DATABASE IF NOT EXISTS " . $_POST['dbname']);
    }
    $engine_type = $_POST['engine_type'];
    // connect
    if (!$db->Connect($dbhost, $sysdbuser, $sysdbpass, $dbname)) {
        add_error(_('Could not connect to the database. Please check the setting and try again'));
        return false;
    }
    $global['db'] = $db;
    $conf['db_engine'] = $_POST['dbtype'];
    if ($conf['db_engine'] == 'mysql') {
        $db->Execute("set storage_engine ='{$engine_type}'");
    }
    $conf['storage_engine'] = $engine_type;
    // setup the choosen sql scripts
    shn_setup_run_sqlscript($db, 'inst/mysql-dbcreate.sql');
    shn_setup_run_sqlscript($db, 'inst/mysql-config.sql');
    shn_setup_run_sqlscript($db, 'mod/cs/inst/dbcreate.sql');
    shn_setup_run_sqlscript($db, 'mod/rms/inst/dbcreate.sql');
    shn_setup_run_sqlscript($db, 'mod/gis/dbcreate.sql');
    shn_setup_run_sqlscript($db, 'mod/vm/inst/dbcreate.sql'); // for Volunteer management
    shn_setup_run_sqlscript($db, 'mod/ims/inst/dbcreate.sql');
    shn_setup_run_sqlscript($db, 'mod/msg/inst/dbcreate.sql'); // patched by SriGaneshan 2007-03-11 23:42
    shn_setup_run_sqlscript($db, 'mod/rs/inst/dbcreate.sql');
    shn_setup_run_sqlscript($db, 'mod/hr/inst/dbcreate.sql'); //hr
    if ($_POST['dbsample'] == 'y') {
        include_once ($global['approot'] . 'inc/lib_uuid.inc');
        shn_setup_run_sqlscript($db, 'inst/mysql-sampledata.sql');
        include ($global['approot'] . 'mod/cs/inst/setup.inc');
        shn_cs_datacreate();
    }
    //modified by dinusha
    $db->Execute("insert into incident(incident_id,name) values(1,'default')");
    $db->Execute("insert into user_preference values(0,'ims','incident_id',1)");
    $db->Execute("insert into user_preference values(2,'ims','incident_id',1)");
    //end
    return true;
}
// runs the script file relative to the approot
function shn_setup_run_sqlscript($db, $relative_script_path) {
    global $global;
    $script = $global['approot'] . $relative_script_path;
    if (file_exists($script) && $fh = fopen($script, 'r')) {
        $sql = ''; // clear the bucket
        while (!feof($fh)) { // a fix as Execute gets confused over carriage returns
            $buffer = fgets($fh, 1024); // read the script and fill the buffer
            $sql = $sql . $buffer; // fill the buffer until ';' appear
            if (preg_match('/;/', $buffer)) { // look after ';'
                $db->Execute($sql);
                $sql = ''; // clear the bucket again
                
            }
        }
        //$sql = fread($fh,filesize($script));
        fclose($fh);
    }
    // @todo error handler if file does not exist
    
}
function shn_setup_config_update($tmp_configfile) {
    global $global;
    //echo isset($_POST['use_ex_db'])?$_POST['ex_dbuser']:$_POST['shndbuser'];
    $opt_test = !(isset($_POST['use_ex_db']) && ($_POST['use_ex_db'] == '' || strlen(trim($_POST['use_ex_db'])) == 0));
    //var_dump(($opt_test)?$_POST['ex_dbuser']:$_POST['shndbuser']);
    $modify = array('/\'theme\'/' => '$conf[\'theme\'] = \'' . $_POST['theme'] . '\';', '/\'db_host\'/' => '$conf[\'db_host\'] = \'' . $_POST['dbhost'] . '\';', '/\'db_name\'/' => '$conf[\'db_name\'] = \'' . (($opt_test) ? $_POST['ex_dbname'] : $_POST['dbname']) . '\';', '/\'base_uuid\'/' => '$conf[\'base_uuid\'] = \'' . $_POST['base_uuid'] . '\';', '/\'root_name\'/' => '$conf[\'root_name\'] = \'' . $_POST['root_name'] . '\';', '/\'root_tel\'/' => '$conf[\'root_tel\'] = \'' . $_POST['root_tel'] . '\';', '/\'root_email\'/' => '$conf[\'root_email\'] = \'' . $_POST['root_email'] . '\';', '/\'db_user\'/' => '$conf[\'db_user\'] = \'' . (($opt_test) ? $_POST['ex_dbuser'] : $_POST['shndbuser']) . '\';', '/\'db_pass\'/' => '$conf[\'db_pass\'] = \'' . (($opt_test) ? $_POST['ex_dbpass'] : $_POST['shndbpass']) . '\';', '/\'storage_engine\'/' => '$conf[\'storage_engine\'] = \'' . $_POST['engine_type'] . '\';');
    if (@shn_config_file_update($modify, $global['approot'] . 'conf/sysconf.inc.tpl.php', $tmp_configfile)) {
        return true;
    } else {
        add_error(_('Could not write to file $tmp_configfile. Try another location'));
        return false;
    }
}
function shn_setup_header() {
?>
<body>
<div id="container"><?php
    shn_include_page_section('header', $module);
?>
<div id="wrapper" class="clearfix">
<div id="content" class="clearfix"><?php
}
function shn_setup_footer() {
?></div>
<!-- /content --> <?php
    shn_include_page_section('footer', $module);
?></div>
<!-- /wrapper --></div>
<!-- /container -->
</body>
</html>
<?php
}
// install the ACL base
function shn_setup_acl() {
    global $global;
    $db = & NewADOConnection('mysql');
    $use_ex_db = false;
    $use_ex_db = (isset($_POST['use_ex_db']) && $_POST['use_ex_db'] != '' && strlen(trim($_POST['use_ex_db'])) > 0);
    $dbname = '';
    $sysdbuser = '';
    $sysdbpass = '';
    if ($use_ex_db) {
        // existing database name.
        $dbname = $_POST['ex_dbname'];
        $sysdbuser = $_POST['ex_dbuser'];
        $sysdbpass = $_POST['ex_dbpass'];
    } else {
        $dbname = $_POST['dbname'];
        $sysdbuser = $_POST['shndbuser'];
        $sysdbpass = $_POST['shndbpass'];
    }
    $db->Connect($_POST['dbhost'], $sysdbuser, $sysdbpass, $dbname);
    $global['db'] = $db;
    $error = false;
    if ((null == $_POST["admin_name"]) or (is_null($_POST["admin_name"]))) {
        $error = true;
        add_error(_("Admin user name cannot be empty"));
    }
    if ((null == $_POST["root_pwd"]) or (is_null($_POST["root_pwd"]))) {
        $error = true;
        add_error(_("Root password cannot be empty"));
    } else {
        $re_password = trim($_POST['root_pwd_confirm']);
    }
    if ((null == $_POST["root_pwd_confirm"]) or (is_null($_POST["root_pwd_confirm"]))) {
        $error = true;
        add_error(_("Root Confirm password cannot be empty"));
    } else {
        $re_password = trim($_POST['root_pwd_confirm']);
    }
    if (!($_POST['root_pwd'] == $re_password)) {
        $error = true;
        add_error("Root password and confirm password should match");
    }
    if ((null == $_POST["user_name"]) or (is_null($_POST["user_name"]))) {
        $error = true;
        add_error(_("Normal user name cannot be empty"));
    }
    if ((null == $_POST["user_pwd"]) or (is_null($_POST["user_pwd"]))) {
        $error = true;
        add_error(_("User password cannot be empty"));
    } else {
        $password = trim($_POST['user_pwd']);
    }
    if ((null == $_POST["user_pwd_confirm"]) or (is_null($_POST["user_pwd_confirm"]))) {
        $error = true;
        add_error(_("User Confirm password cannot be empty"));
    } else {
        $re_password = trim($_POST['user_pwd_confirm']);
    }
    if (!($_POST['user_pwd'] == $re_password)) {
        $error = true;
        add_error("User password and confirm password should match");
    }
    if ($error) {
        return false;
    }
    if (shn_validate_password($_POST["admin_name"], $_POST["root_pwd"]) == false) {
        $error = true;
        return false;
    }
    if (shn_validate_password($_POST["user_name"], $_POST["user_pwd"]) == false) {
        $error = true;
        return false;
    }
    global $global;
    include_once $global['approot'] . 'inc/lib_security/lib_acl.inc';
    return (shn_acl_install($_POST['admin_name'], $_POST['root_pwd'], $_POST['user_name'], $_POST['user_pwd'], $_POST['acl'] == 'enable'));
}
?>