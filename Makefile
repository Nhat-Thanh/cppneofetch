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
UTILS_HEADERS := $(wildcard ${UTILS_DIR}/*.hpp)

SRC_DIR := src
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

${OBJECT_PATH}/%.o: ${SRC_DIR}/%.cpp  ${INCLUDE_DIR}/%.hpp ${UTILS_HEADERS} | ${OBJECT_PATH}
	$(CXX) -c $< ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

${OBJECT_PATH}/main.o: main.cpp ${UTILS_HEADERS} | ${OBJECT_PATH}
	$(CXX) -c $< ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

${BIN_PATH}/${BINARY}: ${OBJECTS} ${OBJECT_PATH}/main.o | ${BIN_PATH}
	$(CXX) $^ ${COMPILE_SWITCHES} ${INCLUDE_FLAGS} ${CXXFLAGS} -o $@

install: build
	@mkdir -p $(PREFIX)/bin
	install ${BIN_PATH}/${BINARY} ${PREFIX}/bin/${BINARY}

build: ${BIN_PATH}/${BINARY}
	@strip $<

run:
	@./${BIN_PATH}/${BINARY}

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}