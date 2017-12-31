# Copyright 1994-2012 The MathWorks, Inc.
#
# File: ert_tornado.tmf   
#
# Abstract:
#   Simplified version of matlabroot/rtw/c/tornado/tornado.tmf to support
#   the Embedded Real-Time (ERT) code format on the VxWorks operating
#   system. Supports referenced models and user C++ files.
#
#   This file can be used as a starting point for supporting ERT on the
#   Tornado environment.
#
# You need to configure the Tornado environment:
#   MAKECMD
#   WIND_*
#   TGT_TYPE

MAKECMD          = g:/Tornado_vxWorks_221/host/x86-win32/bin/make
HOST             = ANY
BUILD            = no
BUILD_SUCCESS    = *** Created
SYS_TARGET_FILE  = ert.tlc
MAKEFILE_FILESEP = /

#---------------------- Tokens expanded by make_rtw ----------------------------
#
# The following tokens, when wrapped with "|>" and "<|" are expanded by the 
# build procedure.
#   
MODEL               = Subsystem
MODULES             = Subsystem_data.c 
MAKEFILE            = Subsystem.mk
MATLAB_ROOT         = Q:/R2013a.001/R2013a
ALT_MATLAB_ROOT     = Q:/R2013a.001/R2013a
MASTER_ANCHOR_DIR   = 
START_DIR           = C:/_HPDaten/_Local/__Regler/Frick
S_FUNCTIONS         = 
S_FUNCTIONS_LIB     = 
NUMST               = 1
NCSTATES            = 0
COMPUTER            = PCWIN
BUILDARGS           =  MULTI_INSTANCE_CODE=0 GENERATE_ASAP2=0 TMW_EXTMODE_TESTING=0
MULTITASKING        = 0
MODELREFS           = 
SHARED_SRC          = 
SHARED_SRC_DIR      = 
SHARED_BIN_DIR      = 
SHARED_LIB          = 
GEN_SAMPLE_MAIN     = 1
TARGET_LANG_EXT     = c
EXT_MODE            = 0
EXTMODE_TRANSPORT   = 0
EXTMODE_STATIC      = 0
EXTMODE_STATIC_SIZE = 1000000
CLASSIC_INTERFACE   = 0
NUMST               = 1

#--------------------------- Model and reference models -----------------------
MODELLIB                  = Subsystemlib.lib
MODELREF_LINK_LIBS        = 
MODELREF_INC_PATH         = 
RELATIVE_PATH_TO_ANCHOR   = ..
# NONE: standalone, SIM: modelref sim, RTW: modelref coder target
MODELREF_TARGET_TYPE       = NONE

#-- In the case when directory name contains space ---
ifneq ($(MATLAB_ROOT),$(ALT_MATLAB_ROOT))
MATLAB_ROOT := $(ALT_MATLAB_ROOT)
endif

# Code generation option makevariables that are not expanded in the template makefile
# are automatically passed into make. To avoid this, create NOOP replacements.
# For this custom target, these ERT makevariables are not used.
#
NOOP = 0
NOOP = 0
NOOP = 1
NOOP = 1
NOOP = 0

#------------------------------ Tool Locations ---------------------------------
#
# Tornado Setup on WIN32, assumed macros are in environment.
#
WIND_BASE      = g:/Tornado_vxWorks_221
WIND_REGISTRY  = madmax
WIND_HOST_TYPE = x86-win32

#------------------------- VxWorks  Configuration ------------------------------
#
# Set the following variables to reflect the target processor that you are
# using.  Refer to the VxWorks Programmer's Guide for information on
# appropriate values.
#
TGT_TYPE=68k
ifeq ($(TGT_TYPE),PPC)
  VX_TARGET_TYPE = ppc
  CPU_TYPE       = PPC604
else
  VX_TARGET_TYPE = 68k
  CPU_TYPE       = MC68040
endif

#-------------------------------- GNU Tools ------------------------------------
#
# You may need to modify the following variables if you have installed the GNU
# Tools in a different location.
#
GNUROOT = $(WIND_BASE)/host/$(WIND_HOST_TYPE)
GCC_EXEC_PREFIX = $(GNUROOT)/lib/gcc-lib/
export GCC_EXEC_PREFIX = $(GNUROOT)/lib/gcc-lib/

