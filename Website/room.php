<?php
//create room
   if(isset($_POST['subRm'])){
        //def path params
        $hospital = $_COOKIE['hospital'];
        $poth = "/home/hrs/public_html";
        $poths = "/home/hrs/public_html/";
        $room=$_POST['rmSub'];
        setcookie("room", $room, 0, "/");
        //def fullPath
        $fullPath=$poths.$hospital."/";
        //check if the room has already been registered
        if(file_exists($fullPath.$room)){
            echo "This room has already been registered";
        }else{
            //create the room dir
            mkdir($fullPath.$room);
            //copy add file
            copy($poths.$hospital."/exRm/add.php", $poths.$hospital."/".$room."/add.php");
            //create bpm data file
            $beat = fopen($fullPath.$room."/bpm", "w");
            fclose($beat);
            //create temp data file
            $temp = fopen($fullPath.$room."/temp", "w");
            fclose($temp);
            //return a line stating room registration
            echo "Room ".$room." Registered";
        }
        
        $rm = $room;
        
        
        $timezone = $_POST['tz'];
	$go = fopen($poths.$hospital.'/'.$rm."/timezone", "w");     //open the chosen file using the path params and defined destination
       	fwrite($go, $timezone);     //write the data to the file, deleting all previous data from the file
       	fclose($go);     //close the file
       	//echo $timezone;
    }else{
    	$rm = $_COOKIE['room'];
    }
    
//room functions
       //path paramater definition
       $poths = "/home/hrs/public_html/";
       $hos = $_COOKIE['hospital'];
       $des='null';     //set $des to null for first run to prevent errors
       //define the destination and data based upon what submit button was pressed
       if(isset($_POST['submit'])){
               $toSend=$_POST['color'];
               $des="color";
       }elseif(isset($_POST['autoColOn'])){
               $toSend='on';
               $des='autoColor';
       }elseif(isset($_POST['autoColOff'])){
               $toSend="off";
               $des="autoColor";
       }elseif(isset($_POST['submitAge'])){
               $toSend=$_POST['manAge'];
               $des="age";
       }elseif(isset($_POST['autoBrightOn'])){
               $toSend="on";
               $des="autoBright";
       }elseif(isset($_POST['autoBrightOff'])){
               $toSend="off";
               $des="autoBright";
       }elseif(isset($_POST['submitBright'])){
               $toSend=$_POST['manualBright'];
               $des="brightness";
       }elseif(isset($_POST['enAAud'])){
               $toSend=1;
               $des="autoAud";
       }elseif(isset($_POST['disAAud'])){
               $toSend=0;
               $des="autoAud";
       }elseif(isset($_POST['subTAud'])){
               $toSend=$_POST['selTAud'];
               $des="audTrack";
       }elseif(isset($_POST['audOn'])){
               $toSend=1;
               $des="audio";
       }elseif(isset($_POST['audOff'])){
               $toSend=0;
               $des="audio";
       }elseif(isset($_POST['enAVid'])){
               $toSend=1;
               $des="autoVid";
       }elseif(isset($_POST['disAVid'])){
               $toSend=0;
               $des="autoVid";
       }elseif(isset($_POST['subSVid'])){
               $toSend=$_POST['selSVid'];
               $des="vidScene";
       }elseif(isset($_POST['vidOn'])){
               $toSend=1;
               $des="video";
       }elseif(isset($_POST['vidOff'])){
               $toSend=0;
               $des="video";
       }

       $file=fopen($poths.$hos.'/'.$rm."/".$des, "w");     //open the chosen file using the path params and defined destination
       fwrite($file, $toSend);     //write the data to the file, deleting all previous data from the file
       fclose($file);     //close the file
       
       
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
                       echo $data;     //return the data in the form of a text line
               }else{
                       echo $preset;     //return the preset in the form of a text line
               }
        }
        
        function getQuietData($location, $preset) {
               //redefine the path params as a safety net
               $poths = "/home/hrs/public_html/";
               $rm = $_COOKIE['room'];
               $hos = $_COOKIE['hospital'];
               $des=$location;   //set the destination to the inputed location
               $full=$poths.$hos."/".$rm."/".$des;   //encode the full path in a variable
               //check if the file exists
               if(file_exists($full)){
                       $file=fopen($full, "r");   //open the file in read mode
                       $data=fread($file, filesize($full));   //read the entire file
                       fclose($file);  //close the file
                       return $data;   //return the data without printing a text line
               }else{
                       return $preset;   //return the preset without printing a text line
               }
        }
        
        
        function getEn($location, $preset){
        	if(getQuietData($location, $preset)==1){
        		echo "Enabled";
        	}else{
        		echo "Disabled";
        	}
        }
