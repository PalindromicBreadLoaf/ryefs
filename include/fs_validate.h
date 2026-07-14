/*
 * Copyright(c) 2026: PalindromicBreadLoaf (palindromicbreadloaf@tuta.com)
 *
 * Please see the included LICENSE file for more information.
 */

/*
 * Shared format validators.
 *
 * These all should be called prior to doing any work on the drive, as they verify the safety of any writes.
 *
 * A structurally valid image can still be refused by policy (e.g. a dedup volume opened read-write by a driver that
 * does not implement dedup), however, we should do our best to notify the user why mount was refused and provide
 * a clean solution to mount if they wish.
 */

#ifndef RYEFS_FS_VALIDATE_H
#define RYEFS_FS_VALIDATE_H

#include <stdint.h>

#include "fs_structs.h"

typedef enum ryefs_valid
{
    RYEFS_VALID_OK = 0,         /* Passed every check                             */
    RYEFS_ERR_NULL_ARG,         /* A required pointer argument was NULL           */
    RYEFS_ERR_BLOCK_SIZE,       /* block_size is not a supported size             */
    RYEFS_ERR_ZERO_REGION,      /* A mandatory region has zero length             */
    RYEFS_ERR_REGION_BOUNDS,    /* A region runs past the end of the volume       */
    RYEFS_ERR_REGION_OVERLAP,   /* Two regions claim the same block(s)            */
    RYEFS_ERR_DEDUP_REGION,     /* SB_FLAG_DEDUP and the dedup region disagree    */
    RYEFS_ERR_ROOT_INODE,       /* root_inode is 0 or beyond inode_count          */
    RYEFS_ERR_UNKNOWN_FEATURE,  /* A feature flag this build does not know is set */
    RYEFS_ERR_DEDUP_UNAWARE_RW, /* Dedup volume, RW mount, no dedup support       */
    RYEFS_ERR_VOLUME_SUSPECT,   /* SB_FLAG_VOLUME_SUSPECT set, RW refused         */
    RYEFS_ERR_DIRTY_READONLY,   /* SB_FLAG_DIRTY set, v1 RO mount refused         */
} ryefs_valid_t;

/*
 * The set of superblock feature-flag bits this build understands. Any bit set
 * in sb->flags but NOT in this mask was written by a newer/other RyeFS and is
 * treated as an unknown feature (see ryefs_validate_features).
 *
 * Bit 5 and everything at bit 9 and above are reserved for potential future features.
 */
#define RYEFS_SB_FLAGS_KNOWN                                           \
    (SB_FLAG_JOURNAL_ENABLED | SB_FLAG_CHECKPOINT_JOURNAL |            \
     SB_FLAG_DIRTY | SB_FLAG_ERRORS_READONLY | SB_FLAG_UNIX_PERMS |    \
     SB_FLAG_SPARSE | SB_FLAG_DEDUP | SB_FLAG_VOLUME_SUSPECT)

/* Describes the mount a caller wants to perform. */
typedef struct ryefs_mount_req
{
    int writable;        /* Nonzero: read-write. Zero: read-only. */
    int supports_dedup;  /* Nonzero: this driver implements dedup/CoW.  */
} ryefs_mount_req_t;

const char *ryefs_valid_strerror(ryefs_valid_t code);

/*
 * Returns 1 if block_size is supported.
 */
int ryefs_block_size_supported(uint32_t block_size);

ryefs_valid_t ryefs_validate_layout(const superblock_t *sb);

ryefs_valid_t ryefs_validate_features(uint32_t flags, const ryefs_mount_req_t *req);

/*
 * Runs all above verification and returns final say on whether a mount is allowed.
 */
ryefs_valid_t ryefs_validate_mount(const superblock_t *sb, const ryefs_mount_req_t *req);

#endif /* RYEFS_FS_VALIDATE_H */
