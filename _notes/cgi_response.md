
## CGI SCRIPT RULES

- The script MUST check the REQUEST_METHOD variable!

## CGI RETURN RULES

###  GENERAL

- every CGI response MUST contain at least one header field -> HEADER is MANDATORY
- the BODY may be NULL, meaning may be empty 
- the response contains a MESSAGE HEADER and optionally a MESSAGE BODY seperated by a blank line
- NGINX recognizes both options as a delimiter between header and body "\r\n\r\n" and "\n\n"
seems logical because both sequences result in an empty line. Maybe that's why 
the RFC states it like that: 'seperated by a blank line'.
    - IMPORTANT! Two "\n\n" include the "\n" that is at the end of the last header field.
    So a valid minimal CGI header example would be:
        - ```Content-type: text/html\n\n```
    - NGINX even accepts these examples
        - ```Content-type: text/html\r\n\r\n```
        - ```Content-type: text/html\n\r\n```
        - ```Content-type: text/html\n\r``` (this is especially WTF, welcome to minishell hell)
- every header field ends in a "\n" -> this means it can also end in "\r\n"
    - when no "\n" at end of field then NGINX return 502 "bad gateway"
- there are four options for a CGI response:
    - document-response
    - local-redir-response
    - client-redir-response
    - client-redirdoc-response
- the response is printed to the standard out of the CGI script
- the server may implement a timeout period within which the CGI script must print to stdout

### CGI RESPONSE HEADER

The CGI Response header consists of header fields, very similar to the http protocol header.

The header fields can be of three types:
- CGI header fields (these are for the server NOT FOR THE CLIENT)
    - Content-Type
    - Location
    - Status
- extension header fields (these are additional and optional CGI header fields)
- HTTP header fields (these are for the CLIENT and should be forwarded to Client)

Each header field in a CGI-Response MUST be specified on a single line; CGI/1.1 does not
support continuation lines.  Whitespace is permitted between the ":"
and the field-value (but not between the field-name and the ":"), and
also between token s in the field-value.

#### CGI header field - CONTENT-TYPE

The Content-Type field sets the Internet Media Type of the BODY.

example: ``` Content-Type: text/html \n ```

If a BODY is returned, the content-type field is MANDATORY!!! If the CGI script
does not provide a content-type field then the value should be sent
unmodified to the client. Except for any charset parameter changes
(not exactly sure how this charset thing affects us).

### DOCUMENT RESPONSE

This is the 'REGULAR' CGI response. Usually the 'document' is an html file
that the CGI script prints to stdout. The content-type field is mandatory because
the server has no way of knowing what the file type is that it gets from CGI.
So the server can not send the correct filetype to the client.

The file could also be an image or another script or a PDF file... any file.
[This article](https://www.oreilly.com/library/view/cgi-programming-on/9781565921689/06_chapter-03.html)
mentions that the CGI script can check the HTTP_ACCEPT metavariable where the Client
defines what kind of filetypes it accepts. So in a perfect world the CGI script
only sends filetypes that the client can accept. For instance send a .jpeg if it
is supported and if not send text instead.
    

- the script MUST return a Content-Type header field.
- a status header field is optionoal
- status 200 'OK' is assumed if the status header field is omitted
- The server MUST make any modifications to the scripts outpus to make sure that 
the response to the Client complies with HTTP protocol

### LOCAL REDIRECT RESPONSE

The CGI script can return a ***LOCAL*** URI path and a query string in the
Location Header field.

- the script MUST NOT return any other header fiels or a message-body
- and the server MUST generate a response, that it would have, had it gotten
the request in the Location header field.

### CLIENT REDIRECT RESPONSE

Unlike in the local redirect response, the client can return an absolute URI path,
that usually is outside the server, but does not have to be.

The URI is again in a location header field.

- The script MUST not provide any other header fields than the location one,
except for server-defined CGI extensions 
- The server must turn this single Location Header field into a proper
302 redirect response message

### CLIENT REDIRECT RESPONSE WITH DOCUMENT

Just like in the client redirect response the header contains a location field
with an ABSOLUTE PATH (that points to another website usually)

The difference is that here a document is attached. Meaning the CGI response
contains a body. (usually this is a message like: "if your browser does not
support atomatic redirection click this link")

- The location header field MUST be supplied
- the status header field MUST be supplied
- the status header field MUST contain a 302 found or similar extension code
- the content-type header field MUST be supplied
- a body MUST exist
- the server MUST apply any changes to the Header necessary to make it HTTP valid
- 





