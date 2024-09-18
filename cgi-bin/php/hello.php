<?php
// Output the necessary HTTP header for CGI scripts
echo "Content-Type: text\n\r";
//echo "Something: hi\r\r\n";

// Start outputting HTML content
echo "<!DOCTYPE html>";
echo "<html>";
echo "<head><title>Hello World</title></head>";
echo "<body>";
echo "<h1>Hello, World!</h1>";
echo "</body>";
echo "</html>";
?>
