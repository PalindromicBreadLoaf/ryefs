/*
 * Copyright(c) 2026: PalindromicBreadLoaf (palindromicbreadloaf@tuta.com)
 *
 * Please see the included LICENSE file for more information.
 */

#ifndef RYEFS_FS_CONSTANTS_H
#define RYEFS_FS_CONSTANTS_H

// Magic values. All are stored little-endian on disk, so the byte sequence
// in a hex dump reads as the ASCII string in the comment.
#define FS_MAGIC    0x53465952u     // bytes "RYFS"
#define JRNL_MAGIC  0x4C4E524Au     // bytes "JRNL"
#define CKPT_MAGIC  0x54504B43u     // bytes "CKPT"
#define DEDUP_MAGIC 0x58444944u     // bytes "DIDX"

// Current Major/Minor version numbers
#define RYEFS_VERSION_MAJOR 1
#define RYEFS_VERSION_MINOR 0

// Fixed on-disk record sizes
#define RYEFS_SUPERBLOCK_SIZE 512   // Serialized superblock record size in bytes
#define INODE_SIZE            512   // Size in bytes
#define RYEFS_CKPT_SLOT_SIZE  64    // Checkpoint journal slot size in bytes

// Inode sizing
#define INODE_DIRECT_BLOCKS 12

// Supported v1 block sizes
// These are powers of two from 512 to 131072 inclusive
#define RYEFS_MIN_BLOCK_SIZE 512u
#define RYEFS_MAX_BLOCK_SIZE 131072u
#define RYEFS_BLOCK_SIZE_VALID(bs)                                  \
    ((bs) >= RYEFS_MIN_BLOCK_SIZE && (bs) <= RYEFS_MAX_BLOCK_SIZE && \
     (((bs) & ((bs) - 1u)) == 0u))

// Ssector 0 layout.
// These fields are read/written with explicit little-endian load/store helpers at byte offsets.
#define RYEFS_BOOT_OEM_NAME         "RYEFS1.0"  // 8 bytes
#define RYEFS_BOOT_OEM_OFFSET       3           // 8-byte OEM name
#define RYEFS_BOOT_MAGIC_OFFSET     11          // le32 FS_MAGIC ("RYFS")
#define RYEFS_BOOT_BLOCKSIZE_OFFSET 15          // le32 block_size
#define RYEFS_BOOT_VMAJOR_OFFSET    19          // le16 version_major
#define RYEFS_BOOT_VMINOR_OFFSET    21          // le16 version_minor
#define RYEFS_BOOT_SIG_OFFSET       510         // le16 boot signature
#define RYEFS_BOOT_SIG              0xAA55u     // bytes 0x55 0xAA on disk

// Inode Flags
#define INODE_FLAG_IMMUTABLE        (1 << 0)    // File is immutable
#define INODE_FLAG_APPEND_ONLY      (1 << 1)    // Only appending writes are permitted
#define INODE_FLAG_NOATIME          (1 << 2)    // Do not update atime on access
#define INODE_FLAG_SPARSE           (1 << 3)    // File is sparse
#define INODE_FLAG_RESERVED_4       (1 << 4)    // Must be zero in v1
#define INODE_FLAG_RESERVED_5       (1 << 5)    // Must be zero in v1
#define INODE_FLAG_HASH_VALID       (1 << 6)    // File hash reflects current file contents
#define INODE_FLAG_DEDUP_SHARE      (1 << 7)    // Block chain is shared via deduplication

// FS Flags
#define SB_FLAG_JOURNAL_ENABLED     (1 << 0)    // Has active primary journal
#define SB_FLAG_CHECKPOINT_JOURNAL  (1 << 1)    // Has active checkpoint journal
#define SB_FLAG_DIRTY               (1 << 2)    // Set on mount, clear on proper unmount
#define SB_FLAG_ERRORS_READONLY     (1 << 3)    // Remount as RO on error
#define SB_FLAG_UNIX_PERMS          (1 << 4)    // Unix permissions are enabled
#define SB_FLAG_RESERVED_5          (1 << 5)    // Bit 5 reserved, must be zero in v1
#define SB_FLAG_SPARSE              (1 << 6)    // Sparse file support enabled
#define SB_FLAG_DEDUP               (1 << 7)    // File-level dedup is enabled
#define SB_FLAG_VOLUME_SUSPECT      (1 << 8)    // Needs fsck before read-write use

// Journal entry types
#define JTYPE_BLOCK_DATA    1   // Raw block
#define JTYPE_INODE_UPDATE  2   // Serialised inode update
#define JTYPE_BITMAP_UPDATE 3   // Allocation bitmap delta
#define JTYPE_COMMIT        4   // Transaction complete
#define JTYPE_ABORT         5   // Transaction was rolled back
#define JTYPE_SUPERBLOCK    6   // Superblock snapshot
#define JTYPE_WRAP          7   // Padding record before circular wrap
#define JTYPE_DEDUP_UPDATE  8   // Dedup index entry change

// Journal records are zero-padded to this alignment
#define RYEFS_JOURNAL_ALIGN 8

// Inode mode: file type in bits 15-12, POSIX-compatible values so mode can
// be passed directly to stat.st_mode on Unix-like systems
#define RYEFS_S_IFMT   0xF000
#define RYEFS_S_IFIFO  0x1000   // FIFO
#define RYEFS_S_IFCHR  0x2000   // Character device
#define RYEFS_S_IFDIR  0x4000   // Directory
#define RYEFS_S_IFBLK  0x6000   // Block device
#define RYEFS_S_IFREG  0x8000   // Regular file
#define RYEFS_S_IFLNK  0xA000   // Symbolic link
#define RYEFS_S_IFSOCK 0xC000   // Socket

// Dirent file_type hints (mirror the lower 4 bits of the inode mode type)
#define FT_UNKNOWN 0
#define FT_REG     1    // Regular file
#define FT_DIR     2    // Directory
#define FT_CHRDEV  3    // Character device
#define FT_BLKDEV  4    // Block device
#define FT_FIFO    5    // FIFO
#define FT_SOCK    6    // Socket
#define FT_SYMLINK 7    // Symbolic link

// Directory entry limits
#define RYEFS_NAME_MAX           255    // Maximum name length in bytes
#define RYEFS_DIRENT_MIN_REC_LEN 16     // Minimum rec_len, incl. unused slots

// Dedup hash index slot states (value of dedup_entry.inode)
#define DEDUP_SLOT_EMPTY     0ull
#define DEDUP_SLOT_TOMBSTONE 0xFFFFFFFFFFFFFFFFull

#endif //RYEFS_FS_CONSTANTS_H
