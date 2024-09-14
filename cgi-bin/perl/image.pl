
# Access the 'DOCUMENT_ROOT' environment variable
my $document_root = $ENV{'DOCUMENT_ROOT'};

# Combine "/" + document_root + "/image.png"
my $gif_image = $document_root . "/rainbow.png";
if (open (IMAGE, "<" . $gif_image)) {
        $no_bytes = (stat ($gif_image))[7];
        $piece_size = $no_bytes / 10;
        print "Content-type: image/gif", "\n";
        print "Content-length: $no_bytes", "\n\n";
        for ($loop=0; $loop <= $no_bytes; $loop += $piece_size) {
                read (IMAGE, $data, $piece_size);
            print $data;
        }
         close (IMAGE);
} else {
        print "Content-type: text/plain", "\n\n";
        print "Sorry! I cannot open the file $gif_image!", "\n";
}
exit (0);

