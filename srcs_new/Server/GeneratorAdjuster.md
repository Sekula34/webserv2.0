#### Repsonse Generator
* provide file content
* provide file_type  (can be txt it will be store in e_fileType)
* give httpStatus code -> this is momment where client error code becomes 200 if everything was ok or it is just translated to what it was seeted before

#### Response Header Function 
* function that takes Instance of Response Generator object (or all info that can be getted from Response Generator) and create instace of ResponseHeader 
* it will take httpStatus code and file Type and create something like old ResponseHeader class 
* it stores content type in Header Field, and http status code in first line
* Not sure how will this work with cgi but i think cgi creates its own ResponseHeader so it should be fine

#### Message 
* static function that will take instace of Response Generator and creates message 
1. [create Response Header](#response-header-function) if needed
2. create instace of Message with (Header , responseGenerator.getResponse())
