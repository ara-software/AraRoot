ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)

ARA_UTIL_LIB_DIR=$(ARA_UTIL_INSTALL_DIR)/lib
ARA_UTIL_INC_DIR=$(ARA_UTIL_INSTALL_DIR)/include

LD_ARA_UTIL=-L$(ARA_UTIL_LIB_DIR) -lAraEvent -lz
INC_ARA_UTIL=-I$(ARA_UTIL_INC_DIR)

CXX = g++
CXXFLAGS += $(ROOTCFLAGS)
LIBS += $(ROOTLIBS) -lMinuit 
LDFLAGS += -g $(ROOTLDFLAGS)

TARGETS += doBinWidths doEpsilon doInterleave

#doFourierAnalysis

TARGETS_OBJ += $(TARGETS:%=%.o)

.PRECIOUS: %.o

all: $(TARGETS)

%: %.o
	$(CXX) $(LDFLAGS) $(LIBS) $(LD_ARA_UTIL) $< -o $@

%.o: %.cxx
	$(CXX) -c $(CXXFLAGS) $(INC_ARA_UTIL) $<

clean:
	@rm -f $(TARGETS)
	@rm -f $(TARGETS_OBJ)