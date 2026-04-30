//
// Created by breadloaf on 30/04/2026.
//

#ifndef RYEFS_FS_JOURNAL_H
#define RYEFS_FS_JOURNAL_H
#include <stdint.h>

typedef struct journal_entry
{
    uint32_t magic;             // JOURNAL_MAGIC constant
    uint32_t type;              // Entry type
    uint64_t transaction_id;    // Monotonically increasing id of transactions
    uint64_t sequence;          // Entry index within a transaction
    uint64_t target_block;      // Block address the entry applies to
    uint64_t timestamp;         // Last accessed time in Unix nanoseconds
    uint32_t data_length;       // Bytes of following payload
    uint32_t checksum;          // CRC32C of header + payload
    uint8_t data[];             // Block data or metadata payload
} __attribute__((__packed__)) journal_entry;

typedef struct checkpoint_entry
{
    uint32_t magic;             // CHECKPOINT_MAGIC
    uint64_t transaction_id;    // Must match committed primary entry
    uint64_t committed_at;      // Time commit in Unix time
    uint64_t blocks_written;    // Number of blocks flushed in transaction
    uint32_t checksum;          // CRC32C of this transaction
} __attribute__((__packed__)) checkpoint_entry;

#endif //RYEFS_FS_JOURNAL_H
