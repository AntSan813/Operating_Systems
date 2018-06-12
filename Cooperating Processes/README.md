I decided to go with the fork method instead of having you open a multitude of terminals for this program. The number of forks will depend on the number you pass through the command line. Per the instructions, the first 4 forks will have default temperature values.
 
 
 To run:
 
 
 On one terminal -

$ g++ server.cpp -o server -lrt

$ ./server 'number of clients'
 

 Where number of clients is the number of 'processors' you are expecting to receive temperatures from. I would put 4 for your first use. 
 
 
 On another terminal -

$ g++ client.cpp -o client -lrt

$ ./client 'number of clients'

Where number of clients is the number of 'processors' you are expecting to send temperatures from. I would put 4 for your first use. 
 

Please note, the number of clients that you pass through the command line must be the same number for ./server and ./client.
