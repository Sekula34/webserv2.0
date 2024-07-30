## Webserver pseudocode

### Webserver
1. Read config file
2. [Run server](#run-server)
3. End

### Run Server
1. Make Sockets  
2. WHILE ctrl + c is not pressed
	1. IF there is something to read
		1. YES - [Process Ready to Read](#process-ready-to-read)
	2. IF there is something to write
		1. YES - Process Ready to Write
3. End

### Process Ready to Read
1. Get all Socket that are ready to read
2. Check if there is 1 or more ready to read sockets
	1. YES - [Handle all ready socket](#handle-all-ready-socket)
3. Check if there is ready to read Clients 
	1. YES - [Handle all ready clients](#handle-all-ready-clients)

### Handle all ready socket
1. For every ready Socket
	1. Accept connection (get client FD);
	2. Store Client FD in _communicationFDsl
	3. Set Client FD to be ready to read

### Handle all ready Clients 
1. For every ready Client
	1. Get client FD
	2. Value = Read client fd until Header 
	3. IF (value == Error)
		1. Remove Client
		2. BREAK to step 1;
	4. IF (vlaue == Done)
		1. [Generate Client Response](#generate-client-response)

### Generate Client Response 
1. IF client header have error code (that happend while reading)
	1. YES - [Generate server error response](#generate-server-error-response-error-code) (clientHeaderError)
	2. NO - [Generate server "normal" response](#generate-server-normal-reponse)
2. Store Response (but not send it)
3. Set client to be ready to write

### Generate server error response (error code)
1. Create body message //(Response body)
2. _Create response header_ (maybe not here)

### Generate server "normal" reponse 
1. var = Read client header find what he wants 
2. IF var == GET
	1. Handle Get Method
3. IF var == POST 
	1. Handle Post method 
4. IF var = Delete 
	1. Handle Delete


### Handle Get Method
1. Seperate client requested location and client requested file
2. Find which location client wants
3. IF location is redirected
	1. Handle Redirect (nginx return)
	2. return 
4. value = Try to access site (constrct from location and requested file)
5. IF (value == success)
	1. return 200
6. [Generate server erorr response](#generate-server-error-response-error-code) (value)
