PREFIX ?= /usr
MODE ?= release

CXX := g++
CXXFLAGS := -Wall -Wextra
COMPILE_SWITCHES := -DSYSTEM_INSTALL

BUILD_DIR := build
OBJECT_PATH := ${BUILD_DIR}/objects
BIN_PATH := ${BUILD_DIR}/bin
BINARY := cppneofetch

UTILS_DIR := Utils
INCLUDE_DIR := include
INCLUDE_FLAGS := -I${UTILS_DIR} -I${INCLUDE_DIR}
UTILS_GENERATED_HEADERS := PciIds
UTILS_HEADERS := $(patsubst %,${UTILS_DIR}/%.hpp,${UTILS_GENERATED_HEADERS}) $(wildcard ${UTILS_DIR}/*.hpp) 

SRC_DIR := src
SOURCES := $(wildcard ${SRC_DIR}/*.cpp)
OBJECTS := $(patsubst ${SRC_DIR}/%.cpp,${OBJECT_PATH}/%.o,${SOURCES})

ifeq (${MODE}, debug)
	CXXFLAGS += -g
else ifeq (${MODE}, release)
	CXXFLAGS += -O2
endif

all: build

${UTILS_DIR}/PciIds.hpp:
	@bash -c 'if [ ! -f pci.ids ]; then curl -o pci.ids https://pci-ids.ucw.cz/v2.2/pci.ids; fi'
	@printf '[INFO] Generate %s\n' $@
	@python gen-pci-ids-header.py pci.ids

${OBJECT_PATH}:
	@mkdir -p $@

${BIN_PATH}:
	@mkdir -p $@

${OBJECT_PATH}/%.o: ${SRC_DIR}/%.cpp  ${INCLUDE_DIR}/%.hpp ${UTILS_HEADERS} | ${OBJECT_PATH}
	@printf '[INFO] Build object of %s\n' $<
	@$(CXX) -c $< ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

${OBJECT_PATH}/main.o: main.cpp ${UTILS_HEADERS} | ${OBJECT_PATH}
	@printf '[INFO] Build object %s\n' $<
	@$(CXX) -c $< ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

${BIN_PATH}/${BINARY}: ${OBJECTS} ${OBJECT_PATH}/main.o | ${BIN_PATH}
	@printf '[INFO] Link objects\n'
	@$(CXX) $^ ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

install: build
	@mkdir -p $(PREFIX)/bin
	@printf "[INFO] Install binary to %s\n" ${PREFIX}/bin/${BINARY}
	@install ${BIN_PATH}/${BINARY} ${PREFIX}/bin/${BINARY}

build: ${BIN_PATH}/${BINARY}
	@strip $<
	@printf '[INFO] Build successfully\n'

run:
	@./${BIN_PATH}/${BINARY}

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}
	rm -f ${UTILS_DIR}/PciIds.hpp