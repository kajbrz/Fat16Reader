#include <string>
#include <iostream>
#include "SD.h"

int
main(int argv, char *arg[])
{
  if (argv != 2)
  {
    std::cerr << "Error: not correct argument \n <programname> <img_file> \n" << std::endl;
    //exit(1);
    return 1;
  }
  
  SDreader sdreader;
  if(!sdreader.insertDisk(arg[1]))
  {
    //exit(2);   
    std::cerr << "Error: Failed read image \n" << std::endl;
    return 2;
  }
  
  if(!sdreader.initialize())
  {
    //exit(3);
    std::cerr << "Error: Failed initialize image\n" << std::endl;
    return 3;
  }
  
  if (!sdreader.showFile(0))
  {
    std::cerr << "Error: Cannot find file\n" << std::endl;  
    return 4;
  }
  
  if (!sdreader.copyFileToDirectory(0, "myFile.txt"))
  {
    std::cerr << "Error: Cannot copy file" << std::endl;
    return 5;
  }
  
}
