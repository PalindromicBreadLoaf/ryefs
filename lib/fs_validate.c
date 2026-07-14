/*
 * Copyright(c) 2026: PalindromicBreadLoaf (palindromicbreadloaf@tuta.com)
 *
 * Please see the included LICENSE file for more information.
 */

#include "fs_validate.h"

#include <stddef.h>

#include "fs_constants.h"

int ryefs_block_size_supported(const uint32_t block_size)
{
    return RYEFS_BLOCK_SIZE_VALID(block_size) ? 1 : 0;
}

typedef struct region
{
    uint64_t start;
    uint64_t len;
} region_t;

static int regions_overlap(const region_t *a, const region_t *b)
{
    if (a->len == 0 || b->len == 0)
        return 0;
    return a->start < b->start + b->len && b->start < a->start + a->len;
}

static int region_in_bounds(const region_t *r, const uint64_t block_count)
{
    if (r->len == 0)
        return 1;
    if (r->start >= block_count)
        return 0;
    if (r->len > block_count - r->start)
        return 0;
    return 1;
}

ryefs_valid_t ryefs_validate_layout(const superblock_t *sb)
{
    if (sb == NULL)
        return RYEFS_ERR_NULL_ARG;

    if (!ryefs_block_size_supported(sb->block_size))
        return RYEFS_ERR_BLOCK_SIZE;

    const uint64_t total = sb->block_count;

    const int dedup_flag = (sb->flags & SB_FLAG_DEDUP) != 0;
    const int dedup_present =
        sb->dedup_index_start != 0 || sb->dedup_index_blocks != 0;
    if (dedup_flag && sb->dedup_index_blocks == 0)
        return RYEFS_ERR_DEDUP_REGION;
    if (!dedup_flag && dedup_present)
        return RYEFS_ERR_DEDUP_REGION;

    region_t regions[10];
    size_t n = 0;
    regions[n++] = (region_t){ 0, 1 }; /* boot record            */
    regions[n++] = (region_t){ 1, 1 }; /* primary superblock     */
    regions[n++] = (region_t){ 2, 1 }; /* backup superblock      */
    regions[n++] = (region_t){ sb->journal1_start,     sb->journal1_blocks };
    regions[n++] = (region_t){ sb->journal2_start,     sb->journal2_blocks };
    regions[n++] = (region_t){ sb->bitmap_start,       sb->bitmap_blocks };
    regions[n++] = (region_t){ sb->inode_bitmap_start, sb->inode_bitmap_blocks };
    regions[n++] = (region_t){ sb->inode_table_start,  sb->inode_table_blocks };
    regions[n++] = (region_t){ sb->data_start,         sb->data_blocks };
    if (dedup_flag)
        regions[n++] = (region_t){ sb->dedup_index_start,
                                   sb->dedup_index_blocks };

    /* Every mandatory region must have a non-zero length. */
    for (size_t i = 3; i <= 8; i++)
    {
        if (regions[i].len == 0)
            return RYEFS_ERR_ZERO_REGION;
    }

    /* Each region must fit inside the volume. */
    for (size_t i = 0; i < n; i++)
    {
        if (!region_in_bounds(&regions[i], total))
            return RYEFS_ERR_REGION_BOUNDS;
    }

    /* No two regions may share a block. */
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = i + 1; j < n; j++)
        {
            if (regions_overlap(&regions[i], &regions[j]))
                return RYEFS_ERR_REGION_OVERLAP;
        }
    }

    /* Root inode must exist. */
    if (sb->root_inode == 0 || sb->root_inode > sb->inode_count)
        return RYEFS_ERR_ROOT_INODE;

    return RYEFS_VALID_OK;
}

ryefs_valid_t ryefs_validate_features(const uint32_t flags, const ryefs_mount_req_t *req)
{
    if (req == NULL)
        return RYEFS_ERR_NULL_ARG;

    if ((flags & ~(uint32_t)RYEFS_SB_FLAGS_KNOWN) != 0)
        return RYEFS_ERR_UNKNOWN_FEATURE;

    if (flags & SB_FLAG_DEDUP && req->writable && !req->supports_dedup)
        return RYEFS_ERR_DEDUP_UNAWARE_RW;

    return RYEFS_VALID_OK;
}

ryefs_valid_t ryefs_validate_mount(const superblock_t *sb, const ryefs_mount_req_t *req)
{
    if (sb == NULL || req == NULL)
        return RYEFS_ERR_NULL_ARG;

    ryefs_valid_t rc = ryefs_validate_layout(sb);
    if (rc != RYEFS_VALID_OK)
        return rc;

    rc = ryefs_validate_features(sb->flags, req);
    if (rc != RYEFS_VALID_OK)
        return rc;

    if (sb->flags & SB_FLAG_VOLUME_SUSPECT && req->writable)
        return RYEFS_ERR_VOLUME_SUSPECT;

    /* A dirty volume was not cleanly unmounted and needs journal replay.
     * This cannot be executed properly for now.
     * In the future this won't stop mount.
     */
    if (sb->flags & SB_FLAG_DIRTY && !req->writable)
        return RYEFS_ERR_DIRTY_READONLY;

    return RYEFS_VALID_OK;
}

const char *ryefs_valid_strerror(const ryefs_valid_t code)
{
    switch (code)
    {
    case RYEFS_VALID_OK:            return "ok";
    case RYEFS_ERR_NULL_ARG:        return "null argument";
    case RYEFS_ERR_BLOCK_SIZE:      return "unsupported block size";
    case RYEFS_ERR_ZERO_REGION:     return "mandatory region has zero length";
    case RYEFS_ERR_REGION_BOUNDS:   return "region extends past end of volume";
    case RYEFS_ERR_REGION_OVERLAP:  return "regions overlap";
    case RYEFS_ERR_DEDUP_REGION:    return "dedup flag and dedup region disagree";
    case RYEFS_ERR_ROOT_INODE:      return "root inode out of range";
    case RYEFS_ERR_UNKNOWN_FEATURE: return "unknown feature flag set";
    case RYEFS_ERR_DEDUP_UNAWARE_RW:
        return "read-write mount of dedup volume without dedup support";
    case RYEFS_ERR_VOLUME_SUSPECT:
        return "volume marked suspect.";
    case RYEFS_ERR_DIRTY_READONLY:
        return "volume is dirty.";
    }
    return "unknown error";
}
