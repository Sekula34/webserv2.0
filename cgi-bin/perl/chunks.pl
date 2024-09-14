use strict;
use warnings;

#print "Status: 200 OK\r\n";
print "Content-Type: text/html; charset=utf-8\r\n";
print "Transfer-Encoding: chunked\n\n";

# First chunk
print "60\r\n";
print "<html><head><title>Chunked HTML Response</title></head><body><h1>Hello World!</h1></body></html>";
print "\r\n";

print "15\r\n";
print "<h2>second chunk</h2>";
print "\r\n";


# Final chunk
print "0\r\n\r\n";

