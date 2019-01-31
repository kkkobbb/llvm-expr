/*
 * ネイティブコード出力
 * objfile or assembly
 */
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include "NativeGenPass.h"

#define DEFAULT_FNAME "a.s"


using namespace std;
using namespace llvm;
using namespace expr;



/*
 * Moduleからネイティブコードを出力する
 *
 * fnameがnullptrの場合、標準出力に出力する
 *
 * 生成に成功した場合、真を返す
 */
bool NativeGenPass::run(Module &module, string &fname)
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
	// gccでコンパイルできるようにするため、PIC
	const auto rm = Optional<Reloc::Model>(Reloc::Model::PIC_);
	const auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

	module.setDataLayout(targetMachine->createDataLayout());

	string *outfname = new string(DEFAULT_FNAME);
	if (!fname.empty())
		outfname = &fname;
	// 目的ファイル生成
	error_code errorInfo;
	raw_fd_ostream outfile(
			*outfname,
			errorInfo,
			sys::fs::OpenFlags::F_None);

	legacy::PassManager pass;
	//const auto fileType = TargetMachine::CGFT_ObjectFile;
	const auto fileType = TargetMachine::CGFT_AssemblyFile;

	if (targetMachine->addPassesToEmitFile(pass, outfile, fileType)) {
		errs() << "targetMachine can't emit a file of this type";
		return false;
	}

	pass.run(module);
	outfile.flush();

	return true;
}


