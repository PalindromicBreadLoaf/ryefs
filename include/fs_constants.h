//
// Created by breadloaf on 30/04/2026.
//

#ifndef RYEFS_FS_CONSTANTS_H
#define RYEFS_FS_CONSTANTS_H

// Magic
#define FS_MAGIC 0x52796500
#define JRNL_MAGIC 0x4A524E42

// Current Major/Minor version numbers
#define RYEFS_MAJOR_VERSION 1
#define RYEFS_MINOR_VERSION 0

// FS Flags
#define SB_FLAG_JOURNAL_ENABLED     (1 << 0)    // Has active primary journal
#define SB_FLAG_CHECKPOINT_JOURNAL  (1 << 1)    // Has active checkpoint journal
#define SB_FLAG_DIRTY               (1 << 2)    // Set on mount, clear on proper unmount
#define SB_FLAG_ERRORS_READONLY     (1 << 3)    // Remount as RO on error
#define SB_FLAG_UNIX_PERMS          (1 << 4)    // Unix permissions are enabled
#define SB_FLAG_XATTRS              (1 << 5)    // Extended attributes are present
#define SB_FLAG_SPARSE              (1 << 6)    // Sparse file support enabled
#define SB_FLAG_DEDUP               (1 << 7)    // File-level dedup is enabled

// Journal entry types
#define JTYPE_BLOCK_DATA    1;  // Raw block 
#define JTYPE_INODE_UPDATE  2;  // Serialised inode update
#define JTYPE_BITMAP_UPDATE 3;  // Allocation bitmap delta
#define JTYPE_COMMIT        4;  // Transaction complete
#define JTYPE_ABORT         5;  // Transaction was rolled back
#define JTYPE_SUPERBLOCK    6;  // Superblock snapshot

#endif //RYEFS_FS_CONSTANTS_H
