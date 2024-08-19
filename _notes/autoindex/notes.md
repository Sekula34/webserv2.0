## Auto index notes
In config file let say you have   

> location /html/{  
	> autoindex on;  
>}
>

in www/var/html/ there is following :  
* htmlSubFolder/ (with more files in it)
* index.nginx-debian.html
* info.php 
* .hiiden file 

Nginx will create html with all this files as **`a href`** but it will not include files of subfolder nor it will include hidden files that start with `.`, but ther will be **`../`** to go to the parent directory.
[See Example](nginxAutoindex.html)



### autoindex class
should definetly have path to folder from which will construct html file 
will be part of Response Body which is resposible for creation and deletion

need to have some function for generating html from such a folder and must have way of communication with outside class (I guess response body if some error happens)

maybe even generate file that will be later turn into string with Response body


## pseudo code 
1. Take string that is path to folder you want to "Auto index"
2. Check if you have access to it
	NO - (Either server internal or 403 Forbidden)
	return 
3. [Generate Html menu (file) out of that folder](#create-html-menu)


### Create Html Menu
1. Try to open folder once more   
	1. FAIL - HTTP 500
	2. return 
2. for each file in that folder 
	1. take that file and create html link out of it
	(<a href="second.html">Go to Second Page</a>)
	2. store that html link in some kind of vector
3. Create some html file in which you will iterate thorugh vector of links and put them in string for return
