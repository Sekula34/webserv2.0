OSI- Open systems Interconnection model is conceptual model that
	characterrizes and standardizes the communication functions of
	telecommunication or computing system withot regard to its 
	underlying internan structure and technology. Original version 7 layers

Transport layer - primarly resposible for ensuring that data is transferred
	from one point to another realiably and without errors. Common example
	TCP (Transmission Control Protocol), User Datagram Protocol (UDP)

RFC - Request for CommentsAn RFC is authored by engineers and 
	computer scientists in the form of a memorandum describing methods, 
	behaviors, research, or innovations applicable to the working of the 
	Internet and Internet-connected systems

Relevant RFC for HTTP 1.1/ 7230, 7231, 7232, 7233, 7234, 7235

Socket - mechanism that OS provide to give program access to the network.
	It allows messages to be sent and received betweem applications
	(different processes) on different networked machines. 

TCP/IP sockets steps : create socket, identify socket, ON SERVER wait for incoming connection, send and receive message, close the socket


create socket : int serverFD = socket(domain, type, protocol)  in <sys/socket.h>
	domain is AF_INET, type is SOCK_STREAM

AF_INET - Adress Family Internet IP, IPv4 adress 

SOCK_STREAM - socket type, Setting up socket that uses Transmission Cotnrol Protocol for data transmisson
			Connection oriented- connection must be esatblished between client and server 
			Reliable -TCP guarantees that data sent from one end of the connection will arrive at the other 
				end in the same order it was sent without errors. If any data is lost or corrupted it will be retrasmitted
			ByteStream- data is read as a continous stream of bytes, no message limit


part of identifying(naming) socket is bind 
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
0 success, -1 error and errno



bind - assigning a transport address to the socket
		analogy, socket is mailbox and with bind we give it address
		transport address is defined in socket address structure. There is different types of communication interfaces that use socket
		that is why it take sockaddr structure. Format of that structure is determined on the address family ()

To allow your server to restart quickly and reuse the same port, you can set the `SO_REUSEADDR` socket option before you bind the socket. This option allows the socket to reuse the address even if it's in a `TIME_WAIT` state.
SO_REUSEADDR: Allow reuse of local addresses.
SO_REUSEPORT: Allow multiple sockets to bind to the same port.
Here's how you can modify your code:

	```cpp
	int opt = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		return 1;
	}

	// Now bind the socket
	// bindVal = bind(serverFd, const struct sockaddr *addr, socklen_t len)

	// Rest of your code...
```

This code sets the `SO_REUSEADDR` and `SO_REUSEPORT` options on the socket before binding it. If `setsockopt` fails, it prints an error message and returns 1.

Please note that `SO_REUSEPORT` is not available on all platforms. If you're not planning to bind multiple sockets to the same port, you can remove `SO_REUSEPORT` from the `setsockopt` call.
	int opt = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		return 1;
	}

	// Now bind the socket
	// bindVal = bind(serverFd, const struct sockaddr *addr, socklen_t len)

	// Rest of your code...


defined in netinet/in.h
struct sockaddr_in 
{ 
    __uint8_t         sin_len; 
    sa_family_t       sin_family; 
    in_port_t         sin_port; 
    struct in_addr    sin_addr; 
    char              sin_zero[8]; 
};

you need to fill up these before bind

sin_family - socket internet family, here goe AF_INET

sin_port - socket internet Port, you dont need to set this for Client but you need one for server def 

sin_addr - socket internet address, address for this socket. Machine Ip address. 
	machine have one IP for each network interface. if you have WIFI and ethernet machine have two address, one for each interface.
	Usually we dont want to specify a specifit interface and let OS do whatever he wants. That is speciall adress 
	0.0.0.0 defined as INNADDR_ANY - Interned address any. Bindig socket to all available network interfaces.

sizeof(struct sokaddr_in) lenght of structure 

