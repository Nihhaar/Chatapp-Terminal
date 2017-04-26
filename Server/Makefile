DEPDIR = .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CXX = g++
CXXFLAGS = -w -std=c++11
SRCS = $(wildcard *.h) $(wildcard *.cpp)
DEPS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OUTPUT = server

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all : $(DEPS)
	@echo -e "\n\033[1;32m[INFO] Creating Binary Executable [$(OUTPUT)]\033[0m"
	$(CXX) -pthread -o $(OUTPUT) $(DEPS)

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d
	@echo -e "\n\033[1;32m[CC] $<\033[0m"
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

.PHONY: clean
clean:
	@echo -e "\n\033[1;32m[Cleaning]\033[0m"
	rm -rf $(DEPDIR)
	rm -f *.o
	rm -f server

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))