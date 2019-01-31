#ifndef OUTPUT_PASS_FACTORY_H
#define OUTPUT_PASS_FACTORY_H

#include <memory>

#include "OutputPass.h"
#include "BitcodeOutputPass.h"
#include "NativeOutputPass.h"



namespace expr {
	class OutputPassFactory {
		public:
		std::unique_ptr<OutputPass> create(bool bitcode) {
			if (bitcode)  // llvm bitcode 生成
				return std::move(std::unique_ptr<BitcodeOutputPass>(new BitcodeOutputPass()));

			// native code 生成
			return std::move(std::unique_ptr<NativeOutputPass>(new NativeOutputPass()));
		}
	};
}


#endif  // OUTPUT_PASS_FACTORY_H

