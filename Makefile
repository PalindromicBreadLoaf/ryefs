# ==============================================================================
# RyeFS
# ==============================================================================

# --- Toolchain ----------------------------------------------------------------
CC      := clang
AR      := ar
ARFLAGS := rcs

# --- Build type ---------------------------------------------------------------
# BUILD=debug (default) or BUILD=release.
BUILD ?= debug

# --- Directories --------------------------------------------------------------
SRC_LIB   := lib
SRC_MKFS  := mkfs
SRC_FSCK  := fsck
SRC_FUSE  := fuse
SRC_TOOLS := tools
SRC_TESTS := tests/unit

INC_DIR    := include
BUILD_ROOT := build
BUILD_DIR  := $(BUILD_ROOT)/$(BUILD)
BIN_DIR    := bin

# --- Outputs -----------------------------------------------------------------
LIB_NAME    := libryefs.a
LIB_OUT     := $(BUILD_DIR)/$(LIB_NAME)

BIN_MKFS    := $(BIN_DIR)/mkfs.ryefs
BIN_FSCK    := $(BIN_DIR)/fsck.ryefs
BIN_FUSE    := $(BIN_DIR)/ryefs
BIN_INFO    := $(BIN_DIR)/rye-info
BIN_DUMP    := $(BIN_DIR)/rye-dump
BIN_DEDUP   := $(BIN_DIR)/rye-dedup-stat
BIN_TESTS   := $(BIN_DIR)/rye-tests

# --- Compiler flags -----------------------------------------------------------
CFLAGS_COMMON := \
    -std=c11 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Wstrict-prototypes \
    -Wmissing-prototypes \
    -Wcast-align \
    -I$(INC_DIR)

# Debug build (default)
CFLAGS_DEBUG := \
    $(CFLAGS_COMMON) \
    -g3 \
    -O0 \
    -DDEBUG \
    -fsanitize=address,undefined

# Release build
CFLAGS_RELEASE := \
    $(CFLAGS_COMMON) \
    -O2 \
    -flto \
    -DNDEBUG

ifeq ($(BUILD),release)
    CFLAGS := $(CFLAGS_RELEASE)
    LDFLAGS_EXTRA := -flto
else
    CFLAGS := $(CFLAGS_DEBUG)
    LDFLAGS_EXTRA := -fsanitize=address,undefined
endif

# --- FUSE flags ---------------------------------------------------------------
FUSE_CFLAGS  := $(shell pkg-config --cflags fuse3 2>/dev/null)
FUSE_LDFLAGS := $(shell pkg-config --libs   fuse3 2>/dev/null)

# --- BLAKE3 -------------------------------------------------------------------
# Vendored from https://github.com/BLAKE3-team/BLAKE3 (c/, tag 1.8.5).
# See `lib/blake3-LICENSE_A2` and `lib/blake3-LICENSE_CC0` for licensing.
BLAKE3_SRCS := \
    $(SRC_LIB)/blake3.c \
    $(SRC_LIB)/blake3_dispatch.c \
    $(SRC_LIB)/blake3_portable.c

BLAKE3_DEFS := \
    -DBLAKE3_NO_SSE2 \
    -DBLAKE3_NO_SSE41 \
    -DBLAKE3_NO_AVX2 \
    -DBLAKE3_NO_AVX512 \
    -DBLAKE3_USE_NEON=0

# --- Source files -------------------------------------------------------------
LIB_SRCS := \
    $(SRC_LIB)/fs_structs.c \
    $(SRC_LIB)/fs_endian.c \
    $(SRC_LIB)/fs_crc.c \
    $(SRC_LIB)/fs_boot.c \
    $(SRC_LIB)/fs_geometry.c \
    $(SRC_LIB)/fs_validate.c \
    $(SRC_LIB)/fs_io.c \
    $(SRC_LIB)/fs_superblock.c \
    $(SRC_LIB)/fs_bitmap.c \
    $(SRC_LIB)/fs_inode.c \
    $(SRC_LIB)/fs_block.c \
    $(SRC_LIB)/fs_dir.c \
    $(SRC_LIB)/fs_journal.c \
    $(SRC_LIB)/fs_dedup.c \
    $(SRC_LIB)/fs_volume.c \
    $(BLAKE3_SRCS)

MKFS_SRCS := \
    $(SRC_MKFS)/main.c \
    $(SRC_MKFS)/mkfs.c

