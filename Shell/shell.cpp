/*
  Anotnio Santos
  CSCI 411 - Simple Shell program
  Due: 1/29/2018
  This program simulates a shell with 10 custom commands.
*/

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <vector>

int NUM_COMMANDS = 11;

using namespace std;

//function for signal calling
void signal_callback_handler(int signum){
   //output the signal
   printf("Caught signal %d\n",signum);
   // Terminate program
   exit(signum);
}

int main(){
  pid_t pid;
  string line;
  string x,y,z; //just some string holders.
  string hist_line;
  bool not_end = true;
  char ch;
  char buf;

  //list of commands
  string commands[] = {"help","repeat","allprocesses","myprocess","dir","clr","chgd","history","environ","hiMom","quit"};
  std::ofstream fin;
  FILE *stream;

// Register signal and signal handler
  signal(SIGINT, signal_callback_handler);

  int pipefd[2];

/* Create the pipe. */
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

/*create child*/
  pid = fork();

  if(pid < 0){
    cout <<"Fork failed\n";
    exit(-1);
  }
  else if(pid == 0){ // child process properly forked.
    cout << "Child process " << getpid() << " started!"<< endl;

/*****************following is start of shell program******************/
    while(1){
      cout << "AntShell $ ";
      int i;

//gets usser input for command
      getline(cin,line);

//keep track of history
      hist_line = "echo \"" + line + "\" >> history.txt";
      system(hist_line.c_str());

//following goes through all custom commands and prebuilt commands
      for(i = 0; i < NUM_COMMANDS; i++){
        if(line.find(commands[i]) != std::string::npos){

//my custom echo command
         if(strcmp(commands[i].c_str(),"help") == 0){
           x = line.substr(5);
           int z;
           for(z = 0; z < NUM_COMMANDS; z++){
             if(strcmp(commands[z].c_str(),x.c_str()) == 0){
               if(commands[z] == "myprocess"){
                 system("echo Return the current process ID from the getpid function");
               }
               else if(!commands[z].compare("allprocesses")){
                 system("echo Return all current processes");
               }
               else if(!commands[z].compare("chgd")){
                 system("echo Changes the current working directory to specified directory");
               }
               else if(!commands[z].compare("clr")){
                 system("echo Clears the screen ");
               }
               else if(!commands[z].compare("dir")){
                 system("echo Lists the contents of directory");
               }
               else if(!commands[z].compare("environ")){
                 system("echo Lists all the environment strings");
               }
               else if(!commands[z].compare("hiMom")){
                 system("echo The child process will then send a message to the parent. This message will be sent through pipes.");
               }
               else if(!commands[z].compare("quit")){
                 system("echo Quits the shell");
               }
               else if(!commands[z].compare("help")){
                 system("echo Displays the user manual");
               }
               z = NUM_COMMANDS + 1;
             }

//if we went through my commands and none was found, run the systems man
             if(z == NUM_COMMANDS - 1){
               y = "man " + x;
               cout << y << endl;
               system(y.c_str());
             }
           }
         }

//my custom clear screen command
          else if(strcmp(line.c_str(),"clr") == 0){
            system("clear");
          }

//my custom exit command
          else if(strcmp(line.c_str(),"quit") == 0){
            system("echo have a grand ole day!");
            exit(0);
          }

//my custom ps -a command
          else if(strcmp(line.c_str(),"allprocesses") == 0){
            system("ps -a");
          }

//my custom dir command
          else if(strcmp(commands[i].c_str(),"dir") == 0){
            x = line.substr(4); //go to position where dir name is
            y = "ls -al " + x; //concatenate the command
            system(y.c_str()); //execute the command
          }

//my custom env command
          else if(strcmp(line.c_str(),"environ") == 0){
            system("env");
          }

//my custom ps command
          else if(strcmp(line.c_str(),"myprocess") == 0){
            ostringstream con;
            con << getpid(); //convert int to string
            x = "echo " + con.str(); //concatenate command
            system(x.c_str()); //execute command
          }

//command sends a message to the parent process
          else if(strcmp(line.c_str(),"hiMom") == 0){
            close(pipefd[0]); // close the read-end of the pipe, I'm not going to use it
            string msg = "Hi! I am the child process communicating with the parent process.";
            write(pipefd[1], msg.c_str(), msg.length()); // send the content of argv[1] to the reader
            close(pipefd[1]); // close the write-end of the pipe, thus sending EOF to the reader
         }
//my custom cd command
          else if(strcmp(commands[i].c_str(),"chgd") == 0){
            x = line.substr(5);
            int ret = chdir(x.c_str());
            if(ret == -1){
              system("echo Directory does not exist.");
            }
          }

//my custom echo command
          else if(strcmp(commands[i].c_str(),"repeat") == 0){
            x = line.substr(6);
            y = "echo " + x;
            system(y.c_str());
         }

//my custom history command
          else if(strcmp(line.c_str(), "history") == 0){
            system("cat history.txt");
          }
//following else if is for commands that are not custom made by me.
         else if(strcmp(line.c_str(),commands[i].c_str()) == 0){
           system(line.c_str());
         }

//error message for if command does not exist
          else{
            system("echo Command does not exist.");
          }

//exit for statement if we have found the command we were looking for.
          i = NUM_COMMANDS + 1;
        }

//if we did not find command we were looking for, run it anyway to see if it is
//a prebuilt command. if it is not, the prebuilt command error message will show.
        if(i == NUM_COMMANDS - 1){
          system(line.c_str());
        }
      }
    }
  }
  else {
    wait(NULL);
    close(pipefd[1]); // close the write-end of the pipe, I'm not going to use it
    while (read(pipefd[0], &buf, 1) > 0) // read while EOF
      write(1, &buf, 1);
    write(1, "\n", 1);
    close(pipefd[0]); // close the read-end of the pipe
    cout << "Shell process, " << getpid() << ", is ending. Bye." << endl;
    exit(EXIT_SUCCESS);
  }
  return EXIT_SUCCESS;

}
