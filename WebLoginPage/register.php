

<?php
   include("config.php");
   session_start();
   $error = false;
if (isset($_POST['signup'])) {
	$name = $_POST['username'];
	$email = $_POST['email'];
	$password = $_POST['password'];
	$cpassword = $_POST['confirmPassword'];
	
	$sql_username = "SELECT * FROM admin WHERE user='$name'";
  	$sql_email = "SELECT * FROM admin WHERE email='$email'";
  	$res_username = mysqli_query($db, $sql_username);
  	$res_email = mysqli_query($db, $sql_email);

  	if (mysqli_num_rows($res_username) > 0) {
  	  $username_error = "A user with this username already exists";
	  $error = true;
	}
	
	if (mysqli_num_rows($res_email) > 0) {
  	  $emailName_error = "A user with this email already exixts";
	  $error = true;
	}
	
	if($password != $cpassword) {
		$error = true;
		$cpassword_error = "Password and Confirm Password doesn't match";
	}
	
	if(!filter_var($email,FILTER_VALIDATE_EMAIL)) {
		$error = true;
		$email_error = "The email address you supplied is invalid.";
	}
	
	if (!$error) {
    
	if(mysqli_query($db, "INSERT INTO admin(user, email, pass) VALUES('" . $name . "', '" . $email . "', '" . md5($password) . "')")) {
		$success_message = "Successfully Registered . . Redirecting";
		?>
		<body><center><div style = "font-size:25px; color:green; margin-top:10px"><?php echo @$success_message; ?></div></center><body>
		<?php
		echo "<script>setTimeout(\"location.href = 'login.php';\",2500);</script>";
	}
	else {
			$error_message = "Error in registering...Please try again later!";
	}
      
   }
}
?>
<html>

<head>
<body class="hourly">
<link href="/css/c3.css" rel="stylesheet" type="text/css">
<title>Weather Monitor Registration</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
</head>

<h2><center>Weather Monitor Registration</center></h2>
<form class = "registerForm" action = "" method = "post">
  <div class="img_reg">
    <img class="weatherImg">
  </div>

  <div class="registration_container">

    <input class="Myinput" type="text" placeholder="Enter Username" name="username" required>
	<center><div style = "font-size:16px; color:#cc0000; margin-top:10px"><?php echo @$username_error; ?></div></center>
	
    <input class="Myinput"type="text" placeholder="Enter Email" name="email" required>
	<center><div style = "font-size:16px; color:cc0000; margin-top:10px"><?php echo @$email_error; ?></div></center>
	<center><div style = "font-size:16px; color:#cc0000; margin-top:10px"><?php echo @$emailName_error; ?></div></center>
	
    <input class="Myinput" type="password" placeholder="Enter Password" name="password" required>
    <input class="Myinput" type="password" placeholder="Confirm Password" name="confirmPassword" required>
	<center><div style = "font-size:16px; color:#cc0000; margin-top:10px"><?php echo @$cpassword_error; ?></div></center>
    <button class="registerBtn" type="submit" name="signup">Register User</button>

  </div>

</form>

</body>
</html>