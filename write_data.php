<?php
    // log in vao database
    include("config.php");
    // doc user input
    $red = $_POST["r"];
    $green = $_POST["g"];
    $blue = $_POST["b"];
    $brightness = $_POST["br"];
    $check = $_POST["bt"];
    $change = $_POST["led_updated"];
    // update lai database
    $sql = "insert into led_control (red,green,blue,ledState,brightness,changeState) values ($red,$green,$blue,$check,$brightness,$change);";
    mysqli_query($conn, $sql);

    mysqli_close($conn);


?>