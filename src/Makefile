# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES
#****************************************************************************
CC     := $(CROSS_COMPILE)gcc
CXX    := $(CROSS_COMPILE)g++
LD     := $(CROSS_COMPILE)ld
AR     := $(CROSS_COMPILE)ar
RANLIB := $(CROSS_COMPILE)ranlib

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG -Wcpp -DBOOST_SIGNALS_NO_DEPRECATION_WARNING
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3

LIBS		     := -lstdc++ -lwt -lwthttp -lboost_signals -lwtfcgi -lutils
LIB_PATH	     := -L ../build

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
INCS :=-I inc -I utils/inc

#****************************************************************************
# Include paths
#****************************************************************************
BUILD_DIR := ../build


#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************
OUTPUT := ${BUILD_DIR}/addypin.wt 

all: ${OUTPUT}


#****************************************************************************
# Source files
#****************************************************************************
SRCS := main.cpp AddyPin.cpp AddyPinAllocator.cpp AddyPinApp.cpp AddyUserInfo.cpp AddyDB.cpp

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))
OBJS_ABS := $(addprefix ${BUILD_DIR}/,$(OBJS))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS_ABS}
	${CXX} ${LDFLAGS} ${OBJS_ABS} ${LIB_PATH} ${LIBS} ${EXTRA_LIBS} -o $@

# Rules for compiling source files to object files
${BUILD_DIR}/%.o : %.cpp
	${CXX} -lwtfcgi -c ${CXXFLAGS} ${INCS} $< -o ${BUILD_DIR}/$@

dist:
	bash makedistlinux

clean:
	-rm -f core ${OBJS_ABS} ${BUILD_DIR}/${OUTPUT}

depend:
	makedepend -Y -p${BUILD_DIR}/ --${CXXFLAGS} --${SRCS} ${INCS}

# DO NOT DELETE

../build/AddyPin.o: inc/AddyPin.h
../build/AddyPinAllocator.o: inc/AddyPinAllocator.h inc/AddyUserInfo.h
../build/AddyPinAllocator.o: inc/AddyPin.h
../build/AddyPinApp.o: inc/AddyPinApp.h inc/AddyDB.h inc/AddyPin.h
../build/AddyPinApp.o: inc/AddyUserInfo.h inc/AddyPinAllocator.h
../build/AddyUserInfo.o: inc/AddyUserInfo.h
../build/AddyDB.o: inc/AddyDB.h inc/AddyPin.h inc/AddyUserInfo.h
../build/AddyDB.o: inc/AddyPinAllocator.h
