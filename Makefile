PACKAGE_VER_MAJOR = 0
PACKAGE_VER_MINOR = 0
PACKAGE_VER_PATCH = 0
PACKAGE_RELEASE = 0

LIBRARY = lib/libmp7reader.so
LIBRARY_SOURCES = $(wildcard src/*.cpp)
LIBRARY_OBJECT_FILES = $(patsubst src/%.cpp,obj/%.o,${LIBRARY_SOURCES})

EXECUTABLE_SOURCES = $(wildcard src/*.cxx)
EXECUTABLE_OBJECT_FILES = $(patsubst src/%.cxx,obj/%.o,${EXECUTABLE_SOURCES})
EXECUTABLES = $(patsubst src/%.cxx,bin/%.exe,${EXECUTABLE_SOURCES})

$(info ${EXECUTABLE_SOURCES})
$(info ${EXECUTABLE_OBJECT_FILES})
$(info ${EXECUTABLES})
$(info ${LIBRARY})



LIBRARY_PATH = \
			-Llib \
			-L${CACTUS_ROOT}/lib

LIBRARIES = 	\
			-lboost_system \
			-lboost_filesystem \
			-lboost_regex \
			-lboost_thread 

EXECUTABLE_LIBRARIES = ${LIBRARIES} -lmp7reader
$(info ${EXECUTABLE_LIBRARIES})

INCLUDE_PATH = 	\
		-Iinclude/ \
		-I${CACTUS_ROOT}/include

CPP_FLAGS = -g -Wall -O3 -MMD -MP -fPIC 

LINK_LIBRARY_FLAGS = -g -shared -fPIC -Wall -O3 ${LIBRARY_PATH} ${LIBRARIES}

LINK_EXECUTABLE_FLAGS = -g -Wall -O3 ${LIBRARY_PATH} ${EXECUTABLE_LIBRARIES}


.PHONY: all _all clean _cleanall build _buildall install _installall rpm _rpmall test _testall spec_update

default: build

clean: _cleanall
_cleanall:
	rm -rf ${RPMBUILD_DIR}
	rm -rf obj
	rm -rf bin
	rm -rf lib

all: _all
build: _all
buildall: _all
_all: ${LIBRARY} ${EXECUTABLES}

${LIBRARY}: ${LIBRARY_OBJECT_FILES}
	g++ ${LINK_LIBRARY_FLAGS} ${LIBRARY_OBJECT_FILES} -o $@

${LIBRARY_OBJECT_FILES}: obj/%.o : src/%.cpp
	mkdir -p {lib,obj}
	g++ ${INCLUDE_PATH} ${CPP_FLAGS} -c $< -o $@

-include $(LIBRARY_OBJECT_FILES:.o=.d)
	
${EXECUTABLES}: bin/%.exe: obj/%.o
	g++ ${LINK_EXECUTABLE_FLAGS} $< -o $@

${EXECUTABLE_OBJECT_FILES}: obj/%.o : src/%.cxx 
	mkdir -p {bin,obj,lib}
	g++ -c ${CPP_FLAGS} ${INCLUDE_PATH} $< -o $@

-include $(EXECUTABLE_OBJECT_FILES:.o=.d)
