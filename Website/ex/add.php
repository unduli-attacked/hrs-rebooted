<?php
    //def path params
    $poths = "/home/hrs/public_html/";
    
    //find the d8a and ds vars sent from the chip and copy them
    $room = $_POST["r"];
    $hospital = $_POST["h"];
    $data=$_POST["d8a"];
    $des=$_POST["ds"];
    //open, write data to, and close file
    $fle = fopen($poths.$hospital.$room."/".$des, "w");
    fwrite($fle, $data);
    fclose($fle);
    
    //echo the data and destination for error-checking purposes
    echo $data;
    echo "\n";
    echo $des;
?>