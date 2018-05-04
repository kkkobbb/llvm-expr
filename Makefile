CXX := g++
LLVM-CONFIG := llvm-config-5.0
CXXFLAGS := `$(LLVM-CONFIG) --cxxflags`
LDLIBS := `$(LLVM-CONFIG) --ldflags --system-libs --libs core` -lfl -ly

DEST := exprc
SRC  := main.cpp Ast.cpp IRGenerator.cpp

OBJS := $(patsubst %.cpp,%.o,$(SRC)) Lexer.o Parser.o


.PHONY: all clean

all: $(DEST)

# header
main.o: Ast.h IRGenerator.h Parser.hh
Ast.o: Ast.h
IRGenerator.o: IRGenerator.h ope.h
$(DEST): ope.h  # Parser.cc内でope.hを使用


$(DEST): $(OBJS) $(OBJS_FE)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDLIBS)

# bisonが生成したファイルは-fexceptionsを付ける必要があるのでここに記述する
Parser.o: Parser.cc
	$(CXX) $(CXXFLAGS) -fexceptions -c -o $@ $^

Lexer.cc: Lexer.ll
	flex -o $@ $^
Parser.hh: Parser.cc
Parser.cc: Parser.yy
	bison -v -o $@ $^


clean:
	$(RM) $(OBJS) $(DEST)
	$(RM) Lexer.cc
	$(RM) Parser.cc Parser.hh stack.hh location.hh position.hh Parser.output


