//
// OutputPass生成
//

#include "OutputFactory.h"
#include "OutputBase.h"
#include "BitcodeOutput.h"
#include "NativeOutput.h"

using namespace std;
using namespace llvm;
using namespace expr;


unique_ptr<OutputBase> OutputFactory::create(FileTypeKind fileType) {
	unique_ptr<OutputBase> op;

	switch (fileType) {
	case FileTypeKind::asm_:
		// アセンブリ生成用
		op = make_unique<NativeOutput>(TargetMachine::CGFT_AssemblyFile);
		break;
	case FileTypeKind::obj:
		// オブジェクトファイル生成用
		op = make_unique<NativeOutput>(TargetMachine::CGFT_ObjectFile);
		break;
	case FileTypeKind::bc:
		// llvm bitcode 生成用
		op = make_unique<BitcodeOutput>();
		break;
	}

	return move(op);
}

