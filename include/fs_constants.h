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

// Inode sizing
#define INODE_DIRECT_BLOCKS 12
#define INODE_SIZE          256     // Size in bytes

// Inode Flags
#define INODE_FLAG_IMMUTABLE        (1 << 0)    // File is immutable
#define INODE_FLAG_APPEND_ONLY      (1 << 1)    // Only appending writes are permitted
#define INODE_FLAG_NOATIME          (1 << 2)    // Do not update atime on access
#define INODE_FLAG_SPARSE           (1 << 3)    // File is sparse
#define INODE_FLAG_NOJOUR           (1 << 4)    // File is not journalled
#define INODE_FLAG_HAS_XATTRS       (1 << 5)    // File has a valid xattrs block
#define INODE_FLAG_HASH_VALID       (1 << 6)    // File hash reflects current file contents
#define INODE_FLAG_DEDUP_SHARE      (1 << 7)    // Block chain is shared via deduplication

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

// File Types
#define FT_UNKNOWN 0
#define FT_REG     1    // Regular file
#define FT_DIR     2    // Directory
#define FT_CHRDEV  3    // Character device
#define FT_BLKDEV  4    // Block device
#define FT_FIFO    5    // FIFO
#define FT_SOCK    6    // Socket
#define FT_SYMLINK 7    // Symbolic link

#endif //RYEFS_FS_CONSTANTS_H
