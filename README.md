# What is this repo ?
This repository is a simple database which can store employees in a database file and view them.

# How to use it ?
Compile the repo by `make`,  
delete all db files with `make reset`,  
compile and test with `make test`  

General usage:  
bin/main -n -a <employee name,employee address,employee hours> -f <databasefile> -l   
-f  - (required) path to the database file  
-n  - create new database file  
-a  - add new employee to the database  
-l  - list employees in the database file   
