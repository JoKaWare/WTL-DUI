// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/i18n/icu_util.h"

#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

#include <string>

#include "base/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/string_util.h"
#include "base/sys_string_conversions.h"
//#include "unicode/putil.h"
//#include "unicode/udata.h"

#if defined(OS_MACOSX)
#include "base/mac/foundation_util.h"
#endif

#define ICU_UTIL_DATA_FILE   0
#define ICU_UTIL_DATA_SHARED 1
#define ICU_UTIL_DATA_STATIC 2

#ifndef ICU_UTIL_DATA_IMPL

#if defined(OS_WIN)
#define ICU_UTIL_DATA_IMPL ICU_UTIL_DATA_SHARED
#else
#define ICU_UTIL_DATA_IMPL ICU_UTIL_DATA_STATIC
#endif

#endif  // ICU_UTIL_DATA_IMPL

#if ICU_UTIL_DATA_IMPL == ICU_UTIL_DATA_FILE
#define ICU_UTIL_DATA_FILE_NAME "icudt" U_ICU_VERSION_SHORT "l.dat"
#elif ICU_UTIL_DATA_IMPL == ICU_UTIL_DATA_SHARED
#define ICU_UTIL_DATA_SYMBOL "icudt" U_ICU_VERSION_SHORT "_dat"
#if defined(OS_WIN)
#define ICU_UTIL_DATA_SHARED_MODULE_NAME "icudt.dll"
#endif
#endif

namespace icu_util {

bool Initialize() {
	return true;
}

}  // namespace icu_util
