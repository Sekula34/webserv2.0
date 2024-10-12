#!/usr/bin/perl
use strict;
use warnings;

# Function to encode the data into HTTP chunked transfer encoding
sub encode_chunked {
    my ($data) = @_;
    my $chunk_size = 1024;  # Size of each chunk in bytes

    while (length($$data) > 0) {
        # Get the next chunk
        my $chunk = substr($$data, 0, $chunk_size, '');
        # Print the length of the chunk in hex, followed by \r\n
        printf("%X\r\n", length($chunk));
        # Print the actual chunk, followed by \r\n
        print $chunk . "\r\n";
    }

    # Print the last chunk (0 length) to signal the end
    print "0\r\n\r\n";
}

# Access the 'DOCUMENT_ROOT' environment variable
my $document_root = $ENV{'PATH_INFO'} || ".";  # Default to current directory if not set

# Specify the path to the image
my $image_path = "$document_root/chunks.png";

# Open the image in binary mode
if (open(my $image_fh, "<:raw", $image_path)) {
    # Read the binary contents of the image file
    my $image_data = do { local $/; <$image_fh> };
    close($image_fh);

    # Print HTTP headers
	print "Transfer-Encoding: chunked\n";
    print "Content-type: image/png\n\n";

    # Call the function to send the data as chunked
    encode_chunked(\$image_data);

} else {
    # If the image cannot be opened, print a plain text error message
    print "Content-type: text/plain\n\n";
    print "Sorry! I cannot open the file $image_path!\n";
}

exit 0;

