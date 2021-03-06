/**
 * @file LowLevelConvert.h
 * @author Aaron Stanek
 * @brief Functions for converting
 * between types
*/
#pragma once
#include "../Globals/Globals.h"
#include "../ManyType/ManyType.h"

unsigned long convertSignedToUnsigned(const long) noexcept;

long convertUnsignedToSigned(const unsigned long) noexcept;

void convertToBool(ManyType&);

void convertToInt(ManyType&);

void convertToFtype(ManyType&);

void convertToStructureString(ManyType&);