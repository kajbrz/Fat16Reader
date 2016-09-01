#ifndef  CLASS_SD_H
#define CLASS_SD_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

#define 

class SDreader 
{
  private:
    typedef struct 
    {
      uint8_t firstByte;
      uint8_t startChs[3];
      uint8_t partitionType;
      uint8_t endChs[3];
      uint32_t startSector;
      uint32_t lengthSector;
    } __attribute((packed)) PartitionTable;
    
  public:
    SDreader(const SDreader&);
    SDreader();    
    SDreader operator=(const SDreader&);

    bool insertDisk(std::string);
    bool showFilesConsole();
    bool initialize();
  private:
    bool active = false;
    std::fstream file;
    std::string filename;
    PartitionTable partitionTable[4];
    
    bool readBasicInformation();
    
    
};
#endif // CLASS_SD_H
