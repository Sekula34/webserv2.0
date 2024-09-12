<?php
// Output the necessary HTTP header for CGI scripts
echo "Content-Type: text/html\n";
echo "Content-Length: 80\r\n\r\n";

// Start outputting HTML content
echo "<!DOCTYPE html>";
echo "<html>";
echo "<head><title>Hello World</title></head>";
echo "<body>";
echo "<h1>Hello, World!</h1>";
echo "</body>";
echo "</html>";
?>

