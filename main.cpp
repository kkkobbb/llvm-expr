/*
 * LLVM確認用言語 Exparr コンパイラ
 */
#include <memory>
#include <fstream>

#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Path.h>

#include "Node/AstNode.h"
#include "AstGenerator.h"
#include "IRGenerator.h"
#include "OptimPass.h"
#include "OutputPass.h"
#include "OutputPassFactory.h"


using namespace std;
using namespace expr;

static const string STDOUT_FNAME = "-";



int main(int argc, char *argv[])
{
	// コマンドライン引数の設定
	llvm::cl::OptionCategory CompilerCategory("Compiler Options");
	llvm::cl::opt<string> InputFilename(llvm::cl::Positional,
			llvm::cl::desc("<input file>"),
			llvm::cl::Required,
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<string> OutputFilename("o",
			llvm::cl::desc("Specify output filename"),
			llvm::cl::value_desc("filename"),
			llvm::cl::init(""),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> Optim("O",
			llvm::cl::desc("Enable optimization"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> Force("f",
			llvm::cl::desc("Enable output on terminals"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> PrintAst("print-ast",
			llvm::cl::desc("Print AST"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> PrintLlvm("print-llvm",
			llvm::cl::desc("Print llvm IR"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<FileType> Filetype_("filetype",
			llvm::cl::values(
				clEnumValN(FileType::asm_, "asm", "Emit an assembly ('.s') file"),
				clEnumValN(FileType::obj, "obj", "Emit a native object ('.o') file"),
				clEnumValN(FileType::bc, "bc", "Emit a llvm bitcode ('.bc') file")),
			llvm::cl::desc("Choose a file type"),
			llvm::cl::cat(CompilerCategory));
	// CompilerCategory以外は非表示
	llvm::cl::HideUnrelatedOptions({&CompilerCategory});
	llvm::cl::ParseCommandLineOptions(argc, argv, "tiny LLVM compiler\n");

	ifstream fin;
	fin.open(InputFilename, ios::binary);
	if (!fin) {
		cerr << "Error: open \"" << InputFilename << "\"" << endl;
		return 1;
	}

	// AST生成
	AstGenerator astGen;
	if (!astGen.generate(fin))
		return 1;
	const auto ast = astGen.get();

	if (PrintAst) {
		ast->print_ast(cout);
		cout << endl;
		if (!PrintLlvm)
			return 0;
	}

	// IR生成
	IRGenerator irGen;
	if (!irGen.generate(*ast.get()))
		return 1;
	const auto m = irGen.get();

	// ファイル名設定
	const auto ifname = llvm::sys::path::filename(InputFilename);
	m->setSourceFileName(ifname);

	if (Optim) {
		// 最適化
		OptimPass opp;
		if (!opp.run(*m.get()))
			return 1;
	}

	if (PrintLlvm) {
		m->print(llvm::outs(), nullptr);
		llvm::outs() << "\n";
		return 0;
	}

	// 中間表現表示系のオプションが指定されていた場合、
	// 以降のコード生成は実行されない

	string ofname = OutputFilename;
	if (Force)
		ofname = STDOUT_FNAME;

	OutputPassFactory opf;
	const auto op = opf.create(Filetype_);
	if (op)
		op->run(*m.get(), ofname);

	return 0;
}

