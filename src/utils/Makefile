# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES
#****************************************************************************
CC     := $(CROSS_COMPILE)gcc
CXX    := $(CROSS_COMPILE)g++
LD     := $(CROSS_COMPILE)ld
AR     := $(CROSS_COMPILE)ar
RANLIB := $(CROSS_COMPILE)ranlib

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS} 
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  :=

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************
DEFS :=

#****************************************************************************
# Include paths
#****************************************************************************
INCS :=-I inc

#****************************************************************************
# Include paths
#****************************************************************************
BUILD_DIR := ../../build


#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************
OUTPUT := ${BUILD_DIR}/libutils.a 

all: ${OUTPUT}


#****************************************************************************
# Source files
#****************************************************************************
SRCS := CException.cpp CHdfFileAdapter.cpp CHdfManager.cpp CInputSelect.cpp CPosixFile.cpp NDebugUtils.cpp neo_err.c neo_files.c neo_hash.c neo_hdf.c neo_misc.c neo_rand.c neo_str.c ulist.c ulocks.c wildmat.c

OBJS := $(addsuffix .o,$(basename ${SRCS}))
OBJS_ABS := $(addprefix ${BUILD_DIR}/,$(OBJS))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS_ABS}
	ar -cvq $@ ${OBJS_ABS}

# Rules for compiling source files to object files
${BUILD_DIR}/%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

${BUILD_DIR}/%.o : %.c
	${CC} -c ${CXXFLAGS} ${INCS} $< -o $@

dist:
	bash makedistlinux

clean:
	-rm -f core ${OBJS_ABS} ${OUTPUT}

depend:
	makedepend -Y -p${BUILD_DIR}/ --${CXXFLAGS} --${SRCS} ${INCS}

# DO NOT DELETE

../../build/CHdfFileAdapter.o: inc/CHdfFileAdapter.hpp inc/IHdfStorage.hpp
../../build/CHdfFileAdapter.o: inc/CPosixFile.hpp inc/IFile.hpp inc/types.h
../../build/CHdfFileAdapter.o: inc/CException.hpp inc/NDebugUtils.hpp
../../build/CHdfManager.o: inc/CHdfManager.hpp inc/types.h inc/neo_hdf.h
../../build/CHdfManager.o: inc/neo_err.h inc/neo_misc.h inc/cs_config.h
../../build/CHdfManager.o: inc/neo_str.h inc/ulist.h inc/neo_hash.h
../../build/CHdfManager.o: inc/IHdfStorage.hpp inc/CException.hpp
../../build/CInputSelect.o: inc/CInputSelect.hpp inc/types.h
../../build/CInputSelect.o: inc/CException.hpp
../../build/CPosixFile.o: inc/CPosixFile.hpp inc/IFile.hpp inc/types.h
../../build/CPosixFile.o: inc/CException.hpp inc/CInputSelect.hpp
../../build/NDebugUtils.o: inc/NDebugUtils.hpp inc/types.h
../../build/neo_err.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/neo_err.o: inc/neo_str.h inc/ulist.h inc/ulocks.h
../../build/neo_files.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/neo_files.o: inc/neo_str.h inc/ulist.h inc/neo_files.h
../../build/neo_files.o: inc/wildmat.h
../../build/neo_hash.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/neo_hash.o: inc/neo_str.h inc/ulist.h inc/neo_hash.h
../../build/neo_hdf.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/neo_hdf.o: inc/neo_str.h inc/ulist.h inc/neo_rand.h inc/neo_hdf.h
../../build/neo_hdf.o: inc/neo_hash.h inc/neo_files.h
../../build/neo_misc.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/neo_misc.o: inc/neo_str.h inc/ulist.h
../../build/neo_rand.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/neo_rand.o: inc/neo_str.h inc/ulist.h inc/neo_rand.h
../../build/neo_str.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/neo_str.o: inc/neo_str.h inc/ulist.h
../../build/ulist.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/ulist.o: inc/neo_str.h inc/ulist.h
../../build/ulocks.o: inc/cs_config.h inc/neo_misc.h inc/neo_err.h
../../build/ulocks.o: inc/neo_str.h inc/ulist.h inc/neo_files.h inc/ulocks.h
../../build/wildmat.o: inc/cs_config.h inc/neo_misc.h
