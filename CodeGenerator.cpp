/*
 * CodeGenerator
 * ビットコード出力
 */
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Bitcode/BitcodeWriter.h>

#include "CodeGenerator.h"


using namespace std;
using namespace expr;



/*
 * llvm::Moduleからビットコードを出力する
 *
 * fnameがnullptrの場合、標準出力に出力する
 *
 * 生成に成功した場合、真を返す
 */
bool CodeGenerator::genarate(llvm::Module *module, std::string *fname)
{
	string stdoutfile = "-";
	if (fname == nullptr)
		fname = &stdoutfile;

	// 目的ファイル生成
	error_code errorInfo;
	llvm::raw_fd_ostream outfile(
			*fname,
			errorInfo,
			llvm::sys::fs::OpenFlags::F_None);
	llvm::WriteBitcodeToFile(module, outfile);

	return true;
}

