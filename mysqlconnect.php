 <?php
$servername = "10.12.16.180";
$username = "root";
$password = "root";
$dbname = "animalsurvey";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT id, surveyoption, votes FROM surveyresults";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // output data of each row
    while($row = $result->fetch_assoc()) {
        echo "id: " . $row["id"]. " - Name: " . $row["surveyoption"]. " " . $row["votes"]. "<br>";
    }
} else {
    echo "0 results";
}
$conn->close();
?> 
