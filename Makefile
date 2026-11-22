PREFIX ?= /usr
MODE ?= release

CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -MMD -MP

SCRIPT_PATH := scripts
BUILD_PATH := build
GENERATED_PATH := $(BUILD_PATH)/generated
EXTERNAL_PATH := $(BUILD_PATH)/external

OBJECT_PATH := $(BUILD_PATH)/objects
BIN_PATH := $(BUILD_PATH)/bin

BINARY := cppneofetch
TEST_BINARY := cppneofetch_test

INCLUDE_PATH := include
INCLUDE_FLAGS := -I$(INCLUDE_PATH) -I$(GENERATED_PATH)
PCI_IDS_HEADER := $(GENERATED_PATH)/PciIds.hpp

SRC_PATH := src
HELPERS_SRC_PATH := $(SRC_PATH)/helpers
SOURCES := $(wildcard $(SRC_PATH)/*.cpp) \
           $(wildcard $(HELPERS_SRC_PATH)/*.cpp)
OBJECTS := $(patsubst $(SRC_PATH)/%.cpp,$(OBJECT_PATH)/%.o,$(SOURCES))

TEST_PATH := test
HELPERS_TEST_PATH := $(TEST_PATH)/helpers
TEST_SOURCES := $(wildcard $(TEST_PATH)/*.cpp) \
                $(wildcard $(HELPERS_TEST_PATH)/*.cpp)
TEST_OBJECTS := $(patsubst $(TEST_PATH)/%.cpp,$(OBJECT_PATH)/$(TEST_PATH)/%.o,$(TEST_SOURCES))

DEPS := $(OBJECTS:.o=.d) $(OBJECT_PATH)/main.d
TEST_DEPS := $(OBJECTS:.o=.d) $(TEST_OBJECTS:.o=.d)

ifeq ($(MODE), debug)
	CXXFLAGS += -g -O0
else ifeq ($(MODE), release)
	CXXFLAGS += -O2
endif

.PHONY: all build install run test clean

all: build

$(GENERATED_PATH):
	@mkdir -p $@

$(OBJECT_PATH):
	@mkdir -p $@

$(BIN_PATH):
	@mkdir -p $@

$(EXTERNAL_PATH):
	@mkdir -p $@

# Generate header file for pci devices information
$(PCI_IDS_HEADER): $(SCRIPT_PATH)/gen-pci-ids-header.py | $(GENERATED_PATH) $(EXTERNAL_PATH)
	@if [ ! -f $(EXTERNAL_PATH)/pci.ids ]; then \
		echo "[INFO] Download pci.ids file"; \
		curl -fSL --progress-bar -o $(EXTERNAL_PATH)/pci.ids https://pci-ids.ucw.cz/v2.2/pci.ids; \
	fi
	@printf '[INFO] Generate %s\n' $@
	@python $(SCRIPT_PATH)/gen-pci-ids-header.py $(EXTERNAL_PATH)/pci.ids $@

# Build main binary
$(OBJECT_PATH)/%.o: $(SRC_PATH)/%.cpp | $(OBJECT_PATH)
	@mkdir -p $(dir $@)
	@printf '[INFO] Build object of %s\n' $<
	@$(CXX) -c $< $(INCLUDE_FLAGS) $(CXXFLAGS) -o $@

$(OBJECT_PATH)/CGpuInfo.o: $(SRC_PATH)/CGpuInfo.cpp $(PCI_IDS_HEADER) | $(OBJECT_PATH)
	@mkdir -p $(dir $@)
	@printf '[INFO] Build object of %s\n' $<
	@$(CXX) -c $< $(INCLUDE_FLAGS) $(CXXFLAGS) -o $@

$(OBJECT_PATH)/main.o: main.cpp | $(OBJECT_PATH)
	@printf '[INFO] Build object %s\n' $<
	@$(CXX) -c $< $(INCLUDE_FLAGS) $(CXXFLAGS) -o $@

$(BIN_PATH)/$(BINARY): $(OBJECTS) $(OBJECT_PATH)/main.o | $(BIN_PATH)
	@printf '[INFO] Link %s\n' $@
	@$(CXX) $^ $(CXXFLAGS) -o $@
	@printf '[INFO] Build successfully\n'


# Build test binary
$(OBJECT_PATH)/$(TEST_PATH)/%.o: $(TEST_PATH)/%.cpp | $(OBJECT_PATH)
	@mkdir -p $(dir $@)
	@printf '[INFO] Build test object of %s\n' $<
	@$(CXX) -c $< $(INCLUDE_FLAGS) -I$(TEST_PATH) $(CXXFLAGS) -o $@

$(OBJECT_PATH)/$(TEST_PATH)/TestCGpuInfo.o: $(TEST_PATH)/TestCGpuInfo.cpp $(PCI_IDS_HEADER) $(OBJECT_PATH)/CGpuInfo.o  | $(OBJECT_PATH)
	@mkdir -p $(dir $@)
	@printf '[INFO] Build test object of %s\n' $<
	@$(CXX) -c $< $(INCLUDE_FLAGS) -I$(TEST_PATH) $(CXXFLAGS) -o $@

$(BIN_PATH)/$(TEST_BINARY): $(OBJECTS) $(TEST_OBJECTS) | $(BIN_PATH)
	@printf '[INFO] Link %s\n' $@
	@$(CXX) $^ $(CXXFLAGS) -o $@
	@printf '[INFO] Build test binaries successfully\n'

# Rules
build: $(BIN_PATH)/$(BINARY)
ifeq ($(MODE), release)
	@strip $(BIN_PATH)/$(BINARY)
endif

install: build
	@mkdir -p $(PREFIX)/bin
	@printf '[INFO] Install binary to %s\n' $(PREFIX)/bin/$(BINARY)
	@install -m 755 $(BIN_PATH)/$(BINARY) $(PREFIX)/bin/$(BINARY)

run: build
	@./$(BIN_PATH)/$(BINARY)

test: $(BIN_PATH)/$(TEST_BINARY)
	@./$(BIN_PATH)/$(TEST_BINARY)

clean:
	rm -rf $(BUILD_PATH)

-include $(DEPS)
-include $(TEST_DEPS)
