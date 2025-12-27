PREFIX ?= /usr
MODE ?= release
CXX := g++
CXXFLAGS := -Wall -Wextra
COMPILE_SWITCHES := -DSYSTEM_INSTALL
BINARY := cppneofetch
INCLUDE_DIRS := Utils include
SRC_DIR := src
BUILD_DIR := build
OBJECT_PATH := ${BUILD_DIR}/objects
BIN_PATH := ${BUILD_DIR}/bin

INCLUDE_FLAGS := $(patsubst %,-I%,${INCLUDE_DIRS})
SRC := $(wildcard src/*.cpp)

SOURCES := $(wildcard ${SRC_DIR}/*.cpp)
OBJECTS := $(patsubst ${SRC_DIR}/%.cpp,${OBJECT_PATH}/%.o,${SOURCES})

ifeq (${MODE}, debug)
	CXXFLAGS += -g
else ifeq (${MODE}, release)
	CXXFLAGS += -O2
endif

all: build

${OBJECT_PATH}:
	mkdir -p $@

${BIN_PATH}:
	mkdir -p $@

${OBJECT_PATH}/%.o: ${SRC_DIR}/%.cpp | ${OBJECT_PATH}
	$(CXX) -c $< ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

${OBJECT_PATH}/main.o: main.cpp | ${OBJECT_PATH}
	$(CXX) -c $< ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

${BIN_PATH}/${BINARY}: ${OBJECTS} ${OBJECT_PATH}/main.o | ${BIN_PATH}
	$(CXX) $^ ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

install: build
	mkdir -p $(PREFIX)/bin
	install ${BIN_PATH}/${BINARY} ${PREFIX}/bin/${BINARY}

build: ${BIN_PATH}/${BINARY}
	strip $<

run:
	./${BIN_PATH}/${BINARY}

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}