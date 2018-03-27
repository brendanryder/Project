<?php
	include("config.php");
	session_start();
	$error = false;
	
	if (isset($_POST['signup'])) {
		$name = $_POST['username'];
		$email = $_POST['email'];
		$password = $_POST['password'];
		$confirm_password = $_POST['confirmPassword'];
		
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
		if(strlen($password) < 6) {
			$error = true;
			$length_error = "Password must conatin at least SIX characters";
		}
		
		if(!preg_match('/[A-Z]/', $password)){
			$error = true;
			$caps_error = "Password must conatin at least ONE UPPERCASE character";
		}
		
		if(!preg_match('/[0-9]/', $password)){
			$error = true;
			$num_error = "Password must conatin at least ONE number";
		}
		
		if (preg_match('/\s/', $password)) {
			$error = true;
			$whitespace_error = "Password cannot conatin WHITESPACE characters";
		}		
	
		if($password != $confirm_password) {
			$error = true;
			$confirm_password_error = "Password and Confirm Password doesn't match";
		}
		
		if(!filter_var($email,FILTER_VALIDATE_EMAIL)) {
			$error = true;
			$email_error = "The email address you supplied is invalid";
		}
		
		
		if (!$error) {
			if(mysqli_query($db, "INSERT INTO admin(user, email, pass) VALUES('" . $name . "', '" . $email . "', '" . md5($password) . "')")) {
				$success_message = "Successfully Registered . . Redirecting";
?>
<body>
	<center>
		<div class ="successRegister" ><?php echo @$success_message; ?></div>
	</center>
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
	<body class="register">
		<link href="/css/c3.css" rel="stylesheet" type="text/css">
		<title>Weather Monitor Registration</title>
		<meta name="viewport" content="width=device-width, initial-scale=1">
		</head>
		<h2>
			<center>Weather Monitor Registration</center>
		</h2>
		<form class = "registerForm" action = "" method = "post">
			<div class="img_reg" alt="weather">
				<img src="weather.png" alt="weather">
			</div>
			<div class="registration_container">
				<input class="Myinput" type="text" placeholder="Enter Username" name="username" required>
				<center>
					<div class = "unsuccessfulRegister"><?php echo @$username_error; ?></div>
				</center>
				<input class="Myinput"type="text" placeholder="Enter Email" name="email" required>
				<center>
					<div class = "unsuccessfulRegister"><?php echo @$email_error; ?></div>
				</center>
				<center>
					<div class = "unsuccessfulRegister"><?php echo @$emailName_error; ?></div>
				</center>
				<input class="Myinput" type="password" placeholder="Enter Password" name="password" required>
				<div class = "unsuccessfulRegister"><?php echo @$length_error; ?></div>
				<div class = "unsuccessfulRegister"><?php echo @$caps_error; ?></div>
				<div class = "unsuccessfulRegister"><?php echo @$num_error; ?></div>
				<div class = "unsuccessfulRegister"><?php echo @$whitespace_error; ?></div>
				
				<input class="Myinput" type="password" placeholder="Confirm Password" name="confirmPassword" required>
				<center>
					<div div class = "unsuccessfulRegister"><?php echo @$confirm_password_error; ?></div>
				</center>
				<button class="registerBtn" type="submit" name="signup">Register User</button>
			</div>
		</form>
	</body>
</html>


