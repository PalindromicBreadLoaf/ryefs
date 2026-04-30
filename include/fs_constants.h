//
// Created by breadloaf on 30/04/2026.
//

#ifndef RYEFS_FS_CONSTANTS_H
#define RYEFS_FS_CONSTANTS_H

#define FS_MAGIC 0x5279656673

#define RYEFS_MAJOR_VERSION 1
#define RYEFS_MINOR_VERSION 0

#define SB_FLAG_JOURNAL_ENABLED     (1 << 0)    // Has active primary journal
#define SB_FLAG_CHECKPOINT_JOURNAL  (1 << 1)    // Has active checkpoint journal
#define SB_FLAG_DIRTY               (1 << 2)    // Set on mount, clear on proper unmount
#define SB_FLAG_ERRORS_READONLY     (1 << 3)    // Remount as RO on error
#define SB_FLAG_UNIX_PERMS          (1 << 4)    // Unix permissions are enabled
#define SB_FLAG_XATTRS              (1 << 5)    // Extended attributes are present
#define SB_FLAG_SPARSE              (1 << 6)    // Sparse file support enabled
#define SB_FLAG_DEDUP               (1 << 7)    // File-level dedup is enabled

#endif //RYEFS_FS_CONSTANTS_H