htonl - host to network long. It is used to convert 32 bit integer(example address) 
	from host byte to network byte orderd. 
	HOST byte order can be either little endian or bign endian
	Network byte order is standardized and it is big endian. (most siginificatn byte is at smalles address)
	
	#include <arpa/inet.h>
	uint32_t htonl(uint32_t hostlong);

ntohl - network to host long. Reverse action of htonl. 

htons - host to network short. convert 16 bit integer (example port);
	#include <arpa/inet.h>
	uint16_t htons(uint16_t hostshort);

ntohs - networkd to host short. Reverse action of htons;

3. Part of serve wait for an incoming connection 

Before client can connect to a server server should have socket that is prepared
	to accept the connections. listen system call tells a socket that it should 
	be capable of accepting connections 

#include <sys/socket.h> 
int listen(int socket, int backlog);
0 sucesss, -1 error and errno

backglog defines the maximum number of pending connections that can be queed up
before connections are refused. 

accept system call grabs first connection request on the queue of pending connections
	(set up) in listen and creates a new socket for that connection

The original socket that was set up for listening is used only for accepting connections,
	not for exchanging data. By default socket operations are synchronous, or blocking,
	and accept will block until a connection is presetn on the queue.

#include <sys/socket.h> 
int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);

first param socket is the socket that was set for accepting connections with listen.
	second param is the address structure that gets filed in with the address of the client that is doing the connect
	This allows us to examine the address and port number of the connecting socket if we want to.
	The third parameter is filled in with the length of the address structure.
	return success nonNegative filedescriptor, error -1 and errno 


So, if you type www.example.com in the web browser, the web browser 
re-constructs the URL/Address as:
http://www.example.com:80
80 is default port for http


select in c 
	 Upon return, each of the file descriptor sets is
       modified in place to indicate which file descriptors are
       currently "ready".  Thus, if using select() within a loop, the
       sets must be reinitialized before each call.
	Whenever you use select and before it return it decler all socket 
	descripptors set

	int select(int nfds, fd_set *_Nullable restrict readfds,
		fd_set *_Nullable restrict writefds,
		fd_set *_Nullable restrict exceptfds,
		struct timeval *_Nullable restrict timeout);

       void FD_CLR(int fd, fd_set *set);
       int  FD_ISSET(int fd, fd_set *set);
       void FD_SET(int fd, fd_set *set);
       void FD_ZERO(fd_set *set);


       The contents of a file descriptor set can be manipulated using
       the following macros:

       FD_ZERO()
              This macro clears (removes all file descriptors from) set.
              It should be employed as the first step in initializing a
              file descriptor set.

       FD_SET()
              This macro adds the file descriptor fd to set.  Adding a
              file descriptor that is already present in the set is a
              no-op, and does not produce an error.

       FD_CLR()
              This macro removes the file descriptor fd from set.
              Removing a file descriptor that is not present in the set
              is a no-op, and does not produce an error.

       FD_ISSET()
              select() modifies the contents of the sets according to
              the rules described below.  After calling select(), the
              FD_ISSET() macro can be used to test if a file descriptor
              is still present in a set.  FD_ISSET() returns nonzero if
              the file descriptor fd is present in set, and zero if it
              is not.


Header recived from Google Chrome browser : http://localhost:8080/hej
	GET /hej HTTP/1.1
	Host: localhost:8080
	Connection: keep-alive
	sec-ch-ua: "Not/A)Brand";v="8", "Chromium";v="126", "Google Chrome";v="126"
	sec-ch-ua-mobile: ?0
	sec-ch-ua-platform: "Linux"
	Upgrade-Insecure-Requests: 1
	User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36
	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
	Sec-Fetch-Site: none
	Sec-Fetch-Mode: navigate
	Sec-Fetch-User: ?1
	Sec-Fetch-Dest: document
	Accept-Encoding: gzip, deflate, br, zstd
	Accept-Language: en-US,en;q=0.9,hr;q=0.8,bs;q=0.7