?>

<html>
	<head>
		<link type="text/css" rel="stylesheet" href="/home/hrs/public_html/style.css" />
		<link rel="shortcut icon" href="/home/teamcosmo/public_html/favicon.ico" type="image/x-icon"/>
		<title>Room <?php echo $rm; ?> - <?php echo $_COOKIE['hospital']; ?> - HRS Controller</title>
		
	</head>
	
	<body>		
		


		
		<h1>Welcome to room <?php echo $rm; ?> of <?php echo $_COOKIE['hospital']; ?></h1>
		
		<h2>HRS Controller</h2>
		
		
	
		<script src="jscolor.js"></script>
                
                
        <!---Manual Colors Hex Input-->
        <?php if(getQuietData('manColAdmin', 1)==1): ?>       
            <form id="form" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="POST" name="form">      
                <h3>Light Color:</h3>

                <input id="colorIn" class="jscolor" name="color" placeholder="Enter color" type="text" value="<?php getData('color', 'ffffff'); ?>">
                <input type="submit" name="submit" value="Submit Color"><br>
            </form>
        <?php else: ?>
        	<h5>Manual colors disabled. Contact your administrator for more information.</h5>
        <?php endif ?>
        
                <!---Automatic Colors Toggle-->
                <?php if(getQuietData('autoColAdmin', 1)==1): ?>
                    <h3>Automatic Colors: </h3>
                    <!---two submit buttons, one for on one for off-->
                    <form id="form" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="POST" name="formTwo">
                            <input type="submit" name="autoColOn" value="On">
                            <input type="submit" name="autoColOff" value="Off"><br>
                    </form>
                    
                    <!---Age Input Box-->
                    <?php if(getQuietData('autoColor', 'on')=='on'): ?>
                            <h4>Age:</h4> 
                            <!---simple text input-->
                            <form id="form" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="POST" name="formFive">
                                    <input type="text" id="ageIn" name="manAge" value="<?php getData('age', '20' ); ?>">
                                    <input type="submit" name="submitAge" value="Submit Age"><br>
                            </form>
                    <?php endif ?>
                <?php else: ?>
        	    <h5>Automatic colors disabled. Contact your administrator for more information.</h5>
                <?php endif ?>
                        
                <!---Automatic Brightness Toggle-->
                <?php if(getQuietData('autoBrightAdmin', 1)==1): ?>
                    <h3>Automatic Brightness: </h3>
                    <!---two buttons, one for on, one for off-->
                    <form id="form" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="POST" name="formThree">
                            <input type="submit" name="autoBrightOn" value="On">
                            <input type="submit" name="autoBrightOff" value="Off">

                    </form>
                <?php else: ?>
        	    <h5>Automatic brightness disabled. Contact your administrator for more information.</h5>
                <?php endif ?>  
                
                <!---Manual Brightness Slider-->
                <?php if(getQuietData('manBrightAdmin', 1)==1): ?>
                    <?php if(getQuietData('autoBright', 'on')=='off'): ?>
                            <h4>Manual Brightness:</h4> 
                            <!---slider from 0 to 255-->
                            <form id="form" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="POST" name="formFour">
                                    Off<input type="range" id="brightIn" name="manualBright" value="<?php getData('brightness', 255); ?>" min="0" max="255">Full
                                    <input type="submit" name="submitBright" value="Submit Brightness"><br>
                            </form>
                    <?php endif ?>
                <?php else: ?>
        	    <h5>Manual brightness disabled. Contact your administrator for more information.</h5>
                <?php endif ?>
                
                <?php if(getQuietData('audAdmin', 1)==1): ?>
                	<h3>Relaxation Audio</h3>
                	<h4>Automatic <?php getEn("autoAud", 1); ?></h4>
                	<form id="aAud" action="room.php" method="POST" name="aAud">
                		<input type="submit" name="enAAud" value="Enable">
                		<input type="submit" name="disAAud" value="Disable">
                	</form>
                	<h4>Select Track</h4>
                	<form id="tAud" action="room.php" method="POST" name="tAud">
                		<select name="selTAud">
                			<?php if(getQuietData("audTrack","ocean")=="ocean"): ?>
                				<option value="ocean" selected>Ocean</option>
                				<option value="city">City</option>
                				<option value="forest">Forest</option>
                				<option value="water">Water</option>
                				<option value="wind">Wind</option>
                			<?php elseif(getQuietData("audTrack", "ocean")=="city"): ?>
                				<option value="ocean">Ocean</option>
                				<option value="city" selected>City</option>
                				<option value="forest">Forest</option>
                				<option value="water">Water</option>
                				<option value="wind">Wind</option>
                			<?php elseif(getQuietData("audTrack", "ocean")=="forest"): ?>
                				<option value="ocean">Ocean</option>
                				<option value="city">City</option>
                				<option value="forest" selected>Forest</option>
                				<option value="water">Water</option>
                				<option value="wind">Wind</option>
                			<?php elseif(getQuietData("audTrack", "ocean")=="water"): ?>
                				<option value="ocean">Ocean</option>
                				<option value="city">City</option>
                				<option value="forest">Forest</option>
                				<option value="water" selected>Water</option>
                				<option value="wind">Wind</option>
                			<?php elseif(getQuietData("audTrack","ocean")=="wind"): ?>
                				<option value="ocean">Ocean</option>
                				<option value="city">City</option>
                				<option value="forest">Forest</option>
                				<option value="water">Water</option>
                				<option value="wind" selected>Wind</option>
                			<?php endif ?>
                			
                		</select>
                		<input type="submit" name="subTAud" value="Submit">
                	</form>
                	<h4>Manual Control</h4>
                	<form id="manAud" action="room.php" method="POST" name="manAud">
                		<input type="submit" name="audOn" value="Turn On">
                		<input type="submit" name="audOff" value="Turn Off">
                	</form>
                <?php else: ?>
                	<h5>Relaxation audio disabled. Contact your administrator for more information.</h5>
                <?php endif ?>
                
                <?php if(getQuietData('vidAdmin', 1)==1): ?>
                	<h3>Relaxation Video</h3>
                	<h4>Automatic <?php getEn("autoVid", 1); ?></h4>
                	<form id="aVid" action="room.php" method="POST" name="aVid">
                		<input type="submit" name="enAVid" value="Enable">
                		<input type="submit" name="disAVid" value="Disable">
                	</form>
                	<h4>Select Scene</h4>
                	<form id="sVid" action="room.php" method="POST" name="sVid">
                		<select name="selSVid">
                			<?php if(getQuietData("vidScene","ocean")=="ocean"): ?>
                				<option value="ocean" selected>Ocean</option>
                				<option value="city">City</option>
                				<option value="sunset">Sunset</option>
                				<option value="clouds">Clouds</option>
                				<option value="wind">Puppies</option>
                			<?php elseif(getQuietData("vidScene", "ocean")=="city"): ?>
                				<option value="ocean">Ocean</option>
                				<option value="city" selected>City</option>
                				<option value="sunset">Sunset</option>
                				<option value="clouds">Clouds</option>
                				<option value="wind">Puppies</option>
                			<?php elseif(getQuietData("vidScene", "ocean")=="sunset"): ?>
                				<option value="ocean">Ocean</option>
                				<option value="city">City</option>
                				<option value="sunset" selected>Sunset</option>
                				<option value="clouds">Clouds</option>
                				<option value="wind">Puppies</option>
                			<?php elseif(getQuietData("vidScene", "ocean")=="clouds"): ?>
                				<option value="ocean">Ocean</option>
                				<option value="city">City</option>
                				<option value="sunset">Sunset</option>
                				<option value="clouds" selected>Clouds</option>
                				<option value="wind">Puppies</option>
                			<?php elseif(getQuietData("vidScene","ocean")=="puppies"): ?>
                				<option value="ocean">Ocean</option>
                				<option value="city">City</option>
                				<option value="sunset">Sunset</option>
                				<option value="clouds">Clouds</option>
                				<option value="wind" selected>Puppies</option>
                			<?php endif ?>
                			
                		</select>
                		<input type="submit" name="subSVid" value="Submit">
                	</form>
                	<h4>Manual Control</h4>
                	<form id="manVid" action="room.php" method="POST" name="manVid">
                		<input type="submit" name="vidOn" value="Turn On">
                		<input type="submit" name="vidOff" value="Turn Off">
                	</form>
                <?php else: ?>
                	<h5>Relaxation video disabled. Contact your administrator for more information.</h5>
                <?php endif ?>
                
                <form id="form" action="/admin.php" method="POST" name="adminForm">
                	<input type="submit" name="goAdmin" value="Click Here For Admin"><br>
                </form>
                
                
                        
                                


		<script>
		function setTextColor(picker) {
			document.POSTElementsByTagName('body')[0].style.color = '#' + picker.toString()
		}
		</script>
	</body>
</html>
