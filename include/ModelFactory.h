﻿#pragma once

#include <mrl/common/SingletonSupport.h>
#include "ModelExport.h"

class ILogQuery;

class MODEL_EXPORT ModelFactory : public mrl::common::SingletonSupport<ModelFactory> {
public:
	ILogQuery* CreateLogQuery() const;

private:
	SHLIB_COMMON_SINGLETON_SUPPORT_DECLARE(ModelFactory)
};
