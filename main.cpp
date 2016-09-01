#include <string>
#include <iostream>
#include "SD.h"

int
main(int argv, char *arg[])
{
  if (argv != 2)
  {
    std::cerr << "Error: not correct argument \n <programname> <img_file> \n";
    exit(1);
  }
  
  SDreader sdreader;
  if(!sdreader.insertDisk(arg[1]))
  {
    exit(2);
  }
  
  if(!sdreader.initialize())
  {
    exit(3);
  }
  
}
