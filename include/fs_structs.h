//
// Created by breadloaf on 30/04/2026.
//

#ifndef RYEFS_FS_STRUCTS_H
#define RYEFS_FS_STRUCTS_H
#include <stdint.h>

#include "fs_constants.h"

typedef struct superblock
{
    uint32_t magic;                 // FS_MAGIC
    uint16_t version_major;         // For breaking changes
    uint16_t version_minor;         // For non-breaking changes
    uint32_t block_size;            // 512, 1024, 2048, 2049, 4096, 8192, 16384, 32768, or 65526 bytes
    uint64_t block_count;           // Total blocks on volume
    uint64_t free_blocks;           // Number of unallocated blocks
    uint64_t inode_count;           // Total inodes provisioned at creation time
    uint64_t free_inodes;           // Number of unallocated inodes
    uint64_t journal1_start;        // Initial block address of primary journal
    uint32_t journal1_blocks;       // Length of primary journal
    uint64_t journal2_start;        // Initial block address of journal 2
    uint32_t journal2_blocks;       // Length of secondary journal
    uint64_t bitmap_start;          // Block address of allocation bitmap start
    uint64_t inode_table_start;     // Block address of inode table start
    uint64_t data_start;            // Block address of first data block
    uint64_t root_inode;            // Inode number of the root directory
    uint64_t dedup_index_start;     // Block address of the dedup hash index (0 = none)
    uint32_t dedup_index_blocks;    // Length of dedup index
    uint32_t flags;                 // Enabled feature flags
    uint32_t mount_count;           // Number of times volume has been mounted
    uint32_t max_mount_count;       // Force run fsck after this many mounts
    uint64_t last_mount_time;       // In Unix epoch nanoseconds
    uint64_t last_write_time;       // ^^
    uint64_t last_check_time;       // ^^
    uint8_t uuid[16];               // Volume UUID
    uint8_t volume_label[32];       // UTF-8 with null-padding
    uint32_t checksum;              // CRC32C of bytes 0..(sizeof-4)
    uint8_t _pad[pad_to_block];     // Zeropad this struct to exactly one block
} __attribute__((packed)) superblock_t;

typedef struct inode
{
    uint16_t mode;                          // File type and permissions (Unix-standard)
    uint16_t uid;                           // Owners user ID
    uint16_t gid;                           // Owners group ID
    uint16_t _pad0;                         // Padding
    uint32_t link_count;                    // Number of hard links
    uint32_t flags;                         // Inode flags
    uint64_t size;                          // File size in bytes
    uint64_t block_count;                   // 512-byte blocks allocated
    uint64_t atime;                         // Last access time in Unix nanoseconds
    uint64_t mtime;                         // Last modified time in Unix nanoseconds
    uint64_t ctime;                         // Last status-change time in Unix nanoseconds
    uint64_t crtime;                        // Creation time in Unix nanoseconds
    uint64_t direct[INODE_DIRECT_BLOCKS];   // Direct pointers to blocks
    uint64_t indirect1;                     // Pointer to a single indirect block
    uint64_t indirect2;                     // Pointer to a double indirect block
    uint64_t indirect3;                     // Pointer to a triple indirect block
    uint64_t xattr_block;                   // Block containing extended attributes (0 = none)
    uint8_t file_hash[32];                  // BLAKE3 hash of full file contents
    uint64_t dedup_cononical;               // Inode that owns the blocks (0 = self)
    uint32_t dedup_ref_count;               // 0 = unshared, >1 = block chain is shared
    uint32_t generation;                    // Increment on inode reuse
    uint32_t checksum;                      // CRC32C of inode structure
    uint8_t _reserved[remaining_bytes];     // Zero-pad to INODE_SIZE
} __attribute__((packed)) inode_t;

#endif //RYEFS_FS_STRUCTS_H
