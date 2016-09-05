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
    printf("Partition %lu, type 0x%x\n", i, partitionTables[i].partitionType);
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
    
    std::cout << "\t\t: Now we are in byte: " << std::hex << SECTOR_SIZE * partitionTables[i].startSector;
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
    printf("\nPartition : %lu\n", i);
    printf("jump : %X:%X:%X\n", 
      bootSectors[i].jump[0], 
      bootSectors[i].jump[1],
      bootSectors[i].jump[2]);
      
    printf("rootDirEntries : %hu\n", bootSectors[i].rootDirEntries);
    printf("totalSectorsShort : %hu\n", bootSectors[i].totalSectorsShort);
    printf("mediaDescriptor : %hd\n", bootSectors[i].mediaDescriptor);
    printf("sectorsPerTrack : %hu\n", bootSectors[i].sectorsPerTrack);    
    printf("numberOfHeads : %hu\n", bootSectors[i].numberOfHeads);
    printf("hiddenSectors : %u\n", bootSectors[i].hiddenSectors);
    printf("totalSectorsLong : %u\n", bootSectors[i].totalSectorsLong);
    
    printf("  reservedSectors : %u\n", bootSectors[i].reservedSectors);
    printf("  fatSizeSectors : %hhu\n", bootSectors[i].fatSizeSectors);
    printf("  numberOfFats : %hhu\n", bootSectors[i].numberOfFats);
    printf("  sectorSize : %u\n", bootSectors[i].sectorSize);      
    printf("  sectersPerCluster : %u\n", bootSectors[i].sectersPerCluster);
    
    printf("Partition : %.8s\n", bootSectors[i].oem);    
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
  
  std::cout << "\t\t: Now we are in byte: " 
    << std::hex << 
      (SECTOR_SIZE * partitionTables[0].startSector + 
      (bootSectors[0].reservedSectors - 1 +
      bootSectors[0].fatSizeSectors * 
      bootSectors[0].numberOfFats) * bootSectors[0].sectorSize) << std::endl;
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
      
    std::cerr << "TRY ENTRY: " << i;
    fat16Entries.push_back(temporary16Entry);
    
    printf("File %lu: [%.8s.%.3s]\n", i,
      fat16Entries[i].filename, fat16Entries[i].ext); 
  }
  
  return false;
}

bool 
SDreader::showFile(size_t which) //from root
{
  if (!active)
  {
    std::cerr << "ERROR: Not Active" << std::endl;
    return false;
  }
  if (which >= fat16Entries.size())
  {  
    std::cerr << "ERROR: fat16Entries Wrong" << std::endl;
    return false;
  }
  uint8_t hour    = (fat16Entries[which].modifyTime & 0xf800) >> (5+6);  
  uint8_t minutes = (fat16Entries[which].modifyTime & 0x07e0) >> (5);  
  uint8_t seconds = (fat16Entries[which].modifyTime & 0x001f) << 1;  
  
  printf("\n[%.8s.%.3s]\n", 
    fat16Entries[which].filename, fat16Entries[which].ext);  
  
  std::cout << "Attributes: " << std::hex << fat16Entries[which].attributes << std::endl
    << "Modify Time: " << std::dec
      << static_cast<unsigned int>(hour)    << ":" 
      << static_cast<unsigned int>(minutes) << ":" 
      << static_cast<unsigned int>(seconds) << std::endl   
    << "Modify Date: " << std::hex << fat16Entries[which].modifyDate << std::endl
    << "Starting Cluster: " << std::hex << fat16Entries[which].startingCluster << std::endl
    << "File Size: " << std::hex << fat16Entries[which].fileSize << std::endl;
  
  return true;
}

uint32_t 
SDreader::clusterToSector(uint32_t cluster)
{
  unsigned il; 
  il= cluster-2; 
  //il= il * sectorsPerCluster; 
  //il= il + dataStart; 
  
  return(il);      
}

bool 
SDreader::copyFileToDirectory(size_t which, std::string path)
{
  //uint32_t sector = clusterToSector(fat16Entries[which].startingCluster);
  uint32_t dataStartBlock = 
      (SECTOR_SIZE * partitionTables[which].startSector + 
      (bootSectors[which].reservedSectors - 1 +
      bootSectors[which].fatSizeSectors * 
      bootSectors[which].numberOfFats) * bootSectors[which].sectorSize)
      + (bootSectors[which].sectersPerCluster * bootSectors[which].rootDirEntries) + ((fat16Entries[which].startingCluster - 1) * bootSectors[which].sectersPerCluster * bootSectors[which].fatSizeSectors);
      
  const uint32_t countBuff = fat16Entries[which].fileSize;
  char buff[countBuff];
  file.seekg(dataStartBlock, std::ios_base::beg);
  
  std::cout << "\t\t: Now we are in byte: " 
    << std::hex << dataStartBlock << std::endl;
  
  file.read(buff, countBuff);
  std::cout << buff << std::endl;
    
  return true;
}

