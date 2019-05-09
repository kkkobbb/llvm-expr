//
// LLVM確認用言語 Exparr コンパイラ
//
#include "AstGenerator.h"
#include "IRGenerator.h"
#include "OptimPass.h"
#include "OutputPass.h"
#include "OutputPassFactory.h"
#include "Ast/Node.h"
#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Path.h>
#include <iostream>
#include <memory>
#include <fstream>

using namespace std;
using namespace expr;

namespace {
	// 標準出力を表すファイル名
	const string STDOUT_FNAME = "-";

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
	llvm::cl::opt<FileTypeKind> FileType("filetype",
			llvm::cl::desc("Choose a file type"),
			llvm::cl::init(FileTypeKind::asm_),
			llvm::cl::values(
				clEnumValN(FileTypeKind::asm_, "asm", "Emit an assembly ('.s') file"),
				clEnumValN(FileTypeKind::obj, "obj", "Emit a native object ('.o') file"),
				clEnumValN(FileTypeKind::bc, "bc", "Emit a llvm bitcode ('.bc') file")),
			llvm::cl::cat(CompilerCategory));
}


int main(int argc, char *argv[])
{
	// CompilerCategory以外は非表示
	llvm::cl::HideUnrelatedOptions({&CompilerCategory});
	llvm::cl::ParseCommandLineOptions(argc, argv, "tiny LLVM compiler\n");

	// 入力ファイルを開く
	ifstream fin;
	fin.open(InputFilename, ios::binary);
	if (!fin) {
		cerr << "Error: open \"" << InputFilename << "\"\n";
		return 1;
	}

	// AST生成
	AstGenerator astGen;
	if (!astGen.generate(fin))
		return 1;
	const auto ast = astGen.get();

	if (PrintAst) {
		// 構文木表示
		ast->print_ast(cout);
		cout << "\n";
		if (!PrintLlvm)
			return 0;
	}

	// IR生成
	IRGenerator irGen;
	const bool irGenerated = irGen.generate(*ast.get());
	const auto m = irGen.get();
	// 入力ファイル名を出力用に設定
	const auto ifname = llvm::sys::path::filename(InputFilename);
	m->setSourceFileName(ifname);

	// 生成に失敗した場合、終了
	if (!irGenerated) {
		if (!PrintLlvm)
			return 1;
		// llvm IR 表示の指定があれば、エラーでもIRは表示する
		llvm::outs() << "\n\n";
		m->print(llvm::outs(), nullptr);
		llvm::outs() << "\n\n";
		return 1;
	}

	if (Optim) {
		// 最適化
		OptimPass opp;
		if (!opp.run(*m.get()))
			return 1;
	}

	if (PrintLlvm) {
		// llvm IR 表示
		m->print(llvm::outs(), nullptr);
		llvm::outs() << "\n";
		return 0;
	}

	// 中間表現表示系のオプションが指定されていた場合、
	// 以降のコード生成は実行されない

	// 出力ファイル名の決定
	string ofname = OutputFilename;
	if (Force)
		ofname = STDOUT_FNAME;

	// ファイル生成
	OutputPassFactory opf;
	const auto op = opf.create(FileType);
	if (op)
		op->run(*m.get(), ofname);

	return 0;
}

