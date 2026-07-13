/*
 * Copyright(c) 2026: PalindromicBreadLoaf (palindromicbreadloaf@tuta.com)
 *
 * Please see the included LICENSE file for more information.
 */

#ifndef RYEFS_FS_JOURNAL_H
#define RYEFS_FS_JOURNAL_H
#include <stddef.h>
#include <stdint.h>

#include "fs_constants.h"

// Primary journal record. Each record is zero-padded to
// RYEFS_JOURNAL_ALIGN bytes. Record_length must be at least header_size,
// must equal align8(header_size + data_length), and must fit inside the
// journal region.
typedef struct journal_entry
{
    uint32_t magic;                 // JRNL_MAGIC
    uint16_t header_size;           // sizeof(journal_entry_t) in v1
    uint16_t flags;                 // Must be zero in v1
    uint32_t type;                  // Entry type (JTYPE_*)
    uint32_t record_length;         // Header + payload + zero padding, align8
    uint64_t transaction_id;        // Monotonically increasing id of transactions
    uint64_t sequence;              // Entry index within a transaction
    uint64_t transaction_entries;   // Valid on COMMIT: entries before commit
    uint64_t journal_epoch;         // Matches superblock journal epoch
    uint64_t target_block;          // Block address the entry applies to
    uint64_t timestamp;             // Entry write time in Unix nanoseconds
    uint32_t data_length;           // Bytes of following payload
    uint32_t checksum;              // CRC32C of complete padded record with this field zeroed
    uint8_t data[];                 // Block data or metadata payload
} __attribute__((packed)) journal_entry_t;

_Static_assert(sizeof(journal_entry_t) == 72,
               "journal entry header must be exactly 72 bytes");
_Static_assert(sizeof(journal_entry_t) % RYEFS_JOURNAL_ALIGN == 0,
               "journal entry header must be 8-byte aligned in length");
_Static_assert(offsetof(journal_entry_t, transaction_id) == 16,
               "journal entry 64-bit fields must start 8-byte aligned");

// Checkpoint journal record. Written only after every block
// of the corresponding primary transaction has been flushed to its final
// location. Stored in fixed RYEFS_CKPT_SLOT_SIZE-byte slots. The record at
// the start of the slot with the remainder zero filled.
typedef struct checkpoint_entry
{
    uint32_t magic;             // CKPT_MAGIC
    uint32_t _reserved0;        // Must be zero
    uint64_t transaction_id;    // Must match the committed primary entry
    uint64_t committed_at;      // Commit time in Unix nanoseconds
    uint64_t blocks_written;    // Number of blocks flushed in transaction
    uint32_t checksum;          // CRC32C of this record with this field zeroed
    uint32_t _reserved1;        // Must be zero
} __attribute__((packed)) checkpoint_entry_t;

_Static_assert(sizeof(checkpoint_entry_t) == 40,
               "checkpoint entry must be exactly 40 bytes");
_Static_assert(sizeof(checkpoint_entry_t) <= RYEFS_CKPT_SLOT_SIZE,
               "checkpoint entry must fit in one checkpoint slot");

#endif //RYEFS_FS_JOURNAL_H
