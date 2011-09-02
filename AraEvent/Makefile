#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include ../StandardDefinitions.mk


LD_ARA_UTIL=-L$(ARA_UTIL_LIB_DIR) 
INC_ARA_UTIL=-I$(ARA_UTIL_INC_DIR) 


#Generic and Site Specific Flags
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_ARA_UTIL) -I. -DARA_ROOT
LDFLAGS      += -g $(ROOTLDFLAGS) 


LIBS          = $(ROOTLIBS) -lMathMore -lMinuit $(SYSLIBS) $(LD_ARA_UTIL) $(FFTLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#Now the bits we're actually compiling
ROOT_LIBRARY = libAraEvent.${DLLSUF}
LIB_OBJS = RawAraTestBedStationHeader.o RawAraTestBedStationEvent.o RawAraStationEvent.o  FullAraTestBedHkEvent.o UsefulAraStationEvent.o AraEventCalibrator.o AraTestBedTriggerMonitor.o AraTestBedHkData.o AraRawTestBedRFChannel.o AraAntennaInfo.o AraGeomTool.o  UsefulAraStationEvent.o UsefulAraTestBedStationEvent.o AraOneEventHkData.o AraOneSensorHkData.o RawAraOneSimpleStationEvent.o RawAraOneStationBlock.o RawAraOneGenericHeader.o RawAraOneStationEvent.o UsefulAraOneStationEvent.o eventDict.o
CLASS_HEADERS =RawAraTestBedStationHeader.h RawAraTestBedStationEvent.h  RawAraStationEvent.h  FullAraTestBedHkEvent.h UsefulAraStationEvent.h AraEventCalibrator.h AraTestBedTriggerMonitor.h AraTestBedHkData.h AraRawTestBedRFChannel.h AraAntennaInfo.h AraGeomTool.h  UsefulAraStationEvent.h UsefulAraTestBedStationEvent.h araTestbedStructures.h AraOneEventHkData.h AraOneSensorHkData.h araOneStructures.h araTestBedDefines.h RawAraOneSimpleStationEvent.h RawAraOneStationBlock.h RawAraOneGenericHeader.h RawAraOneStationEvent.h UsefulAraOneStationEvent.h


all : $(ROOT_LIBRARY)

tests: printStructSizes


#The library
$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(LDFLAGS) $(LIBS) $^ $(OutPutOpt) $@
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


eventDict.C: $(CLASS_HEADERS)
	@echo "Generating dictionary ..."
	@ rm -f *Dict* 
	rootcint $@ -c $(CLASS_HEADERS) LinkDef.h

install: $(ROOT_LIBRARY)
	install -d $(ARA_UTIL_LIB_DIR)
	install -d $(ARA_UTIL_INC_DIR)
ifeq ($(PLATFORM),macosx)
	install -c -m 755 $(ROOT_LIBRARY) $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY)) $(ARA_UTIL_LIB_DIR)
else
	install -c -m 755 $(ROOT_LIBRARY) $(ARA_UTIL_LIB_DIR)
endif
	install -c -m 644  $(CLASS_HEADERS) $(ARA_UTIL_INC_DIR)
	install -d $(ARA_UTIL_CALIB_DIR)
	for file in calib/*.dat calib/*.csv calib/*.txt; do install -c -m 644 "$${file}" $(ARA_UTIL_CALIB_DIR); done


clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(LIBRARY)
	@rm -f $(ROOT_LIBRARY)
	@rm -f $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY))	
	@rm -f $(TEST)
#############################################################################



