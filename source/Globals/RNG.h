/**
 * @file RNG.h
 * @author Aaron Stanek
 * @brief Functions for generating
 * pseudorandom numbers.
*/
#pragma once
#include "Globals.h"

unsigned long RNG_getRaw() noexcept;

long RNG_getInt(const long, const long) noexcept;