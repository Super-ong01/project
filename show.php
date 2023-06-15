<link rel="stylesheet"  href="style.css" />

<?php
include "connect.php";

$sql = "SELECT * FROM tbl1";
$result = mysqli_query($conn, $sql);

if (mysqli_num_rows($result) > 0) {
    // output data of each row
?>

<table >
    <th> name </th>
    <th> address </th>
    <th> email </th>
    <th> photo </th>

<?php
    while ($row = mysqli_fetch_assoc($result)) {
?>

    <tr>
        <td> <?php echo "$row[name] "; ?> </td>
        <td> <?php  echo "$row[address] ";  ?>  </td>
        <td> <?php echo "$row[email] "; ?> </td>
        <td> <?php echo "$row[photo] ";  ?> </td>
    </tr>
    
<?php
    }
?>

</table>

<?php

} else {
    echo "0 results";
}

mysqli_close($conn);
?>