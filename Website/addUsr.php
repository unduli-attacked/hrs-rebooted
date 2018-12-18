<?php
    //junk
    $poths = "/home/hrs/public_html/";
    $hospital = $_COOKIE['hospital'];
    $lgd = 0;
    $error = 0;
    
    if(isset($_COOKIE['logged'])){
    	$lgd = 1;
    }

    
    if(isset($_POST['subAuths'])){
    	$usr = $_POST['usrName'];
    	$pass = $_POST['usrPass'];
    	
    	$passH = hash('sha512', $pass);
    	
    	if(file_exists($poths.$hospital."/usrs/".$usr)){
    		$passes = fopen($poths.$hospital."/usrs/".$usr, "r");
    		$grabedPass = fread($passes, filesize($poths.$hospital."/usrs/".$usr));
    		fclose($passes);
    		if($grabedPass == $passH){
    			setcookie('logged', "1" , time() + 1800); //expires in 30 mins
    			echo "Logged in";
    			$lgd = 1;
    		}else{
    			echo "Invalid login info";
    		}
    	}else{
    		echo "User not found";
    	}
    	
    }
    
    if(isset($_POST['loogout'])){
    	setcookie('logged', '0', time()-10); //expires 10 seconds ago, deletes cookie
    	echo "Logged out";
    	$lgd = 0;
    	//echo "<button type='button'><a href='http://hrs.heliohost.org/admin.php'>Ok</a></button>";
    }

    
    if(isset($_POST['subNewUsr'])){
        $nPass1 = $_POST['newPass1'];
        $nPass2 = $_POST['newPass2'];
        $nUsr = $_POST['newUsr'];
        $nPassH = hash('sha512', $nPass1);

        if($nPass1 == $nPass2){
            if(file_exists($poths.$hospital."/usrs/".$nUsr)){
            	$dupCheck = fopen($poths.$hospital."/usrs/".$nUsr, "r");
            	$dupPass = fread($dupCheck,  filesize($poths.$hospital."/usrs/".$nUsr));
            	fclose($dupCheck);
            	if($dupPass == $nPassH){
            		echo "User already exists.";
            		$error = 1;
            	}else{
            		echo "Username is taken.";
            		$error = 1;
            	}
            }else{
            $newUser = fopen($poths.$hospital."/usrs/".$nUsr, "w");
            fwrite($newUser, $nPassH);
            fclose($newUser);
            echo $nUsr." Registered";
            $error = 0;
        }
        }else{
            echo "Passwords do not match";
            $error = 1;
        }
    }
?>
<!DOCTYPE HTML>
<html>
    <head>
        <link type="text/css" rel="stylesheet" href="/home/hrs/public_html/style.css" />
        <link rel="shortcut icon" href="favicon.ico" type="image/x-icon"/>
        <title><?php echo $_COOKIE['hospital']; ?> - HRS Admin</title>    </head>
    
    <body>
        <?php if($lgd == 1): ?>
            <h2>Add A New User to <?php echo $_COOKIE['hospital']; ?></h2>
            <form id="new" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post" name="new">
                <p>New Username:</p> <input type="text" name="newUsr" value="<?php if($error==1){ echo $_POST['newUsr']; } ?>"><br>
                <p>New Password:</p> <input type="password" name="newPass1"><br>
                <p>Confirm New Password:</p> <input type="password" name="newPass2"><br>
                <input type="submit" name="subNewUsr" value="Submit"><br>
            </form>
            
            <form id='logout' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='logout'>
	       		<input type='submit' name="loogout" value="Log Out"><br>
	    </form>
        <?php else: ?>
            <h2>Enter Your Login Information for <?php echo $_COOKIE['hospital']; ?></h2>
            <form id="auth" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post" name="auth">
                Username: <input type="text" name="usrName"><br>
                Password: <input type="password" name="usrPass"><br>
                <input type="submit" name="subAuths" value="Submit"><br>
            </form>
        <?php endif ?>
        
        <form id="form2" action="/index.php" method="POST" name="home">
		<input type="submit" name="goHome" value="Return to the Homepage"><br>
	</form>
    </body>
</html>