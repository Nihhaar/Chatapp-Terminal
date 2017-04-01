DEPDIR = .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CXX = g++
CXXFLAGS = -Wall -std=c++11
SRCS = $(wildcard *.h) $(wildcard *.cpp)
DEPS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OUTPUT = client

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all : $(DEPS)
	$(CXX) -o $(OUTPUT) $(DEPS) 

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

.PHONY: clean
clean:
	rm *.o $(OUTPUT)

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))