CC   = $(GNUROOT)/bin/cc$(VX_TARGET_TYPE)
CPP  = $(GNUROOT)/bin/c++$(VX_TARGET_TYPE)
LD   = $(GNUROOT)/bin/ld$(VX_TARGET_TYPE)
AR   = $(GNUROOT)/bin/ar$(VX_TARGET_TYPE)

# External Mode
# Uncomment -DVERBOSE to have information printed to stdout
# To add a new transport layer, see the comments in
#   <matlabroot>/toolbox/simulink/simulink/extmode_transports.m
ifeq ($(EXT_MODE),1)
  EXT_CC_OPTS = -DEXT_MODE #-DVERBOSE
  ifeq ($(EXTMODE_TRANSPORT),0) #tcpip
    EXT_SRC = ext_svr.c updown.c ext_work.c rtiostream_interface.c rtiostream_tcpip.c
  endif
  ifeq ($(EXTMODE_STATIC),1)
    EXT_SRC     += mem_mgr.c
    EXT_CC_OPTS += -DEXTMODE_STATIC -DEXTMODE_STATIC_SIZE=$(EXTMODE_STATIC_SIZE)
  endif
endif

#------------------------------ Include Path -----------------------------------
#
MATLAB_INCLUDES = \
	-I$(MATLAB_ROOT)/simulink/include \
        -I$(MATLAB_ROOT)/extern/include \
	-I$(MATLAB_ROOT)/rtw/c/src \
	-I$(MATLAB_ROOT)/rtw/c/src/ext_mode/common \
	-I$(MATLAB_ROOT)/rtw/c/tornado/devices

# Additional includes 
#
ADD_INCLUDES = \
	-I$(START_DIR)/Subsystem_ert_rtw \
	-I$(START_DIR) \


# Tornado includes
#
TORNADO_INCLUDES = \
	-I$(WIND_BASE)/target/h \
	-I$(WIND_BASE)/host/include/share \
	-I$(GNUROOT)/lib/gcc-lib/m68k-wrs-vxworks/gcc-2.96/include


USER_INCLUDES = 

SHARED_INCLUDES =
ifneq ($(SHARED_SRC_DIR),)
SHARED_INCLUDES = -I$(SHARED_SRC_DIR) 
endif

INCLUDES = -I. -I$(RELATIVE_PATH_TO_ANCHOR) $(MATLAB_INCLUDES) $(ADD_INCLUDES) \
           $(USER_INCLUDES) $(TORNADO_INCLUDES) $(MODELREF_INC_PATH) \
           $(SHARED_INCLUDES)

#-------------------------------- C Flags --------------------------------------
# General User Options 
OPTS      = 
OPT_OPTS  = -O

# Required Options
GCC_FLAGS = -W -nostdinc -fno-builtin -B$(GCC_EXEC_PREFIX)
REQ_OPTS  = -DCPU=$(CPU_TYPE) -DVXWORKS $(GCC_FLAGS)

CC_OPTS   = $(REQ_OPTS) $(OPTS) $(EXT_CC_OPTS)

CPP_REQ_DEFINES = -DMT=$(MULTITASKING) -DNCSTATES=$(NCSTATES) -DNUMST=$(NUMST)

CFLAGS    =$(CC_OPTS) $(CPP_REQ_DEFINES) $(INCLUDES) $(OPT_OPTS)
CPPFLAGS = $(CPP_OPTS) $(CC_OPTS) $(CPP_REQ_DEFINES) $(INCLUDES)

LDFLAGS   = -r

LIBS =
 
LIBS += $(S_FUNCTIONS_LIB)

#----------------------------- Source Files ------------------------------------
SHARED_OUTPUT_OPTS = -o $@
SRCS = $(MODULES) $(S_FUNCTIONS)
ifeq ($(MODELREF_TARGET_TYPE), NONE)
  # Top-level model for code generation
  PRODUCT            = $(RELATIVE_PATH_TO_ANCHOR)/$(MODEL).lo
  BIN_SETTING        = $(LD) $(LDFLAGS) -o $(PRODUCT) $(SYSLIBS) 
  BUILD_PRODUCT_TYPE = "executable"
  SRCS               += $(MODEL).$(TARGET_LANG_EXT)  $(EXT_SRC)
  ifeq ($(GEN_SAMPLE_MAIN),0)
    ifeq ($(MULTI_INSTANCE_CODE), 1)
       SRCS += rt_malloc_main.c
    else
       SRCS += rt_main.c
    endif
  else
    SRCS += ert_main.$(TARGET_LANG_EXT)
  endif

