#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include ../StandardDefinitions.mk


LD_ARA_UTIL=-L$(ARA_UTIL_LIB_DIR)
INC_ARA_UTIL=-I$(ARA_UTIL_INC_DIR)

#Generic and Site Specific Flags
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) -I../AraEvent -I../AraDisplay -I../AraCorrelator -I.  $(INC_ARA_UTIL)
LDFLAGS      += -g $(ROOTLDFLAGS) -lz


# ARA libs go first - first AraDisplay, then AraCorrelator, then AraEvent
# ROOT libs go LAST, they don't depend on anything else.
LIBS          = -L../AraDisplay -lAraDisplay -L../AraCorrelator -lAraCorrelator -L../AraEvent -lAraEvent $(SYSLIBS) $(LD_ARA_UTIL) $(FFTLIBS) -Llib -lAraConfig -lAraKvp -L. -lsqlite3 $(ROOTLIBS) -lMinuit -lGui
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#Now the bits we're actually compiling
ROOT_LIBRARY = libAraWebPlotter.${DLLSUF}
LIB_OBJS = AraEventPlotter.o AraHkPlotter.o AraWebPlotterConfig.o AraTimeHistoHandler.o AraHistoHandler.o AraPlotUtils.o araWebPlotterDict.o
CLASS_HEADERS = AraEventPlotter.h AraHkPlotter.h AraTimeHistoHandler.h AraHistoHandler.h AraWebPlotterConfig.h AraPlotUtils.h


WEB_PLOTTER_BINS =  AraWebPlotter AraTimeWebPlotter getDirFromConfig


all :  lib/libAraConfig.so lib/libAraKvp.so $(ROOT_LIBRARY) $(WEB_PLOTTER_BINS)


#Dependencies
CONFIG_LIB_OBJS =  configLib/configLoad.o configLib/configStore.o configLib/configError.o configLib/configFileSpec.o

lib/libAraConfig.so: $(CONFIG_LIB_OBJS)
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@  $(LDFLAGS) $^ $(OutPutOpt) $@
ifneq ($(subst $(MACOSX_MINOR),,123456),123456)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
		$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $^ \
		   $(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(CONFIG_LIB_OBJS) -o $@
endif

KVP_LIB_OBJS =  kvpLib/keyValuePair.o

lib/libAraKvp.so: $(KVP_LIB_OBJS)
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@  $(LDFLAGS) $^ $(OutPutOpt) $@
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
		$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $^ \
		   $(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(KVP_LIB_OBJS) -o $@
endif


% :  %.$(SRCSUF) $(ROOT_LIBRARY)
	@echo "<**Linking**> "  
# Executable goes first, then AraWebPlotter. They depend on LDFLAGS stuff.
	$(LD)  $(CXXFLAGS) $< -L. -lAraWebPlotter $(LDFLAGS)  $(LIBS) -o $@



#The library
$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(LIBS) $(LDFLAGS) $^ $(OutPutOpt) $@
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
		$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $^ \
		   $(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIBS) $(LIB_OBJS) -o $@
endif

%.$(OBJSUF) : %.$(SRCSUF)
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) -c $< -o  $@

%.$(OBJSUF) : %.C
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) $ -c $< -o  $@

%.$(OBJSUF) : %.c
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) -I. -c $< -o  $@


araWebPlotterDict.C: $(CLASS_HEADERS) LinkDef.h
	@echo "Generating dictionary ..."
	@ rm -f *Dict* 
#	rootcint $@ -c $(CXXFLAGS) $(CLASS_HEADERS) LinkDef.h
	rootcint -f $@ -c $(CXXFLAGS) -p $^


install: $(ROOT_LIBRARY) $(WEB_PLOTTER_BINS)
	install -d $(ARA_UTIL_LIB_DIR)
	install -d $(ARA_UTIL_INC_DIR)
	install -d $(ARA_UTIL_BIN_DIR)
ifeq ($(PLATFORM),macosx)
	install -c -m 755 $(ROOT_LIBRARY) $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY)) $(ARA_UTIL_LIB_DIR)
else
	install -c -m 755 $(ROOT_LIBRARY) $(ARA_UTIL_LIB_DIR)
endif
	install -c -m 644  $(CLASS_HEADERS) $(ARA_UTIL_INC_DIR)
	install -c -m 755 lib/libAraKvp.so $(ARA_UTIL_LIB_DIR)
	install -c -m 755 lib/libAraConfig.so $(ARA_UTIL_LIB_DIR)
	install -c -m 755 $(WEB_PLOTTER_BINS) $(ARA_UTIL_BIN_DIR)

clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF} configLib/*.$(OBJSUF) kvpLib/*.$(OBJSUF)
	@rm -f $(ROOT_LIBRARY) lib/libAraConfig.so lib/libAraKvp.so
	@rm -f $(WEB_PLOTTER_BINS)
	@rm -f $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY))	
	@rm -f $(TEST)
#############################################################################



