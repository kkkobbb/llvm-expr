#ifndef OUTPUT_FACTORY_H
#define OUTPUT_FACTORY_H

#include "OutputBase.h"
#include <memory>


namespace expr {
	enum FileTypeKind {
		asm_, obj, bc,
	};

	class OutputFactory {
	public:
		std::unique_ptr<OutputBase> create(FileTypeKind fileType);
	};
}

#endif  // OUTPUT_FACTORY_H

