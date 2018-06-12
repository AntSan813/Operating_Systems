#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;
int main(){
  system("echo HOSTNAME");
  system("hostname");
  system("echo NUMBER OF PROCESSORS");
  system("cat /proc/cpuinfo | grep \"processor\" | wc -l");
  system("echo CPU TYPE AND MODEL");
  system("cat /proc/cpuinfo | grep \"vendor\"");
  system("cat /proc/cpuinfo | grep \"model name\"");
  system("echo LINUC KERNEL VERSION");
  system("uname -or");
  system("echo SYSTEM TIME");
  system("awk '{print int($1/3600)\":\"int(($1%3600)/60)\":\"int($1%60)}' /proc/uptime");
  system("echo MEMORY INFO");
  system("cat /proc/meminfo | grep \"MemTotal\"");
  system("cat /proc/meminfo | grep \"MemFree\"");

 return 0;

}
