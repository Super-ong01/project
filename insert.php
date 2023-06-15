<?php
include "connect.php";
$fullname = $_POST['fullname'];
$address = $_POST['address'];
$email = $_POST['email'];
$photo = $_POST['photo'];

$sql = "INSERT INTO tbl1 (name, address, email,photo) VALUES ('$fullname', '$address', '$email', '$photo')";

if (mysqli_query($conn, $sql)) {
    echo "New record created successfully";
} else {
    echo "Error: " . $sql . "<br>" . mysqli_error($conn);
}

mysqli_close($conn);
header( "location: form.php" );
?>