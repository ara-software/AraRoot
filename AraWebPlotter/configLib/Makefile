#-------------------------------------------------------------------------
# Makefile for config library and test programs
#
# Author : G.J.Crone
# Stolen by : R. Nichol
#
#
#-------------------------------------------------------------------------
include ../Makefile.arch

LIB_OBJS =  configLoad.o configStore.o configError.o configFileSpec.o

Name = libConfig
DynLib  = $(WEBPLOTTER_DIR)/lib/$(Name).${DllSuf}

all:  $(DynLib)

$(DynLib): $(LIB_OBJS)
	@/bin/rm -f $(DynLib)	
	@echo "Creating $(DynLib) ..."
	@$(LD) $(LDFLAGS) $(LIBS) $(SOFLAGS) $(LIB_OBJS) -o $(DynLib)
	@chmod 555 $(DynLib)

clean: objclean
	@/bin/rm -f $(Library) $(DynLib)

