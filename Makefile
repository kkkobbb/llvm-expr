CXX := g++
LLVM-CONFIG := llvm-config
CXXFLAGS := -g -I. `$(LLVM-CONFIG) --cxxflags`
#LDLIBS := `$(LLVM-CONFIG) --ldflags --system-libs --libs core` -lfl -ly -Xlinker --allow-shlib-undefined
LDLIBS := `$(LLVM-CONFIG) --ldflags --system-libs --libs core`

DEST := exparrc
SRC  := main.cpp \
        $(wildcard Node/*.cpp) \
        AstGenerator.cpp IRGenerator.cpp IRState.cpp OptimPass.cpp \
        OutputPassFactory.cpp BitcodeOutputPass.cpp NativeOutputPass.cpp

OBJS := Lexer.o Parser.o $(patsubst %.cpp,%.o,$(SRC))


all: $(DEST)

$(DEST): $(OBJS)
	$(CXX) -Wall -o $@ $^ $(LDLIBS)

# bisonが生成したファイルは-fno-exceptionsが付いているとコンパイルに失敗する
Parser.o: Parser.cc
	$(CXX) $(CXXFLAGS) -fexceptions -c -o $@ $<

%.cc: %.l
	flex -o $@ $^
%.cc %.hh: %.y
	bison -v -o $(basename $^).cc $^


clean:
	$(RM) $(OBJS) $(DEST)
	$(RM) Lexer.cc
	$(RM) Parser.cc Parser.hh stack.hh location.hh position.hh Parser.output


test: $(DEST)
	test/run.sh $^


.PHONY: all clean test
.PRECIOUS: %.cc %.hh



distclean: clean

### ヘッダ依存関係 自動解決 ###
DEPEND_DIR := _depend
DEPENDS := $(patsubst %.o,$(DEPEND_DIR)/%.d,$(OBJS))

$(DEPEND_DIR)/%.d: %.cpp
	@install -d $(dir $@)
	$(CXX) -MM -MG -MF $@ $<

$(DEPEND_DIR)/%.d: %.cc
	@install -d $(dir $@)
	$(CXX) -MM -MG -MF $@ $<

.PHONY: cleandep
cleandep:
	$(RM) -r $(DEPEND_DIR)
distclean: cleandep

ifneq "$(findstring clean, $(MAKECMDGOALS))" "clean"
  -include $(DEPENDS)
endif


