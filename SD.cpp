#include "SD.h"

#include <fstream>
#include <string>

SDreader::SDreader(const SDreader &sdreader)
{

}

SDreader::SDreader()
{

}

SDreader SDreader::operator=(const SDreader &sdreader)
{
  SDreader mysdreader(sdreader);
  
  return mysdreader;
}

bool 
SDreader::insertDisk(std::string image)
{
  filename = image;
  
  file.open(filename, std::fstream::in);
  
  if (file)
  {
    std::cout << "File " << image << " is loaded" << "\n";
    file.close();
  }
  else
  {    
    std::cerr << "ERROR: File " << image << " not found" << "\n";
  }  
      
  return false;
}

bool 
SDreader::showFilesConsole()
{

  return false;
}

bool 
SDreader::initialize()
{  
  file.open(filename, std::fstream::in | std::fstream::binary);
  if (!file)
    return false;
  
  file.seekg(0x1be, )
  
  
  return false;
}


bool 
SDreader::readBasicInformation()
{
  return false;
}

