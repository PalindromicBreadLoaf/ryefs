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

#endif //RYEFS_FS_JOURNAL_H
