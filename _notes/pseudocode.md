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


### Get Location Part of URL (param:URL that ends with /)
1. string to try = url
1. While(TRUE)  
	1. IF [AM I Location](#am-i-location-paramurl)(to try) == true  
		1. retunr URL
	2. [Get new dir Name](#getdirname-param-url)(/hej/i/am/long.txt/ to /hej/i/am)


### getDirname (param: url)
1. if url end with / remove it 
2. find position of first / from right to left
3.  return that substring

### AM I Location (param:URL)
1. IF URL IS Server Location   
	1. return TRUE;
3. Return False;


### FUCNTIOn(VECTOR)
1. MAKE PAIRS
2. VEC2 = STORE BIG PART
3. IF(VEC2 > 3)
	1. FoUNCTION(VEC2)
4. INSERt PART

5. RETURN


19 20 21 22 23 24
18 6   2  4  1 11

18 19 20 21 22 23 24
### INSERT PART
1. find which Jacobstal index (6)
1 3 2 5 4 6


### SetFilePath(std::& path)
1. Find Base name of url and location
2. if(base name == "")
	1. call setIndexPagePath function
	2. retrun true or false
3. construct path from base name + root
4. return true or false 




## Epoll version
1. Parse Config
2. [Run Server](#run-server-1)


### Run Server
1. Prepare Sockets
2. Fill epoll with server Sockets (the one that will accept later conntection) (listenFd will only be for Read not for write(EPPOLIN)) //listenFD to be named ServerSocket and accpeted one from ServerSocket we still can call Client FD
3. while(true)
	1. [Epoll Main function](#epoll-main-function) 
	2. 

#### Epoll main function
1. retVal = Epollwait (same as select) retval is number of fds
2. if (retVal > 0)
	1. [Something ready](#something-ready) (either read or written to)
3. if (retVal == -1)
	1. error (CRITICAL, MAYBE WARNING???)

#### Something ready 
1. IF Loop through ready epoll FDs and check if it is Server Socket 
	1. Yes - accpet connetcion (Client Fd) should be same as Run Server step 2 (EPOLLIN and EPOLLOUT, server socket is just EPOLL IN)
	2. Add Client fd to epoll ready FDs (epoll ctrl)
2. ELSE 
	1. [Handle Client](#handle-client)


##### Handle Client 
//reading and writing at the same time 
1. Read Client Header (maybe multiple times, end of header, several times through epoll) //Simplyfiy Client Header to take only string(from which i create others variables )
	1. If buffer full and not  /r/n/r/n then go step Run Server 3.
		RETURN  
	2. If flag completeHeader == true process setCHVariables (THIS IS WHERE I CHECK IF HEADER IS VALID OR NOT)
2. Does containg body 
	1. NO - Continue
	2. Yes - AllowREad body

##### AloweRead body 
1. IF (ALLOWED)
	1. Read Body
2. Put error code to Client Header




## Possible solution for client lifespan
1. Create Client with accept 
2. Map [key:fd] [value:client]
3. If in map that key exist then marked In client instance  that is not allowed to write and remove that old client from the map
4. Value will now become new client and the old one is not destroyed yet
5. Destroy client after write and remove it from map


## Epoll client Handle 
1. PROCESS HEADER
2. IF There is body 
	1. READ BODY
	2. PROCESS BODY
3. PROCESS ANSWER