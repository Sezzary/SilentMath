# Variables

CMAKE       = cmake
BUILD_DIR   = $(PWD)/Build
DEBUG_DIR   = $(BUILD_DIR)/Debug
RELEASE_DIR = $(BUILD_DIR)/Release

# Targets

.PHONY: debug-config release-config debug-build release-build

debug-config:
	$(CMAKE) -S $(PWD) -B $(DEBUG_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Debug

release-config:
	$(CMAKE) -S $(PWD) -B $(RELEASE_DIR) -DCMAKE_BUILD_TYPE=Release

debug-build:
	$(CMAKE) --build $(DEBUG_DIR)

release-build:
	$(CMAKE) --build $(RELEASE_DIR)
