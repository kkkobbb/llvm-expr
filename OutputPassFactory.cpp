//
// OutputPass生成
//

#include "OutputPassFactory.h"
#include "OutputPass.h"
#include "BitcodeOutputPass.h"
#include "NativeOutputPass.h"
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


unique_ptr<OutputPass> OutputPassFactory::create(FileTypeKind fileType) {
	unique_ptr<OutputPass> op;

	switch (fileType) {
	case FileTypeKind::asm_:
		// アセンブリ生成用
		op = make_unique<NativeOutputPass>(TargetMachine::CGFT_AssemblyFile);
		break;
	case FileTypeKind::obj:
		// オブジェクトファイル生成用
		op = make_unique<NativeOutputPass>(TargetMachine::CGFT_ObjectFile);
		break;
	case FileTypeKind::bc:
		// llvm bitcode 生成用
		op = make_unique<BitcodeOutputPass>();
		break;
	}

	return move(op);
}