FSCK_SRCS := \
    $(SRC_FSCK)/main.c \
    $(SRC_FSCK)/fsck_phase0.c \
    $(SRC_FSCK)/fsck_phase1.c \
    $(SRC_FSCK)/fsck_phase2.c \
    $(SRC_FSCK)/fsck_phase3.c \
    $(SRC_FSCK)/fsck_phase3b.c \
    $(SRC_FSCK)/fsck_phase4.c \
    $(SRC_FSCK)/fsck_phase5.c

FUSE_SRCS := \
    $(SRC_FUSE)/main.c \
    $(SRC_FUSE)/fuse_ops.c \
    $(SRC_FUSE)/fuse_read.c \
    $(SRC_FUSE)/fuse_write.c

TOOL_INFO_SRCS  := $(SRC_TOOLS)/rye-info.c
TOOL_DUMP_SRCS  := $(SRC_TOOLS)/rye-dump.c
TOOL_DEDUP_SRCS := $(SRC_TOOLS)/rye-dedup-stat.c

# Unit tests link into one rye-tests binary.
# Tests should not redefine a main().
TEST_SRCS := \
    $(SRC_TESTS)/test_main.c \
    $(SRC_TESTS)/test_constants.c \
    $(SRC_TESTS)/test_endian.c \
    $(SRC_TESTS)/test_crc.c \
    $(SRC_TESTS)/test_structs.c \
    $(SRC_TESTS)/test_boot.c \
    $(SRC_TESTS)/test_geometry.c \
    $(SRC_TESTS)/test_io.c \
    $(SRC_TESTS)/test_superblock.c \
    $(SRC_TESTS)/test_bitmap.c \
    $(SRC_TESTS)/test_inode.c \
    $(SRC_TESTS)/test_dirent.c \
    $(SRC_TESTS)/test_block.c \
    $(SRC_TESTS)/test_journal.c \
    $(SRC_TESTS)/test_dedup.c

# --- Object files -----------------------------------------------------------
LIB_OBJS      := $(patsubst %.c,$(BUILD_DIR)/%.o,$(LIB_SRCS))
MKFS_OBJS     := $(patsubst %.c,$(BUILD_DIR)/%.o,$(MKFS_SRCS))
FSCK_OBJS     := $(patsubst %.c,$(BUILD_DIR)/%.o,$(FSCK_SRCS))
FUSE_OBJS     := $(patsubst %.c,$(BUILD_DIR)/%.o,$(FUSE_SRCS))
INFO_OBJS     := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TOOL_INFO_SRCS))
DUMP_OBJS     := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TOOL_DUMP_SRCS))
DEDUP_OBJS    := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TOOL_DEDUP_SRCS))
TEST_OBJS     := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TEST_SRCS))
BLAKE3_OBJS   := $(patsubst %.c,$(BUILD_DIR)/%.o,$(BLAKE3_SRCS))

# Vendored BLAKE3 objects get the portable-only defines
$(BLAKE3_OBJS): CFLAGS += $(BLAKE3_DEFS)

# ==============================================================================
# Targets
# ==============================================================================

.PHONY: all lib mkfs fsck fuse tools tests check integration clean distclean help

# Default: build everything except the FUSE driver
all: lib mkfs fsck tools tests

# Build the core static library
lib: $(LIB_OUT)

# Individual binaries
mkfs:  $(BIN_MKFS)
fsck:  $(BIN_FSCK)
fuse:  $(BIN_FUSE)
tools: $(BIN_INFO) $(BIN_DUMP) $(BIN_DEDUP)
tests: $(BIN_TESTS)

# Run unit tests
check: tests
	@echo "==> Running unit tests"
	$(BIN_TESTS)

# Run integration tests (requires FUSE and root access)
integration: fuse mkfs fsck
	@echo "==> Running integration tests"
	@bash tests/integration/test_mkfs_fsck.sh
	@bash tests/integration/test_basic_ro.sh
	@bash tests/integration/test_basic_rw.sh
	@bash tests/integration/test_large_file.sh
	@bash tests/integration/test_permissions.sh
	@bash tests/integration/test_crash_recovery.sh
	@bash tests/integration/test_fault_injection.sh
	@bash tests/integration/test_dedup.sh
	@bash tests/integration/test_fill.sh

# ==============================================================================
# Build rules
# ==============================================================================

