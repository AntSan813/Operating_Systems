/*
 * Antonio Santos
 * CSCI 411
 * compile: g++ client.cpp -o client -lrt
 * after compiling the server and running the server program,
 * run: ./client x
 * where x is the number of processes you would like to run. must be the same as server
 *
 *
 * Adapted from  Softpryaog
 *    https://www.softprayog.in/programming/interprocess-communication-using-posix-message-queues-in-linux
 * by MA Doman 2018
 * client.c: Client program
 *           to demonstrate interprocess communication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <cstring>
#include <iostream>

#include <sstream>
#include <sys/wait.h>
#include <cstdlib>

#define SERVER_QUEUE_NAME   "/AMSserver"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

using namespace std;

int do_work(pid_t,int, int, int);

int main (int argc, char *argv[]){
  pid_t pid;

  //get number of client processes to be ran from command line
  int numClients = atoi(argv[1]);

  //initial fork
  pid = fork();
  int temperature = 100;
  int pnum = 1;

  //main process will fork x - 1 amount of times.
  //each loop, the new child will get an initial temp, and a unique process number.
  //for testing purposes, there is a sleep statement to see it all happen.
  for(int i = 0; i < numClients - 1; i++){
    if(pid != 0){
      sleep(2);
      pid = fork();
      pnum++;
    }
    else //not a new child, break
      break;

    //get process a temp. first 4 are default, any process after that is random between 1-100
    if(i == 0)
      temperature = 22;
    else if(i == 1)
      temperature = 50;
    else if(i == 2)
      temperature = 40;
    else
      temperature = rand()%100 + 1;

  }

  if(pid==0){ //all children will run this
    do_work(pid,temperature,pnum,numClients);
  }
  else{ //parent process will run here
    for(int i = 0; i < numClients; i++){
      wait(NULL); //wait for each child process to finish
    }
    exit(EXIT_SUCCESS); //then exit
  }
}

//function does all the grunt work for this program
int do_work(pid_t pid, int temperature, int pnum, int numClients){

  //key is what tells us when to stop
  char key[] = "done";

  mqd_t qd_server, qd_client;   // queue descriptors

  //initial main temperature
  int maintemp = 60;

  // create the client queue for receiving messages from server
  char client_queue_name [64];

  //get pid and convert it to string
  stringstream ss;
  ss << getpid();
  string mypid;
  ss >> mypid;

  //now concatenate everything into one name for client
	string  str_client_queue_name = "/client-" + mypid + "\\0'";
	strcpy(client_queue_name, str_client_queue_name.c_str());

  //create message queue attributes
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;

  if ((qd_client = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
    cerr<<"Client: mq_open (client)" ;
    exit (1);
  }

  if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
    cerr<< qd_server << " Client: mq_open (server)";
    exit (1);
  }

  char in_buffer [MSG_BUFFER_SIZE];

  char temp_buf [10];

  do{

    sprintf(temp_buf, "%d", temperature);

    //sleep for pnum seconds and then execute the following. this allows us to
    //syncronize each process.
    sleep(pnum);

    // send name to server
    if (mq_send (qd_server, client_queue_name , strlen(client_queue_name), 0) == -1) {
         cerr<<"Client: Not able to send message to server";
        continue;
    }

    //output what happened
    cout << getpid() << ": " << temp_buf << endl;

    //Send temp to server
    if (mq_send (qd_server, temp_buf , strlen(temp_buf), 0) == -1) {
         cerr<<"Client: Not able to send message to server";
        continue;
    }

    // receive response from server
    if (mq_receive (qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
         cerr<<"Client: mq_receive";
        exit (1);
    }

    sleep (1);

    //update main temp
    maintemp = atoi(in_buffer);

    //calculate new external temperature
    temperature = (temperature*3+2*maintemp)/(numClients+1);

  }while(strcmp(key,in_buffer) != 0);//keep going if key and message sent from server dont match

  //close message queue
  if (mq_close (qd_client) == -1) {
    cerr<<"Client: mq_close";
    exit (1);
  }

  //unlink message queue
  if (mq_unlink (client_queue_name ) == -1) {
    cerr<<"Client: mq_unlink";
    exit (1);
  }

  //output bye message
  cout << "Client: bye\n";

  return 1;
}
