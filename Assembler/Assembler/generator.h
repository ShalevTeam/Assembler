#ifndef _GENERATOR_
#define _GENERATOR_

/** @generator.h
 *
 * @brief This module handles the code files generation
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Yam Shalev.  All rights reserved.
 */

#include "globalDefs.h"

ESucsessFail generateCodeFile();
ESucsessFail generateEntryFile();
ESucsessFail generateExternalsFile();

#endif