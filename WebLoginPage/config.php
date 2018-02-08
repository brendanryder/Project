<?php
   define('DB_address', '192.168.0.157');
   define('DB_user', 'root');
   define('DB_pass', 'root');
   define('DB_databaseName', 'mydb123456');
   $db = mysqli_connect(DB_address,DB_user,DB_pass,DB_databaseName);
?>