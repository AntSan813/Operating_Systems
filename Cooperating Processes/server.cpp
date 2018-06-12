/*
 * Antonio Santos
 * CSCI 411
 * compile: g++ server.cpp -o server -lrt
 * run: ./server x
 * where x is the number of processes you would like to run. must be the same as client
 *
 *
 * server.cpp: Server program
 *           to demonstrate interprocess commnuication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <cstring>
#include <iostream>

#include <string>
#include <unistd.h>

#define SERVER_QUEUE_NAME   "/AMSserver"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

using namespace std;

bool all_equal(int,string*,int);

int main (int argc, char *argv[]){

  // queue descriptors
  mqd_t qd_server, qd_client;

  //get number of client processes from command line
  int numClients = atoi(argv[1]);

  //temperature ints
  int temperature = 60;
  int totalExtTemp = 0;

  //buffers for messages.
  char in_buffer [MSG_BUFFER_SIZE];
  char out_buffer [MSG_BUFFER_SIZE];
  char temp[MSG_BUFFER_SIZE];

  //other
  string hold [numClients];
  string clients[numClients];
  int childTemps = 0;
  int counter = 0;

  cout << "Server: Hello, World!\n\n";

  //message queue attributes
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;

  //open message queue for AMSserver with defined attributes
  if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
      cerr << "Server: mq_open (server)";
      exit (1);
  }

  //following do while statement is bulk of program
  do{

    //1. get the name of client
    if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
      cerr << "Server: mq_receive";
      exit (1);
    }

    //put client name into an array
    clients[counter] = in_buffer;

    //2. get the temperature from client
    if (mq_receive (qd_server, temp, MSG_BUFFER_SIZE, NULL) == -1) {
      cerr << "Server: mq_receive";
      exit (1);
    }

    //store temperature in an array
    hold[counter] = temp;

    //output what just happenend
    cout << "Server: Temperature " << hold[counter] << " received from " << clients[counter] << "\n";

    //increment counter used to track how many messages we intake
    counter++;

    //3. if we intake x amount of messages, we want to send a message to all of them
    if(counter == numClients){
      totalExtTemp = 0;

      //following for loop calculates total external temperature
      for(int i = 0; i < numClients; i++){
        totalExtTemp = totalExtTemp + atoi(hold[i].c_str());
      }

      //calculate new internal temperature
      temperature = (2*temperature+totalExtTemp)/(numClients+2);

      // The following copies an integer into the c-string
      sprintf(out_buffer, "%d", temperature);

      sleep(1);

      //check to see if we're done
      if(all_equal(numClients,hold,temperature) != true){

        cout << "\n---------------------------------------------------------" << endl;
        cout << "Sending temperatures..." << endl;

        //if we're not done,
        //send a message to all clients
        for(int i = 0; i < numClients; i++){

          //first, open the message queue for client
          if ((qd_client = mq_open (clients[i].c_str(), O_WRONLY)) == -1) {
            cerr << "Server: Not able to open client queue";
            continue;
          }

          //now, send the temperature
          if (mq_send (qd_client, out_buffer, strlen (out_buffer), 0) == -1) {
             cerr << "Server: Not able to send message to client";
             continue;
          }

          //output message to command line saying what just happened
          cout << "Server: sent temperature " << out_buffer << " to client: " << clients[i] << endl;

          //now repeat for each client....
        }
      cout << "---------------------------------------------------------\n" << endl;

      }
      counter = 0;
    } //end of 3.

    //reset char arrays to 0
    memset(in_buffer, 0, sizeof(in_buffer));
    memset(temp, 0, sizeof(temp));

  }while(all_equal(numClients,hold,temperature) != true); //end of do while

  memset(out_buffer, 0, sizeof(out_buffer));

  //special notifier for clients that we are done, is sending them the word "done"
  out_buffer[0] = 'd';
  out_buffer[1] = 'o';
  out_buffer[2] = 'n';
  out_buffer[3] = 'e';

  //send the reply message created above
  for(int i = 0; i < numClients; i++){

    //first, open the message queue for client
    if ((qd_client = mq_open (clients[i].c_str(), O_WRONLY)) == -1) {
      cerr << "Server: Not able to open client queue";
      continue;
    }

    //now, send the end message
    if (mq_send (qd_client, out_buffer, strlen (out_buffer), 0) == -1) {
       cerr << "Server: Not able to send message to client";
       continue;
    }
  }

  return 0;
} //end of main

//following function is used to see if all temps are equal
bool all_equal(int numClients,string* client_temps,int temp){
  bool result = true;

  //go through each clients temperature and compare it to the main temp
  for(int i = 0; i < numClients; i++){
    if(atoi(client_temps[i].c_str()) == temp){
      //do nothing
    }
    else
      result = false;
  }
  return result;
}