else
  # sub-model for code generation
  PRODUCT            = $(MODELLIB)
  BUILD_PRODUCT_TYPE = "library"
endif


USER_SRCS =
USER_OBJS = $(addsuffix .o, $(basename $(USER_SRCS)))
LOCAL_USER_OBJS = $(notdir $(USER_OBJS))

OBJS      = $(addsuffix .o, $(basename $(SRCS))) $(USER_OBJS)
LINK_OBJS = $(addsuffix .o, $(basename $(SRCS))) $(LOCAL_USER_OBJS)

SHARED_SRC := $(wildcard $(SHARED_SRC))
SHARED_OBJS = $(addsuffix .o, $(basename $(SHARED_SRC)))

#--------------------------------- Rules ---------------------------------------
ifeq ($(MODELREF_TARGET_TYPE),NONE)
# Top-level model for code generation
$(PRODUCT) : $(OBJS) $(SHARED_LIB) $(MODELREF_LINK_LIBS) $(LIBS)
	$(BIN_SETTING) $(LINK_OBJS) $(MODELREF_LINK_LIBS) $(SHARED_LIB) $(LIBS)
	@echo "$(BUILD_SUCCESS) $(BUILD_PRODUCT_TYPE): $@"	
else
# sub-model for code generation
$(PRODUCT) : $(OBJS) $(SHARED_OBJS)
	@rm -f $(MODELLIB)
	$(AR) ruv $(MODELLIB) $(LINK_OBJS)
	@echo "### $(MODELLIB) Created"
	@echo "$(BUILD_SUCCESS) $(BUILD_PRODUCT_TYPE): $@"	
endif

%.o : %.c
	$(CC) -c $(CFLAGS) $<

%.o : %.cpp
	$(CC) -c $(CPPFLAGS) $<

%.o : ../%.c
	$(CC) -c $(CFLAGS) $<

%.o : ../%.cpp
	$(CC) -c $(CPPFLAGS) $<

%.o : $(RELATIVE_PATH_TO_ANCHOR)/%.c
	$(CC) -c $(CFLAGS) $<

%.o : $(RELATIVE_PATH_TO_ANCHOR)/%.cpp
	$(CC) -c $(CPPFLAGS) $<

ifeq ($(GEN_SAMPLE_MAIN),0)
%.o : $(MATLAB_ROOT)/rtw/c/tornado/%.c
	$(CC) -c $(CFLAGS) $<
endif

%.o : $(MATLAB_ROOT)/rtw/c/src/%.c
	$(CC) -c $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/common/%.c
	$(CC) -c $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/rtiostream/rtiostreamtcpip/%.c
	$(CC) -c $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/custom/%.c
	$(CC) -c $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/%.c
	$(CC) -c $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/simulink/src/%.c
	$(CC) -c $(CFLAGS) $<



%.o : $(MATLAB_ROOT)/rtw/c/src/%.cpp
	$(CC) -c $(CPPFLAGS) $<

%.o : $(MATLAB_ROOT)/simulink/src/%.cpp
	$(CC) -c $(CPPFLAGS) $<



%.o : $(MATLAB_ROOT)/simulink/src/%.c
	$(CC) -c $(CFLAGS) $<

%.o : $(MATLAB_ROOT)/simulink/src/%.cpp
	$(CC) -c $(CPPFLAGS) $<

#------------------------------- Libraries -------------------------------------





#----------------------------- Dependencies ------------------------------------

$(OBJS) : $(MAKEFILE) rtw_proj.tmw

$(SHARED_OBJS) : $(SHARED_BIN_DIR)/%.o : $(SHARED_SRC_DIR)/%.c  
	$(CC) -c $(CFLAGS) $(SHARED_OUTPUT_OPTS) $<

$(SHARED_LIB) : $(SHARED_OBJS)
	@echo "### Creating $@ "
	$(AR) r $@ $(SHARED_OBJS)
	@echo "### $@ Created "

