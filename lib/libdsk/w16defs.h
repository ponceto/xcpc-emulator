/* Windows 95 disk I/O functions - based on Stan Mitchell's DISKDUMP.C */

#define   LEVEL0_LOCK   0
#define   LEVEL1_LOCK   1
#define   LEVEL2_LOCK   2
#define   LEVEL3_LOCK   3
#define   LEVEL1_LOCK_MAX_PERMISSION      0x0001

#define   DRIVE_IS_REMOTE                 0x1000
#define   DRIVE_IS_SUBST                  0x8000

#pragma pack(1)
typedef struct MID {
    WORD  midInfoLevel;       // information level, must be 0
    DWORD midSerialNum;       // serial number for the medium
    char  midVolLabel[11];    // volume label for the medium
    char  midFileSysType[8];  // type of file system as 8-byte ASCII
    }
    MID, *PMID;

typedef struct driveparams {    /* Disk geometry */
    BYTE special;
    BYTE devicetype;
    WORD deviceattrs;
    WORD cylinders;
    BYTE mediatype;
    /* BPB starts here */
    WORD bytespersector;
    BYTE sectorspercluster;
    WORD reservedsectors;
    BYTE numberofFATs;
    WORD rootdirsize;
    WORD totalsectors;
    BYTE mediaid;
    WORD sectorsperfat;
    WORD sectorspertrack;
    WORD heads;
    DWORD hiddensectors;
    DWORD bigtotalsectors;
    BYTE  reserved[6];
    /* BPB ends here */
    WORD sectorcount;
    WORD sectortable[80];
    } DRIVEPARAMS, *PDRIVEPARAMS;
/*}}}*/
               
#pragma pack()
