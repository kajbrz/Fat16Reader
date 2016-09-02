#ifndef  CLASS_SD_H
#define CLASS_SD_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>


const uint32_t PARTITION_TABLE_START = 0x1BE;
const uint32_t SECTOR_SIZE = 512;
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
    
    //size of this structure have to be 0x200 
    //attribute packet works correctly with flag: -mno-ms-bitfields
    typedef struct 
    {
      uint8_t jump[3];                //0x000
      int8_t oem[8];                  //0x003
      uint16_t sectorSize;            //0x00B
      uint8_t sectersPerCluster;      //0x00D
      uint16_t reservedSectors;       //0x00E
      uint8_t numberOfFats;           //0x010
      uint16_t rootDirEntries;        //0x011
      uint16_t totalSectorsShort;     //0x13
      uint8_t mediaDescriptor;        //0x015
      uint16_t fatSizeSectors;        //0x016
      uint16_t sectorsPerTrack;       //0x18
      uint16_t numberOfHeads;         //0x01a
      uint32_t hiddenSectors;         //0x01c
      uint32_t totalSectorsLong;      //0x20
      //here should be 16 bytes of data for FAT32 only
      //for other you avoid this
      uint8_t driveNumber;            //0x020
      uint8_t currentHead;            //0x026
      uint8_t bootSignature;          //0x027
      uint32_t volume_id;             //0x028
      int8_t volumeLabel[11];         //0x2b
      int8_t fsType[8];               //0x36
      int8_t bootCode[448];           //0x03e
      uint16_t bootSectorSignature;   //0x1fe
    } __attribute__((packed)) BootSector; //FAT16
    
    typedef struct 
    {
      uint8_t filename[8];
      uint8_t ext[3];
      uint8_t attributes;
      uint8_t reserved[10];
      uint16_t modifyTime;
      uint16_t modifyDate;
      uint16_t startingCluster;
      uint32_t fileSize;
    } __attribute((packed)) Fat16Entry;  
        
  public:
    SDreader(const SDreader&);
    SDreader();    
    SDreader operator=(const SDreader&);

    bool insertDisk(std::string);
    bool initialize();
    bool showFilesConsole();
    bool showFile(size_t which);
    bool copyFileToDirectory(size_t wich, std::string path);
    
  private:
    bool active = false;
    std::fstream file; 
    std::string filename;  
    
    std::vector <PartitionTable> partitionTables;
    std::vector <BootSector> bootSectors;
    std::vector <Fat16Entry> fat16Entries;
    
    bool readPartition();    
    bool readBootSectors();
    bool readRootSector();
    
    void showPartion();
    void showBootSectors();  
};
#endif // CLASS_SD_H
