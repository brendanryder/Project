<?php
 $filename = "http://api.sunrise-sunset.org/json?lat=53.229524&lng=-9.135489";
 $contents = file_get_contents($filename);
echo $contents;
?>

