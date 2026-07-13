/*
 * Copyright(c) 2026: PalindromicBreadLoaf (palindromicbreadloaf@tuta.com)
 *
 * Please see the included LICENSE file for more information.
 */

#ifndef RYEFS_FS_STRUCTS_H
#define RYEFS_FS_STRUCTS_H
#include <stddef.h>
#include <stdint.h>

#include "fs_constants.h"

// Fixed 512-byte on-disk record. For block sizes larger than 512 bytes it is
// written at the start of its block and the remainder of the block is
// zero-filled by the writer.
typedef struct superblock
{
    uint32_t magic;                 // FS_MAGIC
    uint16_t version_major;         // For breaking changes
    uint16_t version_minor;         // For non-breaking changes
    uint32_t block_size;            // One supported v1 block size (512..131072, power of two)
    uint32_t flags;                 // Enabled feature flags
    uint32_t mount_count;           // Number of times volume has been mounted
    uint32_t max_mount_count;       // Force run fsck after this many mounts
    uint64_t block_count;           // Total blocks on volume
    uint64_t free_blocks;           // Number of unallocated blocks
    uint64_t inode_count;           // Total inodes provisioned at creation time
    uint64_t free_inodes;           // Number of unallocated inodes
    uint64_t journal1_start;        // Block address: primary journal start
    uint64_t journal1_blocks;       // Length of primary journal in blocks
    uint64_t journal2_start;        // Block address: checkpoint journal start
    uint64_t journal2_blocks;       // Length of checkpoint journal in blocks
    uint64_t bitmap_start;          // Block address: allocation bitmap start
    uint64_t bitmap_blocks;         // Length of allocation bitmap in blocks
    uint64_t inode_bitmap_start;    // Block address: inode allocation bitmap
    uint64_t inode_bitmap_blocks;   // Length of inode bitmap in blocks
    uint64_t inode_table_start;     // Block address: inode table start
    uint64_t inode_table_blocks;    // Length of inode table in blocks
    uint64_t dedup_index_start;     // Block address: dedup hash index (0 = none)
    uint64_t dedup_index_blocks;    // Length of dedup index in blocks
    uint64_t data_start;            // Block address: first data block
    uint64_t data_blocks;           // Length of data region in blocks
    uint64_t root_inode;            // Inode number of the root directory
    uint64_t next_transaction_id;   // Next journal transaction id to allocate
    uint64_t journal1_head;         // Byte offset within primary journal region of next write
    uint64_t journal1_tail;         // Byte offset within primary journal region of oldest live entry
    uint64_t journal1_epoch;        // Incremented when primary journal wraps
    uint64_t journal2_head;         // Byte offset within checkpoint journal region of next write
    uint64_t journal2_tail;         // Byte offset within checkpoint journal region of oldest entry
    uint64_t journal2_epoch;        // Incremented when checkpoint journal wraps
    uint64_t last_mount_time;       // In Unix epoch nanoseconds
    uint64_t last_write_time;       // ^^
    uint64_t last_check_time;       // ^^
    uint8_t uuid[16];               // Volume UUID
    uint8_t volume_label[32];       // UTF-8 with null-padding
    uint64_t superblock_generation; // Incremented on every superblock update
    uint32_t checksum;              // CRC32C of this 512-byte record with this field zeroed
    uint8_t _reserved[196];         // Must be zero
} __attribute__((packed)) superblock_t;

_Static_assert(sizeof(superblock_t) == RYEFS_SUPERBLOCK_SIZE,
               "superblock must be exactly 512 bytes");
_Static_assert(offsetof(superblock_t, block_count) == 24,
               "superblock 64-bit fields must start 8-byte aligned");
_Static_assert(offsetof(superblock_t, root_inode) == 168,
               "superblock root_inode offset is frozen");
_Static_assert(offsetof(superblock_t, superblock_generation) == 304,
               "superblock generation offset is frozen");
_Static_assert(offsetof(superblock_t, checksum) == 312,
               "superblock checksum offset is frozen");

