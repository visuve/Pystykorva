#pragma once

#include <any>
#include <cassert>
#include <chrono>
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
#define NOMINMAX
#include <Windows.h>
#else
#include <icu.h>
#endif