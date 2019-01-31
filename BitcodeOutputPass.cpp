/*
 * ビットコード出力
 */
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Bitcode/BitcodeWriter.h>

#include "BitcodeOutputPass.h"

#define DEFAULT_FNAME "a.bc"


using namespace std;
using namespace llvm;
using namespace expr;



/*
 * Moduleからビットコードを出力する
 *
 * fnameがnullptrの場合、標準出力に出力する
 *
 * 生成に成功した場合、真を返す
 */
bool BitcodeOutputPass::run(Module &module, string &fname)
{
	string *outfname = new string(DEFAULT_FNAME);
	if (!fname.empty())
		outfname = &fname;

	// 目的ファイル生成
	error_code errorInfo;
	raw_fd_ostream outfile(
			*outfname,
			errorInfo,
			sys::fs::OpenFlags::F_None);
	WriteBitcodeToFile(&module, outfile);
	outfile.flush();

	return true;
}


