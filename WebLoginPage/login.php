<?php
   include("config.php");
   session_start();
   $error = false;
   
   if($_SERVER["REQUEST_METHOD"] == "POST") {
      // username and password sent from form 
      
      $user = mysqli_real_escape_string($db,$_POST['username']);
      $pass = mysqli_real_escape_string($db,$_POST['password']); 
      
      $sql = "SELECT id FROM admin WHERE username = '$user' AND BINARY passcode = '$pass'";
      $result = mysqli_query($db,$sql);
      $row = mysqli_fetch_array($result,MYSQLI_ASSOC);
      $active = $row['active'];
      
      $count = mysqli_num_rows($result);
      
      // If result matched $user and $pass, table row must be 1 row
		
      if($count == 1) {
         $_SESSION['login_user'] = $user;
         
         header("location: weather.php");
		 
      }else {
          $error = "Incorrect Username or Password";
      }
   }
?>
<html>
<head>
<title>Weather Monitor Login</title>
<meta name="viewport" content="width=device-width, initial-scale=1">

</head>
<body>

<h2><center>Weather Monitor Login</center></h2>

<form action = "" method = "post">
  <div class="imgcontainer">
    <img src="weather.png" alt="Avatar" class="avatar">
  </div>

  <div class="container">
    <label><b>Username</b></label>
    <input type="text" placeholder="Enter Username" name="username" required>
    <label><b>Password</b></label>
    <input type="password" placeholder="Enter Password" name="password" required>
        
    <button type="submit">Login</button>
  </div>

</form>
<center><div style = "font-size:25px; color:#cc0000; margin-top:100px"><?php echo @$error; ?></div>

</body>
</html>