// ネイティブコード出力
// objfile or assembly
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/MCTargetOptions.h>

#include "NativeOutputPass.h"


using namespace std;
using namespace llvm;
using namespace expr;

namespace {
	const string DEFAULT_FNAME = "a.bin";
	const string DEFAULT_FNAME_ASM = "a.s";
	const string DEFAULT_FNAME_OBJ = "a.o";
}



NativeOutputPass::NativeOutputPass(TargetMachine::CodeGenFileType fileType)
	: fileType(fileType), defaultName(getDefaultName(fileType))
{}


// ファイルタイプからデフォルトのファイル名を取得する
string NativeOutputPass::getDefaultName(TargetMachine::CodeGenFileType fileType)
{
	string name = DEFAULT_FNAME;

	// デフォルトのファイル名
	switch (fileType) {
	case TargetMachine::CGFT_AssemblyFile:
		name = DEFAULT_FNAME_ASM;
		break;
	case TargetMachine::CGFT_ObjectFile:
		name = DEFAULT_FNAME_OBJ;
		break;
	case TargetMachine::CGFT_Null:
		// 何もしない
		break;
	}

	return name;
}


// Moduleからネイティブコードを出力する
//
// fnameがnullptrの場合、標準出力に出力する
//
// 生成に成功した場合、真を返す
bool NativeOutputPass::run(Module &module, string &fname)
{
	// Initialize the target registry etc.
	InitializeAllTargetInfos();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();

	const auto targetTriple = sys::getDefaultTargetTriple();
	module.setTargetTriple(targetTriple);

	std::string err;
	const auto target = TargetRegistry::lookupTarget(targetTriple, err);
	if (!target) {
		errs() << err;
		return false;
	}

	const auto cpu = "generic";
	const auto features = "";

	TargetOptions opt;
	// cygwinのgccでコンパイルできるようにするため、例外モデルをdwarfにする
	opt.ExceptionModel = ExceptionHandling::DwarfCFI;
	// gccでコンパイルできるようにするため、PIC
	const auto rm = Optional<Reloc::Model>(Reloc::Model::PIC_);
	const auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

	module.setDataLayout(targetMachine->createDataLayout());

	string outfname = defaultName;
	if (!fname.empty())
		outfname = fname;
	// 目的ファイル生成
	error_code errorInfo;
	raw_fd_ostream outfile(
			outfname,
			errorInfo,
			sys::fs::OpenFlags::F_None);

	legacy::PassManager pass;

	if (targetMachine->addPassesToEmitFile(pass, outfile, fileType)) {
		errs() << "targetMachine can't emit a file of this type";
		return false;
	}

	pass.run(module);
	outfile.flush();

	return true;
}


