/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config.c
 * Purpose: File System Configuration
 * Rev.:    V6.4.0
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>FAT File System
// <i>Define FAT File System parameters

//   <o>Number of open files <1-16>
//   <i>Define number of files that can be opened at the same time.
//   <i>Default: 4
#define FAT_MAX_OPEN_FILES      4

// </h>

// <h>Embedded File System
// <i>Define Embedded File System parameters

//   <o>Number of open files <1-16>
//   <i>Define number of files that can be opened at the same time.
//   <i>Default: 4
#define EFS_MAX_OPEN_FILES      4

// </h>



//#include "fs_config.h"