using curl to send request to specific domain host
	curl stand for Client URL 
	curl -H "Host: specific-domain.com" localhost:8080

	request header received is 
	GET / HTTP/1.1
	Host: specific-domain.com
	User-Agent: curl/7.81.0
	Accept: */*


int socket;
fd_set read;
fd_set readTemp;

FD_ZERO(read);
FD_SET(socket, &read);
while(true)
{
	readTemp = read;
	select()

	

}

minimum HTTP 1.1 request
	GET / HTTP/1.1
	Host: example.com

CR LF - carriage return line feed, \r\n 




-----------------7230-------------------------------



A server can send a 505
 (HTTP Version Not Supported) response if it wishes, for any reason,
 to refuse service of the client’s major protocol version.

A sender MUST NOT generate an "http" URI with an empty host
 identifier. A recipient that processes such a URI reference MUST
 reject it as invalid. 400 Bad request;

If host is a registered name, the registered name is an
 indirect identifier for use with a name resolution service, such as
 DNS, to find an address for that origin server. If the port
 subcomponent is empty or not given, TCP port 80 (the reserved port
 for WWW services) is the default


HTTP FORMAT

HTTP-message = start-line
 *( header-field CRLF )
 CRLF
 [ message-body ]

The normal procedure for parsing an HTTP message is to read the
 start-line into a structure, read each header field into a hash table
 by field name until the empty line, and then use the parsed data to
 determine if a message body is expected. If a message body has been
 indicated, then it is read as a stream until an amount of octets
 equal to the message body length is read or the connection is closed.

A sender MUST NOT send whitespace between the start-line and the
 first header field. A recipient that receives whitespace between the
 start-line and the first header field MUST either reject the message
 as invalid

Start Line - it is either request-line for request or a status-line for responses

Server should not recive response but it it does it is interpreted as an unknown or invalid request methods

Request Line 
	A request-line begins with a method token, followed by a single space
 	(SP), the request-target, another single space (SP), the protocol
 	version, and ends with CRLF.

	request-line = method SP request-target SP HTTP-version CRLF

Recipients of an invalid request-line SHOULD respond with either a
 400 (Bad Request) 

HTTP does not place a predefined limit on the length of a
 request-line, as described in Section 2.5. A server that receives a
 method longer than any that it implements SHOULD respond with a 501
 (Not Implemented) status code. A server that receives a request-target longer than any URI it wishes to parse MUST respond
 with a 414 (URI Too Long) status code (see Section 6.5.12 of
 [RFC7231]).
 It is RECOMMENDED that all HTTP senders and recipients
 support, at a minimum, request-line lengths of 8000 octets.


Status Line 
	The first line of a response message is the status-line, consisting
	of the protocol version, a space (SP), the status code, another
	space, a possibly empty textual phrase describing the status code,
	and ending with CRLF.
	status-line = HTTP-version SP status-code SP reason-phrase CRLF
	status-code = 3DIGIT

	example : [HTTP/1.1 200 OK\r\n]

Header Fields 
	Each header field consists of a case-insensitive field name followed
	by a colon (":"), optional leading whitespace, the field value, and
	optional trailing whitespace.
	header-field = field-name ":" OWS field-value OWS

	Other recipients SHOULD ignore unrecognized header fields.
	These requirements allow HTTP’s functionality to be enhanced without
	requiring prior update of deployed intermediaries.

	OWS = *( SP / HTAB )
	; optional whitespace
	RWS = 1*( SP / HTAB )
	; required whitespace
	BWS = OWS
	; "bad" whitespace

"Field-Name: Field Value"
	No whitespace is allowed between the header field-name and colon. In
	the past, differences in the handling of such whitespace have led to
	security vulnerabilities in request routing and response handling. A
	server MUST reject any received request message that contains
	whitespace between a header field-name and colon with a response code
	of 400 (Bad Request). A proxy MUST remove any such whitespace from a
	response message before forwarding the message downstream.

		A field value might be preceded and/or followed by optional
	whitespace (OWS); a single SP preceding the field-value is preferred
	for consistent readability by humans. The field value does not
	include any leading or trailing whitespace: OWS occurring before the
	first non-whitespace octet of the field value or after the last
	non-whitespace octet of the field value ought to be excluded by
	parsers when extracting the field value from a header field.


Messaage Body 
	message-body = *OCTET

A server MAY reject a request that contains a message body but not a
 Content-Length by responding with 411 (Length Required).

An HTTP/1.1 user agent MUST NOT preface
 or follow a request with an extra CRLF. If terminating the request
 message body with a line-ending is desired, then the user agent MUST
 count the terminating CRLF octets as part of the message body length

 The presence of a message body in a request is signaled by a
 Content-Length or Transfer-Encoding header field.

A server that receives a request message with a transfer coding it
 does not understand SHOULD respond with 501 (Not Implemented).

example : Transfer-Encoding: gzip, chunked


Content-Length = 1*DIGIT
 An example is
 Content-Length: 3495

 Any Content-Length field value greater than or equal to zero is
 valid. Since there is no predefined limit to the length of a
 payload, a recipient MUST anticipate potentially large decimal
 numerals and prevent parsing errors due to integer conversion
 overflows (Section 9.3).

Response Content-Length
	All 1xx (Informational), 204 (No Content), and 304 (Not Modified)
	responses do not include a message body. All other responses do
	include a message body, although the body might be of zero length.

Request Content-Length
	For example, a Content-Length header
	field is normally sent in a POST request even when the value is 0
	(indicating an empty payload body). A user agent SHOULD NOT send a
	Content-Length header field when the request message does not contain
	a payload body and the method semantics do not anticipate such a
	body.

If a valid Content-Length header field is present without
 Transfer-Encoding, its decimal value defines the expected message
 body length in octets. If the sender closes the connection or
 the recipient times out before the indicated number of octets are
 received, the recipient MUST consider the message to be
 incomplete and close the connection.

A server MAY reject a request that contains a message body but not a
Content-Length by responding with 411 (Length Required).

message that uses a valid Content-Length is incomplete
if the size of the message body received (in octets) is less than the
value given by Content-Length.


Request Target 
	can have 4 forms if i understand this correctly in this case i can only receive origin form



If the target URI’s path component is
 empty, the client MUST send "/" as the path within the origin-form of
 request-target. A Host header field is also sent, as defined in
 Section 5.4.
 For example, a client wishing to retrieve a representation of the
 resource identified as
 http://www.example.org/where?q=now
 directly from the origin server would open (or reuse) a TCP
 connection to port 80 of the host "www.example.org" and send the
 lines:
 GET /where?q=now HTTP/1.1
 Host: www.example.org
 followed by the remainder of the request message.

A client MUST send a Host header field in all HTTP/1.1 request
 messages


A server MUST respond with a 400 (Bad Request) status code to any
 HTTP/1.1 request message that lacks a Host header field and to any
 request message that contains more than one Host header field or a
 Host header field with an invalid field-value.


Connection: close
 in either the request or the response header fields indicates that
 the sender is going to close the connection after the current
 request/response is complete (Section 6.6).
 A client that does not support persistent connections MUST send the
 "close" connection option in every request message.
 A server that does not support persistent connections MUST send the
 "close" connection option in every response message that does not
 have a 1xx (Informational) status code.


 -----7230 END--------


---------7231------------------

A sender that generates a message containing a payload body SHOULD
 generate a Content-Type header field in that message unless the
 intended media type of the enclosed representation is unknown to the
 sender. If a Content-Type header field is not present, the recipient
 MAY either assume a media type of "application/octet-stream"
 ([RFC2046], Section 4.5.1) or examine the data to determine its type.
 Content-Type: text/html; charset=ISO-8859-4


A language tag is a sequence of one or more case-insensitive subtags,
each separated by a hyphen character ("-", %x2D). In most cases, a
language tag consists of a primary language subtag that identifies a
broad family of related languages (e.g., "en" = English), which is
optionally followed by a series of subtags that refine or narrow that
language’s range (e.g., "en-CA" = the variety of English as
communicated in Canada). Whitespace is not allowed within a language
tag. Example tags include:
fr, en-US, es-419, az-Arab, x-pig-latin, man-Nkoo-GN

Content-Language: mi, en
However, just because multiple languages are present within a
representation does not mean that it is intended for multiple
linguistic audiences. An example would be a beginner’s language
primer, such as "A First Lesson in Latin", which is clearly intended
to be used by an English-literate audience. In this case, the
Content-Language would properly only include "en".

Long story short for this project put Content-Lnaguage: en;

All general-purpose servers MUST support the methods GET and HEAD.
 All other methods are OPTIONAL.

HEAD | Same as GET, but only transfer the status line and header section

When a request method is received
that is unrecognized or not implemented by an origin server, the
origin server SHOULD respond with the 501 (Not Implemented) status
code. When a request method is received that is known by an origin
server but not allowed for the target resource, the origin server
SHOULD respond with the 405 (Method Not Allowed) status code.


The HEAD method is identical to GET except that the server MUST NOT
 send a message body in the response (i.e., the response terminates at
 the end of the header section). 


o 1xx (Informational): The request was received, continuing process
 o 2xx (Successful): The request was successfully received,
 understood, and accepted
 o 3xx (Redirection): Further action needs to be taken in order to
 complete the request
 o 4xx (Client Error): The request contains bad syntax or cannot be
 fulfilled
Fielding & Reschke Standards Track [Page 47]
RFC 7231 HTTP/1.1 Semantics and Content June 2014
 o 5xx (Server Error): The server failed to fulfill an apparently
 valid request


curl -i www.google.com   


Date: Fri, 05 Jul 2024 13:41:24 GMT
Sun, 06 Nov 1994 08:49:37 GMT


What will NGINX do

location /
root /var/www

nginx will try to find /var/www/index.html and show it 

example 2;
location /folder/
root /var/www

nginx will try to find /var/www/folder/index.html;

example 3;

location /folder2
alias /somethinNew/hej/drek;

nginx will show /somethingNew/hej/drek;


CGI Common Gateway Interface;


## const type* name

1. const type* name
Example: const int* ptr;
Explanation:

ptr is a pointer to a const int.
You can change the address stored in ptr, but you cannot modify the value at the address ptr is pointing to.
Implications:

You can make ptr point to a different integer, but you cannot change the value of the integer that ptr currently points to.

int a = 10;
int b = 20;
const int* ptr = &a;  // ptr points to a

*ptr = 30;  // Error: cannot modify a through ptr

ptr = &b;   // This is fine: ptr now points to b



## type* const name

Example: int* const ptr;
Explanation:

ptr is a constant pointer to an int.
You can modify the value at the address ptr is pointing to, but you cannot change the address stored in ptr.
Implications:

The pointer ptr will always point to the same memory address, but you can change the value at that address.

int a = 10;
int* const ptr = &a;  // ptr is a constant pointer to a

*ptr = 30;  // This is fine: you can modify the value of a through ptr

int b = 20;
ptr = &b;   // Error: cannot change the address stored in ptr


const type* const name
Example: const int* const ptr;
Explanation:

ptr is a constant pointer to a constant int.
You cannot change the address stored in ptr, nor can you modify the value at that address. Both are constant.

int a = 10;
const int* const ptr = &a;  // ptr is a constant pointer to a constant int

*ptr = 30;  // Error: cannot modify a through ptr
ptr = &b;   // Error: cannot change the address stored in ptr


getScript name = it should end .py or .php (this should be in config), 
Path Info is always after script name  this is optional

and query string can exit without path info. everyhing after question mark not including 
