#include "SD.h"

#include <fstream>
#include <string>
#include <cstdio>

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
    return false;
  }  
      
  return true;
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
  
  file.seekg(PARTITION_TABLE_START, std::ios_base::beg);
  file.read(reinterpret_cast<char*>(partitionTable), sizeof(PartitionTable) * 4); 
  
  for (size_t i = 0; i < 4; i++)
  {
    printf("Partition %d, type 0x%x\n", i, partitionTable[i].partitionType);
    printf("\tStart sector 0x%x, %d sectors long\n", 
      partitionTable[i].startSector, partitionTable[i].lengthSector);  
  }
    
  return true;
}


bool 
SDreader::readBasicInformation()
{
  return false;
}

