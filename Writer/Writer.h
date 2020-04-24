#pragma once

#ifdef __linux__ 
#include <boost/interprocess/shared_memory_object.hpp>
#elif _WIN32
#include <boost/interprocess/windows_shared_memory.hpp>
#endif

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_sharable_mutex.hpp>

#include <iostream>
#include <fmt/format.h>
#include <thread>
#include <chrono>

#include "Schema_generated.h"