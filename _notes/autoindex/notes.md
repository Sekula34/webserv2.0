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
3. Generate Html menu (file) out of that folder
