CXX := g++
LLVM-CONFIG := llvm-config
CXXFLAGS := `$(LLVM-CONFIG) --cxxflags`
#LDLIBS := `$(LLVM-CONFIG) --ldflags --system-libs --libs core` -lfl -ly -Xlinker --allow-shlib-undefined
LDLIBS := `$(LLVM-CONFIG) --ldflags --system-libs --libs core`

DEST := exprc
SRC  := main.cpp Ast.cpp \
        AstGenerator.cpp IRGenerator.cpp

OBJS := $(patsubst %.cpp,%.o,$(SRC)) Lexer.o Parser.o


all: $(DEST)

# header
main.o: Ast.h AstGenerator.h IRGenerator.h
Ast.o: Ast.h
AstGenerator.o: AstGenerator.h Ast.h Lexer.h Parser.hh
IRGenerator.o: IRGenerator.h ope.h
Parser.o: ope.h


$(DEST): $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDLIBS)

# bisonが生成したファイルは-fexceptionsを付ける必要があるのでここに記述する
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


