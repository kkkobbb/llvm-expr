#ifndef OUTPUT_PASS_FACTORY_H
#define OUTPUT_PASS_FACTORY_H

#include <memory>

#include "OutputPass.h"
#include "BitcodeOutputPass.h"
#include "NativeOutputPass.h"



namespace expr {
	enum FileType {
		asm_, obj, bc,
	};

	class OutputPassFactory {
		public:
		std::unique_ptr<OutputPass> create(FileType filetype) {
			switch (filetype) {
			case FileType::asm_:
				// アセンブリ生成用
				return std::move(std::unique_ptr<NativeOutputPass>(new NativeOutputPass()));
			case FileType::obj:
				// オブジェクトファイル生成用
				// TODO 未実装
				return std::move(std::unique_ptr<OutputPass>());
			case FileType::bc:
				// llvm bitcode 生成用
				return std::move(std::unique_ptr<BitcodeOutputPass>(new BitcodeOutputPass()));
			default:
				// 空 (無効、何もしない用)
				return std::move(std::unique_ptr<OutputPass>());
			};
		}
	};
}


#endif  // OUTPUT_PASS_FACTORY_H

