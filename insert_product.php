<?php
include "connect.php";
//$id = $_POST['id'];
$name = $_POST['name'];
$price = $_POST['price'];
$num = $_POST['num'];
$pic = $_POST['pic'];

$sql = "INSERT INTO tbl_product (name,price,num,pic) VALUES ('$name', '$price', '$num','$pic')";

if (mysqli_query($conn, $sql)) {
    echo "บันทึกเเล้วจ้าาาาาาา";
} else {
    echo "Error: " . $sql . "<br>" . mysqli_error($conn);
}

mysqli_close($conn);
header( "location: form_product.php" );
?>