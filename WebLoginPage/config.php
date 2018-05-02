<?php
   define('DB_address', '192.168.43.79');
   define('DB_user', 'root');
   define('DB_pass', 'root');
   define('DB_databaseName', 'userLogin');
   $db = mysqli_connect(DB_address,DB_user,DB_pass,DB_databaseName);
?>