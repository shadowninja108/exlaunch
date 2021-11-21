
/**
 * @file env.h
 * @brief Homebrew environment definitions and utilities.
 * @author plutoo
 * @copyright libnx Authors
 */
#pragma once
#include "../types.h"

/// Returns the handle to the running process.
Handle envGetOwnProcessHandle(void);

/// Sets the handle to the running process.
void envSetOwnProcessHandle(Handle);