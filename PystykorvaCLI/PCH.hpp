#pragma once

#include <any>
#include <algorithm> 
#include <cassert>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <ranges>
#include <sstream>
#include <set>
#include <stop_token>
#include <thread>
#include <typeindex>

#ifdef _WIN32
#include <icu.h>
#else
#include <unicode/uchar.h>
#endif