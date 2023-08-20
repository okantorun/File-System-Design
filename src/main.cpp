#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include "filesystem.hpp"

std::vector<std::string> splitPath(const std::string &path, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(path);

    while (std::getline(ss, token, '\\'))
    {
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }
    return tokens;
}

void printDirectoryTable(DirectoryTable *directoryTable, int index)
{
    
    for (int i = 0; i < directoryTable[index].getEntryCount(); i++)
    {
        if (directoryTable[index].directoryEntry[i].filename[0] != '\0')
        {
            std::cout << "FileName "
              << "Size "
              << "FirstBlockNumber "
              << "Last Modified Time " << std::endl;
            std::cout << directoryTable[index].directoryEntry[i].filename << "     " << directoryTable[index].directoryEntry[i].size
                      << "          " << directoryTable[index].directoryEntry[i].firstblocknumber << "          "
                      << directoryTable[index].directoryEntry[i].lastModfDate << std::endl;
        }
    }
}
void initializeData(int blockSize)
{
    data = new char *[fatEntryCount];
    for (int i = 0; i < fatEntryCount; i++)
    {
        data[i] = new char[blockSize]; // Allocate memory for each data block
    }
    //std::cout << fatEntryCount << " data blocks are created" << std::endl;
}

void dirCommand(std::string path, Fat12Table &fatTable, DirectoryTable *directoryTable, int blockWithoutDir)
{   
    int isInvalidPath = 0;
    std::vector<std::string> tokens;
    tokens = splitPath(path, '\\');
    if (tokens.size() == 0)
    {
        // root directoryde oluştur
        printDirectoryTable(directoryTable, 0);
    }
    else
    {
        // root directoryde değilse
        int firstBlockNumber = 0;
        for (int i = 0; i < tokens.size(); i++)
        {
            if (i == 0)
            {
                for (int j = 0; j < directoryTable[0].getEntryCount(); j++)
                {
                    if (strcmp(directoryTable[0].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                    {
                        firstBlockNumber = directoryTable[0].directoryEntry[j].firstblocknumber;
                        if (i == tokens.size() - 1)
                        {
                            int targetDir = firstBlockNumber - blockWithoutDir;
                            printDirectoryTable(directoryTable, targetDir);
                        }
                        isInvalidPath = 1;
                        break;
                    }
                }
            }
            else if (i == tokens.size() - 1)
            {
                int targetDir = firstBlockNumber - blockWithoutDir;
                for (int j = 0; j < directoryTable[targetDir].getEntryCount(); j++)
                {
                    if (strcmp(directoryTable[targetDir].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                    {
                        firstBlockNumber = directoryTable[targetDir].directoryEntry[j].firstblocknumber;
                        if (i == tokens.size() - 1)
                        {
                            int targetDir = firstBlockNumber - blockWithoutDir;
                            printDirectoryTable(directoryTable, targetDir);
                        }
                        isInvalidPath = 1;
                        break;
                    }
                }
            }
            else
            {
                int targetDir = firstBlockNumber - blockWithoutDir;
                for (int j = 0; j < directoryTable[targetDir].getEntryCount(); j++)
                {
                    if (strcmp(directoryTable[targetDir].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                    {
                        firstBlockNumber = directoryTable[targetDir].directoryEntry[j].firstblocknumber;
                        isInvalidPath = 1;
                        break;
                    }
                }
            }

            if(isInvalidPath == 0)
            {
                std::cout<<"Invalid Path !"<<std::endl;
                break;
            }
            isInvalidPath = 0;
        }
    }
}

int isNameUniqueInDir(std::string name, DirectoryTable *directoryTable, int index)
{
    for (int i = 0; i < directoryTable[index].getEntryCount(); i++)
    {
        if (strcmp(directoryTable[index].directoryEntry[i].filename, name.c_str()) == 0)
        {
            return 0;
        }
    }
    return 1;
}
void updateSizeOfDir(std::string path, Fat12Table &fatTable, DirectoryTable *directoryTable, 
                                                        SuperBLock &superBlock,int blockWithoutDir,std::string timeString)
{
    std::vector<std::string> tokens;
    tokens = splitPath(path, '\\');
    int firstBlockNumber = 0;
        for (int i = 0; i < tokens.size(); i++)
        {
            if (i == 0)
            {
                for (int j = 0; j < directoryTable[0].getEntryCount(); j++)
                {
                    if (strcmp(directoryTable[0].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                    {
                        firstBlockNumber = directoryTable[0].directoryEntry[j].firstblocknumber;
                        directoryTable[0].directoryEntry[j].size += sizeof(DirectoryEntry);
                        std::strcpy(directoryTable[0].directoryEntry[j].lastModfDate, timeString.c_str());
                        break;
                    }
                }
            }
            // Eklenmek istenen directory için
            else if (i == tokens.size() - 1)
            {
                break;
            }
            else
            {
                int targetDir = firstBlockNumber - blockWithoutDir;
                for (int j = 0; j < directoryTable[targetDir].getEntryCount(); j++)
                {
                    if (strcmp(directoryTable[targetDir].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                    {
                        firstBlockNumber = directoryTable[targetDir].directoryEntry[j].firstblocknumber;
                        directoryTable[targetDir].directoryEntry[j].size += sizeof(DirectoryEntry);
                        std::strcpy(directoryTable[targetDir].directoryEntry[j].lastModfDate, timeString.c_str());
                        break;
                    }
                }
            }
        }
}
void mkdirCommand(std::string path, Fat12Table &fatTable, DirectoryTable *directoryTable, 
                                                                SuperBLock &superBlock,int blockWithoutDir)
{
    int isInvalidPath = 0;
    int uniqueNameControl = 0;
    std::vector<std::string> tokens;
    tokens = splitPath(path, '\\');
    if (tokens.size() == 1)
    {
        // root directoryde oluştur
        int firstBlockNumber = 0;

        for (int i = 0; i < fatEntryCount; i++)
        {
            if (fatTable.fat12Entry[i].nextBlockNumber == -2)
            {
                fatTable.fat12Entry[i].nextBlockNumber = -1;
                firstBlockNumber = i;
                break;
            }
        }
        for (int i = 0; i < directoryTable[0].getEntryCount(); i++)
        {
            if (directoryTable[0].directoryEntry[i].filename[0] == '\0')
            {
                int control = isNameUniqueInDir(tokens[0], directoryTable, 0);
                if(control == 0)
                {
                    std::cout<<"Name is not unique !"<<std::endl;
                    fatTable.fat12Entry[firstBlockNumber].nextBlockNumber = -2;
                    uniqueNameControl = 1;
                    break;
                }
                else
                {
                    strcpy(directoryTable[0].directoryEntry[i].filename, tokens[0].c_str());
                    directoryTable[0].directoryEntry[i].firstblocknumber = firstBlockNumber;
                    directoryTable[0].directoryEntry[i].size = 0;
                    time_t now = time(0);
                    std::string dateTimeString = std::ctime(&now);
                    std::string timeString = dateTimeString.substr(11, 8); // Saat, dakika ve saniye bilgilerini alır
                    std::strcpy(directoryTable[0].directoryEntry[i].lastModfDate, timeString.c_str());
                    superBlock.freeBlockCount = superBlock.freeBlockCount - 1; 
                    superBlock.filesCount = superBlock.filesCount + 1;
                    break;
                }
                
            }
        }
    }
    else
    {
        // root directoryde değilse
        int firstBlockNumber = 0;
        for (int i = 0; i < tokens.size(); i++)
        {
            if (i == 0)
            {
                for (int j = 0; j < directoryTable[0].getEntryCount(); j++)
                {
                    if (strcmp(directoryTable[0].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                    {
                        firstBlockNumber = directoryTable[0].directoryEntry[j].firstblocknumber;
                        isInvalidPath = 1;
                        break;
                    }
                }
            }
            // Eklenmek istenen directory için
            else if (i == tokens.size() - 1)
            {
                int targetDir = firstBlockNumber - blockWithoutDir;
                int firstBlockNumber = 0;
                for (int k = 0; k < fatEntryCount; k++)
                {
                    if (fatTable.fat12Entry[k].nextBlockNumber == -2)
                    {
                        fatTable.fat12Entry[k].nextBlockNumber = -1;
                        firstBlockNumber = k;
                        break;
                    }
                }
                for (int k = 0; k < directoryTable[targetDir].getEntryCount(); k++)
                {
                    if (directoryTable[targetDir].directoryEntry[k].filename[0] == '\0')
                    {
                        int control = isNameUniqueInDir(tokens[i], directoryTable, targetDir);
                        if(control == 0)
                        {
                            std::cout<<"Name is not unique !"<<std::endl;
                            fatTable.fat12Entry[firstBlockNumber].nextBlockNumber = -2;
                            uniqueNameControl = 1;
                            break;
                        }
                        else
                        {
                            strcpy(directoryTable[targetDir].directoryEntry[k].filename, tokens[i].c_str());
                            directoryTable[targetDir].directoryEntry[k].firstblocknumber = firstBlockNumber;
                            directoryTable[targetDir].directoryEntry[k].size = 0;
                            time_t now = time(0);
                            std::string dateTimeString = std::ctime(&now);
                            std::string timeString = dateTimeString.substr(11, 8); // Saat, dakika ve saniye bilgilerini alır
                            std::strcpy(directoryTable[targetDir].directoryEntry[k].lastModfDate, timeString.c_str());
                            superBlock.freeBlockCount = superBlock.freeBlockCount - 1; 
                            superBlock.filesCount = superBlock.filesCount + 1;
                            updateSizeOfDir(path, fatTable, directoryTable, superBlock, blockWithoutDir, timeString);
                            break;
                        }
                        
                    }
                }
                isInvalidPath = 1;
            }
            else
            {
                int targetDir = firstBlockNumber - blockWithoutDir;
                for (int j = 0; j < directoryTable[targetDir].getEntryCount(); j++)
                {
                    if (strcmp(directoryTable[targetDir].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                    {
                        firstBlockNumber = directoryTable[targetDir].directoryEntry[j].firstblocknumber;
                        isInvalidPath = 1;
                        break;
                    }
                }
            }
            if(isInvalidPath == 0)
            {
                std::cout<<"Invalid Path !"<<std::endl;
                break;
            }
            isInvalidPath = 0;
        }
    }
}
void rmdirHelper(int targetBlockNumber,Fat12Table &fatTable, DirectoryTable *directoryTable, SuperBLock &superBlock , int blockWithoutDir)
{
    int endOfDir = 0;
    while (endOfDir != 1)
    {
        int targetDir = targetBlockNumber - blockWithoutDir;
        for (int k = 0; k < directoryTable[targetDir].getEntryCount(); k++)
        {
            if (directoryTable[targetDir].directoryEntry[k].filename[0] != '\0')
            {
                std::cout<<"filename: "<<directoryTable[targetDir].directoryEntry[k].filename<<" is deleted"<<std::endl;
                directoryTable[targetDir].directoryEntry[k].filename[0] = '\0';
                directoryTable[targetDir].directoryEntry[k].size = 0;
                directoryTable[targetDir].directoryEntry[k].lastModfDate[0] = '\0';
                targetBlockNumber = directoryTable[targetDir].directoryEntry[k].firstblocknumber;
                int firstBlockNumber = directoryTable[targetDir].directoryEntry[k].firstblocknumber;
                fatTable.fat12Entry[firstBlockNumber].nextBlockNumber = -2;
                superBlock.freeBlockCount = superBlock.freeBlockCount + 1;
                superBlock.filesCount = superBlock.filesCount - 1;
                rmdirHelper(targetBlockNumber, fatTable, directoryTable, superBlock, blockWithoutDir);
                //break;
            }
        }
        endOfDir = 1;
    }
}
void rmdirCommand(std::string path, Fat12Table &fatTable, DirectoryTable *directoryTable, SuperBLock &superBlock, int blockWithoutDir)
{
    std::vector<std::string> tokens;
    tokens = splitPath(path, '\\');
    int firstBlockNumber = 0;
    int targetBlockNumber = 0;
    for (int i = 0; i < tokens.size(); i++)
    {
        if (i == 0)
        {
            for (int j = 0; j < directoryTable[0].getEntryCount(); j++)
            {
                if (strcmp(directoryTable[0].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                {
                    firstBlockNumber = directoryTable[0].directoryEntry[j].firstblocknumber;
                    if (i == tokens.size() - 1)
                    {
                        std::cout<<"filename: "<<directoryTable[0].directoryEntry[j].filename<<" is deleted"<<std::endl;
                        directoryTable[0].directoryEntry[j].filename[0] = '\0';
                        directoryTable[0].directoryEntry[j].size = 0;
                        directoryTable[0].directoryEntry[j].lastModfDate[0] = '\0';
                        targetBlockNumber = directoryTable[0].directoryEntry[j].firstblocknumber;
                        fatTable.fat12Entry[firstBlockNumber].nextBlockNumber = -2;
                        superBlock.freeBlockCount = superBlock.freeBlockCount + 1; 
                        superBlock.filesCount = superBlock.filesCount - 1;
                    }
                    break;
                }
            }
        }
        else if (i == tokens.size() - 1)
        {
            int targetDir = firstBlockNumber - blockWithoutDir;
            int firstBlockNumber = 0;
            for (int k = 0; k < directoryTable[targetDir].getEntryCount(); k++)
            {
                if (std::strcmp(directoryTable[targetDir].directoryEntry[k].filename, tokens[i].c_str()) == 0)
                {
                    std::cout<<"filename: "<<directoryTable[targetDir].directoryEntry[k].filename<<" is deleted"<<std::endl;
                    directoryTable[targetDir].directoryEntry[k].filename[0] = '\0';
                    directoryTable[targetDir].directoryEntry[k].size = 0;
                    directoryTable[targetDir].directoryEntry[k].lastModfDate[0] = '\0';
                    targetBlockNumber = directoryTable[targetDir].directoryEntry[k].firstblocknumber;
                    firstBlockNumber = directoryTable[targetDir].directoryEntry[k].firstblocknumber;
                    fatTable.fat12Entry[firstBlockNumber].nextBlockNumber = -2;
                    superBlock.freeBlockCount = superBlock.freeBlockCount + 1;
                    superBlock.filesCount = superBlock.filesCount - 1;
                    break;
                }
            }
        }
        else
        {
            int targetDir = firstBlockNumber - blockWithoutDir;
            for (int j = 0; j < directoryTable[targetDir].getEntryCount(); j++)
            {
                if (strcmp(directoryTable[targetDir].directoryEntry[j].filename, tokens[i].c_str()) == 0)
                {
                    firstBlockNumber = directoryTable[targetDir].directoryEntry[j].firstblocknumber;
                    break;
                }
            }
        }
    }
    int endOfDir = 0;
    while (endOfDir != 1)
    {
        int targetDir = targetBlockNumber - blockWithoutDir;
        for (int k = 0; k < directoryTable[targetDir].getEntryCount(); k++)
        {
            if (directoryTable[targetDir].directoryEntry[k].filename[0] != '\0')
            {   
                std::cout<<"filename: "<<directoryTable[targetDir].directoryEntry[k].filename<<" is deleted"<<std::endl;
                directoryTable[targetDir].directoryEntry[k].filename[0] = '\0';
                directoryTable[targetDir].directoryEntry[k].size = 0;
                directoryTable[targetDir].directoryEntry[k].lastModfDate[0] = '\0';
                targetBlockNumber = directoryTable[targetDir].directoryEntry[k].firstblocknumber;
                firstBlockNumber = directoryTable[targetDir].directoryEntry[k].firstblocknumber;
                fatTable.fat12Entry[firstBlockNumber].nextBlockNumber = -2;
                superBlock.freeBlockCount = superBlock.freeBlockCount + 1;
                superBlock.filesCount = superBlock.filesCount - 1;
                rmdirHelper(targetBlockNumber, fatTable, directoryTable, superBlock,blockWithoutDir);
                //break;
            }
        }
        endOfDir = 1;
    }
}
int freeBlockCount(Fat12Table &fatTable)
{
    int count = 0;
    for (int i = 0; i < fatEntryCount; i++)
    {
        if (fatTable.fat12Entry[i].nextBlockNumber == -2)
        {
            count++;
        }
    }
    return count;
}


void occupiedBlocksWithName(Fat12Table &fatTable, DirectoryTable *directoryTable, int blockWithoutDir)
{   
    std::cout << "filename: " << "SuperBlock ," << " block number: " << 0 << std::endl;
    for(int i=1;i<blockWithoutDir;i++)
    {
        std::cout << "filename: " << "FatTable ," << " block number: " << i << std::endl;
    }
    std::cout << "filename: " << "Root Block ," << " block number: " << blockWithoutDir << std::endl;

    for (int i = blockWithoutDir; i < fatEntryCount; i++)
    {
        if (fatTable.fat12Entry[i].nextBlockNumber != -2)
        {
            int targetDir = i - blockWithoutDir;
            for (int j = 0; j < directoryTable[targetDir].getEntryCount(); j++)
            {
                if (directoryTable[targetDir].directoryEntry[j].filename[0] != '\0')
                {
                    std::cout << "filename: " << directoryTable[targetDir].directoryEntry[j].filename << " block number: " 
                                                    << directoryTable[targetDir].directoryEntry[j].firstblocknumber << std::endl;                             
                }
            }
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Invalid Argument" << std::endl;
        return 1;
    }

    // part 2
    if (strcmp(argv[1], "makeFileSystem") == 0)
    {
        int blockSize = std::stoi(argv[2]);
        blockSize = blockSize * 1024; // KB
        if (blockSize > maxBlockSize)
        {
            std::cerr << "Block size too large" << std::endl;
            return 1;
        }
        std::string diskName = argv[3];
        initializeData(blockSize);

        Fat12Table fat12Table(fatEntryCount);
        int fat12EntrySize = fatEntryCount * sizeof(Fat12Entry);
        int dataBlockCountForFat = fat12EntrySize / blockSize;

        
        SuperBLock superBlock;
        superBlock.blockSize = blockSize;
        superBlock.rootDirPosition = dataBlockCountForFat + 1;
        superBlock.blockCount = fatEntryCount;
        superBlock.freeBlockCount = freeBlockCount(fat12Table);
        superBlock.filesCount = 0;

        // Root directoryde kaç tane entry olcak
        int rootDirEntryCount = superBlock.blockSize / sizeof(DirectoryEntry);
        dirEntryCount = rootDirEntryCount;

        int dirTableSize = fatEntryCount - dataBlockCountForFat - 1;

        DirectoryTable *directoryTable = new DirectoryTable[dirTableSize];

        fat12Table.fat12Entry[0].nextBlockNumber = -1; // super block
        for (int i = 1; i <= dataBlockCountForFat; i++)
            fat12Table.fat12Entry[i].nextBlockNumber = -1;                    // fat table
        fat12Table.fat12Entry[dataBlockCountForFat + 1].nextBlockNumber = -1; // root directory

        std::ofstream file(diskName, std::ios::binary);
        if (!file)
        {
            std::cerr << "Dosya açılamadı" << std::endl;
            return 1;
        }
        file.write(reinterpret_cast<char *>(&superBlock), sizeof(SuperBLock));
        file.write(reinterpret_cast<char *>(fat12Table.fat12Entry), sizeof(Fat12Entry) * fatEntryCount);
        for (int i = 0; i < dirTableSize; i++)
        {
            file.write(reinterpret_cast<char *>(directoryTable[i].directoryEntry), sizeof(DirectoryEntry) * rootDirEntryCount);
        }

        file.close();
    }
    // part 3
    else if (strcmp(argv[1], "fileSystemOpen") == 0)
    {
        std::string diskName = argv[2];
        std::string operation = argv[3];
        std::ifstream file2(diskName, std::ios::binary);
        if (!file2)
        {
            std::cerr << "Dosya açılamadı" << std::endl;
            return 1;
        }
        Fat12Table fat12Table(fatEntryCount);
        SuperBLock superBlock;

        // Super block dosyadan data blocka okuma ve blockdan superblocka okuma
        char tempData[50];
        file2.read(tempData, sizeof(SuperBLock));
        SuperBLock *ptrSuperBlock = reinterpret_cast<SuperBLock *>(tempData);
        superBlock = *ptrSuperBlock;
        initializeData(superBlock.blockSize);
        memcpy(data[0], tempData, sizeof(SuperBLock));

        int fat12EntrySize = fatEntryCount * sizeof(Fat12Entry);
        int dataBlockCountForFat = fat12EntrySize / superBlock.blockSize;

        // Fat12Entry'leri dosyadan data blocka okuma
        for (int i = 1; i <= dataBlockCountForFat; i++)
        {
            file2.read(data[i], superBlock.blockSize);
        }

        // Fat12Entry'leri okuma, data block dan
        Fat12Entry *entries = new Fat12Entry[fatEntryCount];
        int count = 0;
        for (int i = 1; i <= dataBlockCountForFat; i++)
        {
            for (int j = 0; j < superBlock.blockSize / sizeof(Fat12Entry); j++)
            {
                memcpy(&entries[count], &(data[i][j * sizeof(Fat12Entry)]), sizeof(Fat12Entry));
                count++;
            }
        }
        fat12Table.fat12Entry = entries;

        // Root directory entry'leri dosyadan data blocka okuma
        int dirTableSize = fatEntryCount - dataBlockCountForFat - 1;
        for (int i = dataBlockCountForFat + 1; i < dirTableSize + 1; i++)
        {
            file2.read(data[i], superBlock.blockSize);
        }
        int rootDirEntryCount = superBlock.blockSize / sizeof(DirectoryEntry);
        dirEntryCount = rootDirEntryCount;
        DirectoryTable *directoryTable = new DirectoryTable[dirTableSize];

        // Root directory entry'leri okuma, data block dan
        DirectoryEntry *entries2 = new DirectoryEntry[rootDirEntryCount];
        for (int i = 0; i < dirTableSize; i++)
        {
            count = 0;
            for (int j = 0; j < superBlock.blockSize / sizeof(DirectoryEntry); j++)
            {
                memcpy(&entries2[count], &(data[dataBlockCountForFat + 1 + i][j * sizeof(DirectoryEntry)]), sizeof(DirectoryEntry));
                count++;
            }
            directoryTable[i].directoryEntry = entries2;
            entries2 = new DirectoryEntry[rootDirEntryCount];
        }

        file2.close();

        if (operation == "mkdir")
        {
            std::string parameters = argv[4];
            int blockWithoutDir = dataBlockCountForFat + 1;

            mkdirCommand(parameters, fat12Table, directoryTable, superBlock ,blockWithoutDir);

            std::ofstream file(diskName, std::ios::binary);
            if (!file)
            {
                std::cerr << "Dosya açılamadı" << std::endl;
                return 1;
            }
            file.write(reinterpret_cast<char *>(&superBlock), sizeof(SuperBLock));
            file.write(reinterpret_cast<char *>(fat12Table.fat12Entry), sizeof(Fat12Entry) * fatEntryCount);
            for (int i = 0; i < dirTableSize; i++)
            {
                file.write(reinterpret_cast<char *>(directoryTable[i].directoryEntry), sizeof(DirectoryEntry) * rootDirEntryCount);
            }

            file.close();
        }
        else if (operation == "dir")
        {
            std::string parameters = argv[4];
            int blockWithoutDir = dataBlockCountForFat + 1;
            dirCommand(parameters, fat12Table, directoryTable, blockWithoutDir);
        }

        else if (operation == "rmdir")
        {
            std::string parameters = argv[4];
            int blockWithoutDir = dataBlockCountForFat + 1;
            rmdirCommand(parameters, fat12Table, directoryTable, superBlock, blockWithoutDir);
            std::ofstream file(diskName, std::ios::binary);
            if (!file)
            {
                std::cerr << "Dosya açılamadı" << std::endl;
                return 1;
            }
            file.write(reinterpret_cast<char *>(&superBlock), sizeof(SuperBLock));
            file.write(reinterpret_cast<char *>(fat12Table.fat12Entry), sizeof(Fat12Entry) * fatEntryCount);
            for (int i = 0; i < dirTableSize; i++)
            {
                file.write(reinterpret_cast<char *>(directoryTable[i].directoryEntry), sizeof(DirectoryEntry) * rootDirEntryCount);
            }

            file.close();
        }
        else if (operation == "dumpe2fs")
        {
            int blockWithoutDir = dataBlockCountForFat + 1;
            std::cout<<"Block Size: "<<superBlock.blockSize<<std::endl;
            std::cout<<"Block Count: "<<superBlock.blockCount<<std::endl;
            std::cout<<"Free Block Count: "<<superBlock.freeBlockCount<<std::endl;
            std::cout<<"Files Count: "<<superBlock.filesCount<<std::endl;
            occupiedBlocksWithName(fat12Table, directoryTable,blockWithoutDir);

        }
    }

    return 0;
}
