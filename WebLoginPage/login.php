 <?php
include("config.php");
session_start();
$error      = false;
$myusername = '';
$mypassword = '';

$myusername = addslashes($myusername);

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // username and password sent from form 
    
    $myusername = $_POST['username'];
    $mypassword = $_POST['password'];
    
    $sql    = "SELECT * FROM admin WHERE (email = '$myusername' OR user = '$myusername') AND pass = '" . md5($mypassword) . "'";
    $result = mysqli_query($db, $sql);
    $row    = mysqli_fetch_array($result, MYSQLI_ASSOC);
    //$active = $row['active'];
    
    $count = mysqli_num_rows($result);
    
    // If result matched $myusername and $mypassword, table row must be 1 row
    
    if ($count == 1) {
        $_SESSION['login_user'] = $myusername;
        
        header("location: index.php");
        
    } else {
        $error = "Incorrect Username or Password Entered";
    }
}
?> 


<html>
	<head>
		<form action= "" method="post">
			<button class="regBtn" formaction="/register.php">Register</button>
		</form>
	<body class="login">
		<link href="/css/c3.css" rel="stylesheet" type="text/css">
		<title>Weather Monitor Login</title>
		<meta name="viewport" content="width=device-width, initial-scale=1">
		</head>
		<body>
			<style>
			</style>
			<h2>
				<center>Weather Monitor Login</center>
			</h2>
			<form class="loginForm" action = "" method = "post">
				<div class="LoginImgContainer">
					<img src="weather.png" alt="weather">
				</div>
				<div class="LoginContainer">
					<input class="LoginInput" type="text" placeholder="Email or Username" name="username" required>
					<input class="LoginInput" type="password" placeholder="Password" name="password" required>
					<button class="loginBtn" type="submit">Login</button>
					<center>
					<div class="unsuccessfulLogin"><?php echo $error; ?></div>
				</div>
			</form>
	</body>
</html>
