/*
  Anotnio Santos
  CSCI 411 - implementing the ls command
  Due: 3/28/2018
  this program replicates the ls -la command
*/
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <pwd.h>
#include <grp.h>

using namespace std;

int main(int argc, char **argv){
  //validate command input
  if(argc > 2){
    cout << "invalid command"<< endl;
    return 0;
  }
  string dir;

  //make sure we know which directory we are working with
  if(argc == 1){
    dir = ".";
  }
  else{
    dir = argv[1];
  }

  //variables
  DIR *directory; //browsing folder
  struct dirent *thefile; //file found in folder
  struct stat thestat; //used to retreive info about file
  struct passwd *tf; // used to determine owner
  struct group *gf; //used to determine group

  char buff[512]; //placeholder for string
  directory = opendir(dir.c_str()); //opens the directory

  //if file is found, go through loop until all files have been processed.
  while((thefile = readdir(directory)) != NULL){
    //defines path to our file
    sprintf(buff, "%s/%s", dir.c_str(), thefile->d_name);
    //function used to retrieve info
    stat(buff, &thestat);

    //analyzes info
    //[file type]
    switch (thestat.st_mode & S_IFMT) {
      case S_IFBLK:  printf("b "); break;
      case S_IFCHR:  printf("c "); break;
      case S_IFDIR:  printf("d "); break; //It's a (sub)directory
      case S_IFIFO:  printf("p "); break; //fifo
      case S_IFLNK:  printf("l "); break; //Sym link
      case S_IFSOCK: printf("s "); break;
      //Filetype isn't identified
      default:       printf("- "); break;
    }

    //prints analyzed info [file permissions]
    printf( (thestat.st_mode & S_IRUSR) ? " r" : " -");
    printf( (thestat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (thestat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (thestat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (thestat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (thestat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (thestat.st_mode & S_IROTH) ? "r" : "-");
    printf( (thestat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (thestat.st_mode & S_IXOTH) ? "x" : "-");

    //[number of hard links]
    printf("%d", thestat.st_nlink);

    //[owner]
    tf = getpwuid(thestat.st_uid);
    printf("\t%s ", tf->pw_name);

    //[group]
    gf = getgrgid(thestat.st_gid);
    printf("\t%s ", gf->gr_name);

    //[size in bytes]
    printf("%zu",thestat.st_size);
    printf(" %s", thefile->d_name);
    printf(" %s", ctime(&thestat.st_mtime));

  }
  //close directory
  closedir(directory);


}
