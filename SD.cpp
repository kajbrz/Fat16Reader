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

  readPartition();
  showPartion();
  
  file.close();
  
  file.open(filename, std::fstream::in | std::fstream::binary);
  if (!file)
    return false;

  readBootSectors();
  showBootSectors();
  
  active = true;
    
  return true;
}
    

void 
SDreader::showPartion()
{
  for (size_t i = 0; i < partitionTables.size(); i++)
  {
    printf("Partition %d, type 0x%x\n", i, partitionTables[i].partitionType);
    printf("\tStart sector 0x%x, %d sectors long\n", 
      partitionTables[i].startSector, partitionTables[i].lengthSector);  
  }     
}

bool 
SDreader::readPartition()
{   
  if (!file)
    return false;
    
  PartitionTable temporaryPartitionTable;  
  file.seekg(PARTITION_TABLE_START, std::ios_base::beg);
  
  for (;;)
  {
    file.read(reinterpret_cast<char*>(&temporaryPartitionTable), 
      sizeof(PartitionTable)); 
  
    if (temporaryPartitionTable.partitionType == 0)
      break;
    partitionTables.push_back(temporaryPartitionTable);
  }
}

bool 
SDreader::readBootSectors()
{
  bool err = false;
  if (!file)
    return false;
  
  BootSector temporaryBootSector; //Fat16
    
  for (size_t i = 0; i < partitionTables.size(); i++)
  {    
    switch (partitionTables[i].partitionType) 
    {
      case 4:
      case 6:
      case 14:
        break;
      default:
        err = true;
    }
    
    if (err)
    {
      err = false;
      continue;
    }
    
    file.seekg(SECTOR_SIZE * partitionTables[i].startSector, std::ios_base::beg);
    file.read(reinterpret_cast<char *>(&temporaryBootSector), sizeof(BootSector));
    
    bootSectors.push_back(temporaryBootSector);
  }
  
  return false;
}

void 
SDreader::showBootSectors()
{
  for (size_t i = 0; i < bootSectors.size(); i++)
  {
    printf("Partition : %d\n", i);
    printf("jump : %X:%X:%X\n", 
      bootSectors[i].jump[0], 
      bootSectors[i].jump[1],
      bootSectors[i].jump[2]);
    
    
    printf("Partition : %8s\n", bootSectors[i].oem);    
    printf("Volumne label: [%.11s]\n", bootSectors[i].volumeLabel);
    printf("File system label: [%.8s]\n", bootSectors[i].fsType);
  }
}

bool 
readRootSector()
{


  return false;
}
