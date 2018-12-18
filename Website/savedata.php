<?php
	$poths = "/home/hrs/public_html/";
	$data = $_POST['data'];
	$rm = $_COOKIE['room'];
	$hos = $_COOKIE['hospital'];
	$f = fopen($poths.$hos.'/'.$rm.'/timezone', 'w');
	fwrite($f, $data);
	fclose($f);
?>