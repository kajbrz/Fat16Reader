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
  
  readRootSector();
  
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
        std::cerr << "ERROR: Not supported file system" << std::endl;
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
    printf("\nPartition : %d\n", i);
    printf("jump : %X:%X:%X\n", 
      bootSectors[i].jump[0], 
      bootSectors[i].jump[1],
      bootSectors[i].jump[2]);
      
    printf("numberOfFats : %hhd\n", bootSectors[i].numberOfFats);
    printf("rootDirEntries : %hd\n", bootSectors[i].rootDirEntries);
    printf("totalSectorsShort : %hd\n", bootSectors[i].totalSectorsShort);
    printf("mediaDescriptor : %hd\n", bootSectors[i].mediaDescriptor);
    printf("fatSizeSectors : %hhd\n", bootSectors[i].fatSizeSectors);
    printf("sectorsPerTrack : %hd\n", bootSectors[i].sectorsPerTrack);
    printf("numberOfHeads : %hd\n", bootSectors[i].numberOfHeads);
    printf("hiddenSectors : %d\n", bootSectors[i].hiddenSectors);
    printf("totalSectorsLong : %d\n", bootSectors[i].totalSectorsLong);
      
    printf("Partition : %8s\n", bootSectors[i].oem);    
    printf("Volumne label: [%.11s]\n", bootSectors[i].volumeLabel);
    printf("File system label: [%.8s]\n\n", bootSectors[i].fsType);
  }
}

bool 
SDreader::readRootSector()
{
  if (partitionTables.size() != 1)
  {
    std::cerr << "ERROR: Support only exactly one partition" << std::endl;
    return false;
  }
  
  Fat16Entry temporary16Entry;
  
  file.seekg(
    (bootSectors[0].reservedSectors - 1 +
    bootSectors[0].fatSizeSectors * 
    bootSectors[0].numberOfFats) * bootSectors[0].sectorSize,
    std::ios_base::cur); 
    
  for (size_t i = 0; i < bootSectors[0].rootDirEntries; i++)
  {
    file.read(reinterpret_cast<char *>(&temporary16Entry), sizeof(Fat16Entry));
    
    if (temporary16Entry.filename[0] == '\0')
      continue;
      
    fat16Entries.push_back(temporary16Entry);
    
    printf("File %d: [%.8s.%.3s]\n", i,
      fat16Entries[i].filename, fat16Entries[i].ext); 
  }
  
  return false;
}

bool 
SDreader::showFile(size_t which) //from root
{
  if (!active)
    return false;
  
  if (which >= fat16Entries.size())
    return false;
    
  printf("[%.8s.%.3s]\n", 
    fat16Entries[which].filename, fat16Entries[which].ext);  
  
  std::cout << "Attributes: " << std::hex << fat16Entries[which].attributes << std::endl
    << "Modify Time: " << std::hex << fat16Entries[which].modifyTime << std::endl
    << "Modify Date: " << std::hex << fat16Entries[which].modifyDate << std::endl
    << "Starting Cluster: " << std::hex << fat16Entries[which].startingCluster << std::endl
    << "File Size: " << std::hex << fat16Entries[which].fileSize << std::endl;
  
  return true;
}

bool 
SDreader::copyFileToDirectory(size_t wich, std::string path)
{
  

  return false;
}