typedef struct inode
{
    uint16_t mode;                          // File type and permissions (Unix-standard)
    uint16_t _pad0;                         // Must be zero
    uint32_t uid;                           // Owner's user ID
    uint32_t gid;                           // Owner's group ID
    uint32_t link_count;                    // Number of hard links
    uint32_t flags;                         // Inode flags
    uint32_t _pad1;                         // Align following 64-bit fields, must be zero
    uint64_t size;                          // File size in bytes
    uint64_t block_count;                   // 512-byte blocks allocated (for stat())
    uint64_t rdev;                          // Device number for char/block devices (major
                                            // in high 32 bits, minor in low 32; else zero)
    uint64_t atime;                         // Last access time in Unix nanoseconds
    uint64_t mtime;                         // Last modified time in Unix nanoseconds
    uint64_t ctime;                         // Last status-change time in Unix nanoseconds
    uint64_t crtime;                        // Creation time in Unix nanoseconds
    uint64_t direct[INODE_DIRECT_BLOCKS];   // Direct pointers to blocks
    uint64_t indirect1;                     // Pointer to a single indirect block
    uint64_t indirect2;                     // Pointer to a double indirect block
    uint64_t indirect3;                     // Pointer to a triple indirect block
    uint8_t file_hash[32];                  // BLAKE3 hash of full file contents
    uint64_t dedup_canonical;               // Inode that owns the blocks (0 = self)
    uint32_t dedup_ref_count;               // Canonical-only shared refcount; >= 2 while
                                            // shared, 0 on non-canonical/unshared inodes
    uint32_t generation;                    // Increment on inode reuse
    uint32_t checksum;                      // CRC32C of this 512-byte record with this field zeroed
    uint8_t _reserved[260];                 // Must be zero
} __attribute__((packed)) inode_t;

_Static_assert(sizeof(inode_t) == INODE_SIZE,
               "inode must be exactly 512 bytes");
_Static_assert(offsetof(inode_t, size) == 24,
               "inode 64-bit fields must start 8-byte aligned");
_Static_assert(offsetof(inode_t, direct) == 80,
               "inode direct pointer offset is frozen");
_Static_assert(offsetof(inode_t, checksum) == 248,
               "inode checksum offset is frozen");

typedef struct dirent
{
    uint64_t inode;     // Inode number (0 = deleted/unused slot)
    uint32_t rec_len;   // Bytes to the next entry; 32-bit so one free record
                        // can span an entire 128 KB directory block
    uint8_t name_len;   // Length of name in bytes (max 255)
    uint8_t file_type;  // Redundant type hint, but it speeds up readdir
    uint16_t _reserved; // Must be zero
    char name[];        // UTF-8, NOT null-terminated
    // Implicit zero padding to align the next entry to 8 bytes
} __attribute__((packed)) dirent_t;

_Static_assert(sizeof(dirent_t) == RYEFS_DIRENT_MIN_REC_LEN,
               "dirent header must be exactly 16 bytes");

typedef struct dedup_header
{
    uint32_t magic;         // DEDUP_MAGIC
    uint32_t _reserved0;    // Must be zero
    uint64_t capacity;      // Total number of slots in the table
    uint64_t used;          // Number of occupied slots
    uint64_t tombstones;    // Deleted slots that still extend probe chains
    uint32_t checksum;      // CRC32C of header with this field zeroed
    uint32_t _reserved1;    // Must be zero
} __attribute__((packed)) dedup_header_t;

_Static_assert(sizeof(dedup_header_t) == 40,
               "dedup header must be exactly 40 bytes");

typedef struct dedup_entry
{
    uint8_t hash[32];   // BLAKE3 hash of file contents
    uint64_t inode;     // Canonical inode, DEDUP_SLOT_EMPTY, or DEDUP_SLOT_TOMBSTONE
    uint32_t checksum;  // CRC32C of this entry with this field zeroed
    uint32_t _reserved; // Must be zero
} __attribute__((packed)) dedup_entry_t;

_Static_assert(sizeof(dedup_entry_t) == 48,
               "dedup entry must be exactly 48 bytes");

#endif //RYEFS_FS_STRUCTS_H
