<!DOCTYPE HTML>

<?php 
//check if data is submitted
if(isset($_POST['subAssign'])){
        //def path params
        $poths = "/home/hrs/public_html/";
        $fileName=$_POST['espIP'];
        $hosCode=$_POST['hospital'];
        $rmNum=$_POST['room'];
        $fName = $poths."chips/".$fileName;
        //return a line stating that the chip is registered
        echo $fileName." Is Now Registered";
        //open, write info to, and close file
        $file = fopen($fName, 'w');
        fwrite($file, "hospitalCode:".$hosCode."\nroomNum:".$rmNum);
        fclose($file);
}


?>


<html>
	<head>
		<link type="text/css" rel="stylesheet" href="masterStylesheet.css"/>
		<link rel="shortcut icon" href="favicon.ico">
		<title>HRS Admin</title>
	</head>
	<body>
	<h1>Welcome to HRS Admin</h1>
	<h3>Assign a Chip:</h3>
	
        <form id="form" action="<?php echo $_SERVER['PHP_SELF']; ?>" method="POST" name="form">
                        
                <p>Chip IP Address:</p>

                <input id="ipIn" name="espIP" placeholder="Enter IP Address" type="text"><br>

                <p>Four-Digit Hospital Code:</p>

                <input id="hosIn" name="hospital" placeholder="Enter Hospital Code" type="text" value="<?php echo $_COOKIE['hospital']; ?>"><br>

                <p>Room Number:</p>

                <input id="rmIn" name="room" placeholder="Enter Room Number" type="text" value="<?php echo $_COOKIE['room']; ?>"><br>

                <input type="submit" name="subAssign" value="Submit"><br>
	</form>
	
	<form id="form2" action="/index.php" method="POST" name="home">
		<input type="submit" name="goHome" value="Return to the Homepage"><br>
	</form>
    </body>
</html>