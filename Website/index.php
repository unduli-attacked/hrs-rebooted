<html>
	<head>
		<link type="text/css" rel="stylesheet" href="/home/hrs/public_html/style.css" />
		<link rel="shortcut icon" href="favicon.ico" type="image/x-icon"/>
		<title>HRS</title>
	</head>
	<body>
			<form id="form" action="/rmSplash.php" method="POST" name="form">
                        
				<h1>Hospital Code:</h1>
                        
   				<input id="hosIn" name="hosSub" type="text" value="<?php echo $_COOKIE['hospital']; ?>"><br>
   				<input type="submit" name="subHos" value="Submit"><br>
			</form>
			<form id="form2" action="/assignChip.php" method="POST" name="chip">
				<input type="submit" name="assignChip" value="Register a Chip"><br>
			</form>
                
        </body>
</html>