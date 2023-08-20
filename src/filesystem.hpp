#include <string>
#include <ctime>
#include <iostream>
#include <cstring>

int fatEntryCount = 1024 * 4;   //fat tableda kaç tane entry var
int blockSize;
int maxBlockSize = (1024 * 1024 * 16) / fatEntryCount;  // 16 MB/ 4k toplam block sayısı
int dirEntryCount;

class DirectoryEntry {
    public:
        char filename[8];
        char lastModfDate[12];
        int time;
        int firstblocknumber;
        short size;   
};
class DirectoryTable {
    public:
        DirectoryEntry *directoryEntry;
        
        DirectoryTable()
        {
            this->entryCount = dirEntryCount;
            directoryEntry = new DirectoryEntry[entryCount]; 
            time_t now = time(0);
            for (int i = 0; i < entryCount; i++) {
                directoryEntry[i].filename[0] = '\0';
                std::string dateTimeString = std::ctime(&now);
                std::string timeString = dateTimeString.substr(11, 8); // Saat, dakika ve saniye bilgilerini alır
                std::strcpy(directoryEntry[i].lastModfDate, timeString.c_str());
                directoryEntry[i].time = 0;
                directoryEntry[i].firstblocknumber = 0;
                directoryEntry[i].size = 10;
            }
        }
        int getEntryCount() {
            return entryCount;
        }
    private:
        int entryCount;
};
class Fat12Entry {
    public:
        short nextBlockNumber;
};
class Fat12Table {
    public:
        Fat12Entry *fat12Entry;

        Fat12Table(int fatEntryCount) {
            fat12Entry = new Fat12Entry[fatEntryCount]; // Allocate memory for fat12Entry array

            for (int i = 0; i < fatEntryCount; i++) {
                fat12Entry[i].nextBlockNumber = -2; // Initialize each element to -2,free blocks 
            }
    }
};
class SuperBLock {
    public:
        int blockSize;
        int rootDirPosition;
        int blockCount;
        int freeBlockCount;
        int filesCount;
};

char **data; //data blockları ve içindeki veriler