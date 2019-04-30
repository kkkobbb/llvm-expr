#ifndef OUTPUT_PASS_FACTORY_H
#define OUTPUT_PASS_FACTORY_H

#include "OutputPass.h"
#include <memory>


namespace expr {
	enum FileTypeKind {
		asm_, obj, bc,
	};

	class OutputPassFactory {
		public:
		std::unique_ptr<OutputPass> create(FileTypeKind fileType);
	};
}

#endif  // OUTPUT_PASS_FACTORY_H

