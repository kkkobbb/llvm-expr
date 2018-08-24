CXX := g++
LLVM-CONFIG := llvm-config
CXXFLAGS := -g `$(LLVM-CONFIG) --cxxflags`
#LDLIBS := `$(LLVM-CONFIG) --ldflags --system-libs --libs core` -lfl -ly -Xlinker --allow-shlib-undefined
LDLIBS := `$(LLVM-CONFIG) --ldflags --system-libs --libs core`

DEST := exprc
SRC  := main.cpp \
        AstNodeBase.cpp AstNodeConstant.cpp AstNodeType.cpp \
        AstNodeExpression.cpp AstNodeDefinition.cpp AstNodeControl.cpp \
        AstGenerator.cpp IRGenerator.cpp CodeGenerator.cpp

OBJS := $(patsubst %.cpp,%.o,$(SRC)) Lexer.o Parser.o


all: $(DEST)

$(DEST): $(OBJS)
	$(CXX) -Wall -o $@ $^ $(LDLIBS)

# bisonが生成したファイルは-fno-exceptionsが付いているとコンパイルに失敗する
Parser.o: Parser.cc
	$(CXX) $(CXXFLAGS) -fexceptions -c -o $@ $<

%.cc: %.ll
	flex -o $@ $^
%.cc %.hh: %.yy
	bison -v -o $(patsubst %.yy,%.cc,$^) $^


clean:
	$(RM) $(OBJS) $(DEST)
	$(RM) Lexer.cc
	$(RM) Parser.cc Parser.hh stack.hh location.hh position.hh Parser.output


.PHONY: all clean
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


