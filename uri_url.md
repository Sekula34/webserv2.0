
## Example 
 http://example.com:9090/cgi-bin/hello.py/something/?name=John


## URL 

URL (Unifor Resource Locator) is the full address that provides all information needed to locate and access a resource on the web.
This includes:
* protocol `http`
* domain `example.com`
* port `9090`
* path `/cgi-bin/hello.py/something/`
* query Parameters `?name=John`

**In example url is http://example.com:9090/cgi-bin/hello.py/something/?name=John**


URLOrigin everthing before URL Suffix

## path and beyond URL SUFFIX everything after port
URI (Unifrom Resource Identifier) is the part of the URL that identifies the specific resource being requested **whithout protocol, domain or port**  

* path `/cgi-bin/hello.py/something/`  
* query Parameters `?name=John`

**In the example uri is /cgi-bin/hello.py/something/?name=John**


**in HTTP get request client send URI and Server Combines it with the host to resolve the full URL**


# pseudocode 

path is everyhin from begging of suffix till the end or first question mark or first #
querryParameters are everthing after ? (not including) until end or firs # or end
fragment is from # until end 

there can be only one # in url otherwise it is invalid
