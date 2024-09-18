<?php
// Output the necessary HTTP header for CGI scripts
echo "Something: hi\r\r\n";
echo "Content-Type: text\r\n\r\n";

// Start outputting HTML content
echo "<!DOCTYPE html>";
echo "<html>";
echo "<head><title>Hello World</title></head>";
echo "<body>";
echo "<h1>Hello, World!</h1>";
echo "</body>";
echo "</html>";
?>
