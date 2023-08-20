# File-System-Design
* It is homework for the Operating Systems course.
## General Info
* 6 different structures are designed for the file system. Directory Entry, Directory Table, Fat12 Entry, Fat12 Table, Super Block and Data Block, This file system is designed in accordance with the FAT12 structure.
* Data blocks include Super BLock, Fat Table, Root Directory and Files and directories, respectively. Super Block and Root Directory are designed to be represented in a data block. According to the block size value, the number of blocks occupied by Fat Table varies. Since the Fat12 structure is used, there are 2^12 data blocks
* The FAT structure uses the MS-DOS directory entry structure.
* Directory table, on the other hand, is a place where the directory entries are kept collectively, the directory table is designed as an array, and each index corresponds to the relevant data block.
* Fat Table is a structure that shows the status of the blocks in the file system.
* Super Block contains all important system related information like block size, root directory location, number of free blocks. This information is acted upon.
* In Data Block, data is stored.
* Design-related visuals, test cases and more detailed information are available in the report.
## Setup 
```
$ cd ../File-System-Design/src
$ make
$ ./hw3 makeFileSystem 4 mySystem.dat
$ ./hw3 fileSystemOpen mySystem.dat mkdir "\okan"
```
