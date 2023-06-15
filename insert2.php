<?php
include "connect.php";
$id = $_POST['id'];
$temp = $_POST['temp'];
$led = $_POST['led'];
$sw = $_POST['sw'];
$grap = $_POST['grap'];

$sql = "INSERT INTO tbl2 (id,temp,led,sw,grap) VALUES ('$id', '$temp', '$led', '$sw','$grap')";

if (mysqli_query($conn, $sql)) {
    echo "New record created successfully";
} else {
    echo "Error: " . $sql . "<br>" . mysqli_error($conn);
}

mysqli_close($conn);
header( "location: form2.php" );
?>