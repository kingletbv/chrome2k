/* Chrome - Small 3D library...
 * Copyright (C) Martijn Boekhorst <m.boekhorst@pi.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/********************************************************************
* FILE : nffmodel.h
* Purpose : Header file for the NFF reading part of the Model
*           structure.
* Description : The NFF file format originates from Sense8's 
*               WorldToolkit. The format supported is 2.0 to 2.1.
********************************************************************/

#ifndef NFFMODEL_H
#define NFFMODEL_H

#include "model.h"
#include "parsebuf.h"

/* Model_LoadNFF(),
 * Generates a Model from an NFF filename.
 */
struct Model *Model_LoadNFF(char *sFilename, int bQuick);

/* Model_LoadNFFBuffer(),
 * Generates a Model from a ParseBuf buffer.
 */
struct Model *Model_LoadNFFBuffer(struct ParseBuf *pBuf, int bQuick);

#endif
