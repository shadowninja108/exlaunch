#pragma once

#include "common.hpp"

#include "lib/armv8.hpp"

#include "lib/diag/abort.hpp"
#include "lib/diag/assert.hpp"

#include "lib/log/ilogger.hpp"
#include "lib/log/svc_logger.hpp"

#include "lib/patch/code_patcher.hpp"
#include "lib/patch/patcher_impl.hpp"
#include "lib/patch/random_access_patcher.hpp"
#include "lib/patch/stream_patcher.hpp"

#include "lib/util/math/bitset.hpp"
#include "lib/util/sys/cur_proc_handle.hpp"
#include "lib/util/sys/jit.hpp"
#include "lib/util/sys/mem_layout.hpp"
#include "lib/util/sys/rw_pages.hpp"
#include "lib/util/sys/soc.hpp"
#include "lib/util/crc32.hpp"
#include "lib/util/modules.hpp"
#include "lib/util/murmur3.hpp"
#include "lib/util/ptr_path.hpp"
#include "lib/util/random.hpp"
#include "lib/util/stack_trace.hpp"
#include "lib/util/strings.hpp"
#include "lib/util/typed_storage.hpp"
#include "lib/util/version.hpp"

#include "lib/reloc/reloc.hpp"

#include "lib/hook/base.hpp"
#include "lib/hook/class.hpp"
#include "lib/hook/deprecated.hpp"
#include "lib/hook/inline.hpp"
#include "lib/hook/replace.hpp"
#include "lib/hook/trampoline.hpp"

#include "lib/log/logger_mgr.hpp"
#include <program/loggers.hpp>