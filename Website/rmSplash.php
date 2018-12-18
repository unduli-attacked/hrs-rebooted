<?php
    if(isset($_POST['subHos'])){
    	$poths = "/home/hrs/public_html/";
        //def path params
        $hospital=$_POST['hosSub'];
        setcookie("hospital", $hospital, 0, '/');
        $hosName = $poths.$hospital."/";
        $masterUsr = "master";
        
        //check if the hos has alread been registered
        if(file_exists($hosName)){
            echo "This Hospital Has Already Been Registered";
        }else{
            echo $hospital." Is Now Registered";
            //create the hos dir
            mkdir($hospital);
            //create example room dir
            mkdir($hosName.'exRm');
            //copy the add file
            copy($poths.'ex/add.php', $hosName.'exRm/add.php');
            //create users dir
            mkdir($hosName.'usrs');
            //copy master user file
            copy($poths.'usrs/'.$masterUsr, $hosName.'usrs/'.$masterUsr);
        }
        
        $hospital = $_POST['hosSub'];
  }else{
  	$hospital = $_COOKIE['hospital'];
  }
   
    
?>
<html>
        <head>
		<link type="text/css" rel="stylesheet" href="/home/hrs/public_html/style.css" />
		<link rel="shortcut icon" href="favicon.ico" type="image/x-icon"/>
		<title><?php echo $hospital; ?> - HRS</title>
	</head>
	<body>
            
            <h2>Welcome to <?php echo $hospital; ?></h2>
            
            <form id="form" action="/room.php" method="POST" name="form">

                    <p>Room Number:</p>

                    <input id="rmIn" name="rmSub" type="text" value="<?php echo $_COOKIE['room']; ?>"><br>
                    <input id="tz" name="tz" type="hidden"><br>
                    <input type="submit" name="subRm" value="Submit"><br>
            </form>
            
            <script>
			var timezone_offset_minutes = new Date().getTimezoneOffset();
			timezone_offset_minutes = timezone_offset_minutes == 0 ? 0 : -timezone_offset_minutes;
			//var data = timezone_offest_minutes;
			//$.post('savedata.php', {data=data}, function(){});
			document.getElementById("tz").value = timezone_offset_minutes;
		</script>
            
            <form id="newAd" action="/addUsr.php" method="POST" name="newAd">
            	<input type="submit" name="subNAd" value="Register a New Admin">
            </form>
           
                
        </body>
</html>