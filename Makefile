CXX := g++
# -fexceptions を付けないとbisonが生成したファイルをビルドできない
CXXFLAGS := `llvm-config --cxxflags` -fexceptions
LDLIBS := `llvm-config --ldflags --system-libs --libs core` -lfl -ly

DEST := exprc
SRC  := main.cpp Ast.cpp IRGenerator.cpp

OBJS := $(patsubst %.cpp,%.o,$(SRC))


all: $(DEST)

.PHONY: all clean distclean

# header
main.o: Ast.h IRGenerator.h Parser.hh
Ast.o: Ast.h
IRGenerator.o: IRGenerator.h ope.h
$(DEST): ope.h  # Parser.cc内でope.hを使用


$(DEST): $(OBJS) Lexer.cc Parser.cc
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDLIBS)

Lexer.cc: Lexer.ll
	flex -o $@ $^

Parser.hh: Parser.cc
Parser.cc: Parser.yy
	bison -v -o $@ $^


clean:
	$(RM) $(OBJS) $(DEST)
	$(RM) Lexer.cc
	$(RM) Parser.cc Parser.hh stack.hh location.hh position.hh Parser.output


distclean: clean