# Static library
$(LIB_OUT): $(LIB_OBJS) | $(BUILD_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Binaries
$(BIN_MKFS): $(MKFS_OBJS) $(LIB_OUT) | $(BIN_DIR)
	$(CC) $(LDFLAGS_EXTRA) -o $@ $^

$(BIN_FSCK): $(FSCK_OBJS) $(LIB_OUT) | $(BIN_DIR)
	$(CC) $(LDFLAGS_EXTRA) -o $@ $^

$(BIN_FUSE): $(FUSE_OBJS) $(LIB_OUT) | $(BIN_DIR)
	$(CC) $(LDFLAGS_EXTRA) $(FUSE_LDFLAGS) -o $@ $^

$(BIN_INFO): $(INFO_OBJS) $(LIB_OUT) | $(BIN_DIR)
	$(CC) $(LDFLAGS_EXTRA) -o $@ $^

$(BIN_DUMP): $(DUMP_OBJS) $(LIB_OUT) | $(BIN_DIR)
	$(CC) $(LDFLAGS_EXTRA) -o $@ $^

$(BIN_DEDUP): $(DEDUP_OBJS) $(LIB_OUT) | $(BIN_DIR)
	$(CC) $(LDFLAGS_EXTRA) -o $@ $^

$(BIN_TESTS): $(TEST_OBJS) $(LIB_OUT) | $(BIN_DIR)
	$(CC) $(LDFLAGS_EXTRA) -o $@ $^

# --- Compile rules with dependency tracking -----------------------------------
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD_DIR)/$*.d

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

# FUSE sources additionally need FUSE_CFLAGS
$(BUILD_DIR)/$(SRC_FUSE)/%.o: $(SRC_FUSE)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(FUSE_CFLAGS) -MT $@ -MMD -MP -MF $(BUILD_DIR)/$(SRC_FUSE)/$*.d -c -o $@ $<

ALL_DEPS := $(patsubst %.c,$(BUILD_DIR)/%.d,\
            $(LIB_SRCS) $(MKFS_SRCS) $(FSCK_SRCS) $(FUSE_SRCS) \
            $(TOOL_INFO_SRCS) $(TOOL_DUMP_SRCS) $(TOOL_DEDUP_SRCS) $(TEST_SRCS))

-include $(ALL_DEPS)

# ==============================================================================
# Directory creation
# ==============================================================================

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# ==============================================================================
# Utility targets
# ==============================================================================

# Create a blank test image and mount it (Linux only)
.PHONY: testimg mount umount
TEST_IMG  := /tmp/ryefs_test.img
TEST_MNT  := /tmp/ryefs_mnt
IMG_SIZE  := 64M

testimg: mkfs
	dd if=/dev/zero of=$(TEST_IMG) bs=1M count=64 status=none
	$(BIN_MKFS) -b 4096 -L "RyeFS Test" $(TEST_IMG)
	@echo "Image created: $(TEST_IMG)"
	@echo "Run 'make mount' to mount it."

mount: fuse
	mkdir -p $(TEST_MNT)
	$(BIN_FUSE) $(TEST_IMG) $(TEST_MNT) -f -d &
	@echo "Mounted at $(TEST_MNT) — run 'make umount' to unmount."

umount:
	fusermount3 -u $(TEST_MNT) 2>/dev/null || fusermount -u $(TEST_MNT)

# Dump superblock info from a test image
.PHONY: info
info: tools testimg
	$(BIN_INFO) $(TEST_IMG)

# Clean all build artifacts
clean:
	rm -rf $(BUILD_ROOT) $(BIN_DIR)

# Deep clean including test images
distclean: clean
	rm -f $(TEST_IMG)
	rm -rf $(TEST_MNT)

# ==============================================================================
# Release build shortcut
# ==============================================================================

.PHONY: release
release:
	$(MAKE) BUILD=release all fuse

# ==============================================================================
# Help
# ==============================================================================

help:
	@echo ""
	@echo "RyeFS build system"
	@echo ""
	@echo "  make              Build lib, mkfs, fsck, tools, and unit tests (debug)"
	@echo "  make release      Build everything optimised"
	@echo "  make fuse         Build the FUSE driver (requires libfuse3)"
	@echo "  make check        Run unit tests"
	@echo "  make integration  Run integration tests (Linux only, requires FUSE)"
	@echo "  make testimg      Create a blank 64M test image"
	@echo "  make mount        Mount the test image at $(TEST_MNT)"
	@echo "  make umount       Unmount the test image"
	@echo "  make info         Dump superblock info from the test image"
	@echo "  make clean        Remove build/ and bin/"
	@echo "  make distclean    Remove build/, bin/, and test images"
	@echo ""
	@echo "  BUILD=release     Set to 'release' for an optimised build"
	@echo "                    e.g.: make BUILD=release"
	@echo ""
