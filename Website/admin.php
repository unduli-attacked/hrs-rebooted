<?php
    //define path params
    $poths = "/home/hrs/public_html/";
    $dir=$_COOKIE['room'];
    $prevDir=$_COOKIE['hospital'];
    $path=$poths.$prevDir.'/'.$dir.'/';
    $des = "null";
    $lgd = 0;
    
    if(isset($_COOKIE['logged'])){
    	$lgd = 1;
    }
    
    if(isset($_POST['subAuths'])){
    	$user = $_POST['usrName'];
    	$pass = $_POST['usrPass'];
    	
    	$usrH = $user;
    	$passH = hash('sha512', $pass);
    	
    	if(file_exists($poths.$prevDir."/usrs/".$usrH)){
    		$passes = fopen($poths.$prevDir."/usrs/".$usrH, "r");
    		$grabedPass = fread($passes, filesize($poths.$prevDir."/usrs/".$usrH));
    		fclose($passes);
    		if($grabedPass == $passH){
    			setcookie('logged', '1', time() + 1800); //expires in 30 mins
    			$lgd = 1;
    			echo "Logged in";
    			//echo "<button type='button'><a href='http://hrs.heliohost.org/admin.php'>See options</a></button>";
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
    //set the des and data (1 or 0) based on what button was pressed
    if(isset($_POST['disAutoCol'])){
        $des='autoCol';
        $bool=0;
    }else if(isset($_POST['disAutoBright'])){
        $des='autoBright';
        $bool=0;
    }else if(isset($_POST['disManBright'])){
        $des='manBright';
        $bool=0;
    }else if(isset($_POST['disManCol'])){
        $des='manCol';
        $bool=0;
    }else if(isset($_POST['disAutoTemp'])){
        $des='autoTemp';
        $bool=0;
    }else if(isset($_POST['enAutoCol'])){
        $des='autoCol';
        $bool=1;
    }else if(isset($_POST['enAutoBright'])){
        $des='autoBright';
        $bool=1;
    }else if(isset($_POST['enManBright'])){
        $des='manBright';
        $bool=1;
    }else if(isset($_POST['enManCol'])){
        $des='manCol';
        $bool=1;
    }else if(isset($_POST['enAutoTemp'])){
        $des='autoTemp';
        $bool=1;
    }else if(isset($_POST['enAud'])){
        $des='aud';
        $bool=1;
    }else if(isset($_POST['disAud'])){
        $des='aud';
        $bool=0;
    }else if(isset($_POST['enVid'])){
        $des='vid';
        $bool=1;
    }else if(isset($_POST['disVid'])){
        $des='vid';
        $bool=0;
    }
    
    //open, write to, and close file
    $file=fopen($path.$des.'Admin', 'w');
    fwrite($file, $bool);
    fclose($file);
    
    
    function getData($location, $preset) {
               $poths = "/home/hrs/public_html/";
               $rm = $_COOKIE['room'];
               $hos = $_COOKIE['hospital'];
               $des=$location;     //set the destination to the inputed location
               $full=$poths.$hos."/".$rm."/".$des;     //encode the full path and destination into a variable
               //check if the file exists
               if(file_exists($full)){
                       $file=fopen($full, "r");     //open the file in read mode
                       $data=fread($file, filesize($full));     //read the entire file and encode it in a variable
                       fclose($file);     //close the file
                       return $data;     //return the data in the form of a text line
               }else{
                       return $preset;     //return the preset in the form of a text line
               }
        }
	
   function checkEn($in, $name){
	   if('1'==getData($path.$in."Admin", 1)){
	   	echo $name." Enabled:";
	   }else{
	   	echo $name." Disabled:";
	   }
   }
?>


<html>
    <head>
        <link type="text/css" rel="stylesheet" href="/home/hrs/public_html/style.css" />
        <link rel="shortcut icon" href="/home/hrs/public_html/favicon.ico" type="image/x-icon"/>
        <title>Room <?php echo $_COOKIE['room']; ?> - <?php echo $_COOKIE['hospital']; ?> - HRS Admin</title>
    </head>
    
    <body>
        <h1>Welcome to room <?php echo $_COOKIE['room']; ?> of <?php echo $_COOKIE['hospital']; ?></h1>
        
        <h3>Room Administration</h3>
        
        <?php if($lgd == 1): ?>
        	<!---check if the value is enabled or disabled, and display the correct button to change it-->
        	
        	<strong><?php echo checkEn("manCol", "Manual Colors"); ?></strong>
        	<p>The manual color control on the room page.</p>
	        <form id='prefs' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='prefs4'>
	                <input type='submit' name='disManCol' value='Disable'><br>
	                <input type='submit' name='enManCol' value='Enable'><br>
	        </form>
        	
        	<strong><?php echo checkEn("autoCol", "Automatic Colors"); ?></strong>
        	<p>The automatic color changes based upon the mood of the patient.</p>
	        <form id='prefs' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='prefs1'>
	                <input type='submit' name='disAutoCol' value='Disable'><br>
	                <input type='submit' name='enAutoCol' value='Enable'><br>
	        </form>
	        
	        <strong><?php echo checkEn("autoBright", "Automatic Brightness"); ?></strong>
	        <p>The automatic brightness changes based upon the time of day.</p>
	        <form id='prefs' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='prefs2'>
	                <input type='submit' name='disAutoBright' value='Disable'><br>
	                <input type='submit' name='enAutoBright' value='Enable'><br>
	        </form>
	        
	        <strong><?php echo checkEn("manBright", "Manual Brightness"); ?></strong>
	        <p>The manual brightness control on the room page.</p>
	        <form id='prefs' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='prefs3'>
	                <input type='submit' name='disManBright' value='Disable'><br>
	                <input type='submit' name='enManBright' value='Enable'><br>
	        </form>
	        
	        <strong><?php echo checkEn("autoTemp", "Automatic Temperature"); ?></strong>
	        <p>The automatic temperature changes based upon the skin temperature of the patient.</p>
	        <form id='prefs' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='prefs5'>
	                <input type='submit' name='disAutoTemp' value='Disable'><br>
	                <input type='submit' name='enAutoTemp' value='Enable'><br>
	        </form>
	        
	        <strong><?php echo checkEn("aud", "Audio"); ?></strong>
	        <p>The in-room audio system.</p>
	        <form id='prefs' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='prefs6'>
	                <input type='submit' name='disAud' value='Disable'><br>
	                <input type='submit' name='enAud' value='Enable'><br>
	        </form>
	        
	        <strong><?php echo checkEn("vid", "Video"); ?></strong>
	        <p>The in-room video system.</p>
	        <form id='prefs' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='prefs7'>
	                <input type='submit' name='disVid' value='Disable'><br>
	                <input type='submit' name='enVid' value='Enable'><br>
	        </form>
	       
	       
	        <form id='logout' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='logout'>
	       		<input type='submit' name="loogout" value="Log Out"><br>
	        </form>
        	
        <?php else: ?>
	        <form id='authIn' action="<?php echo $_SERVER['PHP_SELF']; ?>" method='post' name='authIn'>
        		Username: <input type="text" name="usrName"><br>
        		Password: <input type="password" name="usrPass"><br>
        		<input type="submit" name="subAuths" value="Submit"><br>
        	</form>
    	<?php endif ?>
    	
    	<form id="retRm" action="/room.php" method="post" name="retRm">
    		<input type="submit" name="retRoom" value="Return to Room Page"><br>
    	</form>
    	
    	<form id="newAd" action="/addUsr.php" method="POST" name="newAd">
            	<input type="submit" name="subNAd" value="Register a New Admin">
        </form>
        
        <form id="form2" action="/assignChip.php" method="POST" name="chip">
		<input type="submit" name="assignChip" value="Register a Chip"><br>
	</form>
    </body>
</html>