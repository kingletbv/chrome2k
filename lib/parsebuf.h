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
* FILE : parsebuf.h
* Purpose : Header file for the ParseBuf structure.
* Description : The ParseBuf structure contains a buffer in which a
*               file is copied. The ParseBuf functions then allow you
*               to perform matching and reading functions on the
*               buffer. It is used for reading file formats such as
*               the NFF file format.
********************************************************************/

#ifndef PARSEBUF_H
#define PARSEBUF_H

struct ParseBuf
{
	long	nCount;		/* Number of characters stored in buffer,
							 * this is not neccessarily the size of
							 * the allocated buffer. */
	long	nPosition;	/* Current position in buffer. */
	char	*pBuf;		/* Actual buffer containing the file. */
};

/* ParseBuf_Construct(),
 * ParseBuf_ConstructM(), (NEEDS stdlib.h INCLUDED)
 * Initializes a ParseBuf structure.
 */
void ParseBuf_Construct(struct ParseBuf *pThis);
#define ParseBuf_ConstructM(pThis)\
(	(pThis)->nCount = 0,\
	(pThis)->nPosition = 0,\
	(pThis)->pBuf = NULL\
)

/* ParseBuf_Destruct(),
 * ParseBuf_DestructM(), (NEEDS stdlib.h INCLUDED)
 * Frees all memory associated with a ParseBuf structure.
 */
void ParseBuf_Destruct(struct ParseBuf *pThis);
#define ParseBuf_DestructM(pThis)\
(	(pThis)->pBuf != NULL ?\
	(	free((void *)(pThis)->pBuf)\
	):(0)\
)

/* ParseBuf_BuildFromFilename(),
 * Reads a file in a ParseBuf structure. */
int ParseBuf_BuildFromFilename(struct ParseBuf *pThis, char *sFilename);

/* ParseBuf_SkipWhitespaces(),
 * Skips all whitespaces at the current position in a ParseBuf
 * structure. */
int ParseBuf_SkipWhitespaces(struct ParseBuf *pThis);

/* ParseBuf_SkipUntilWhitespace(),
 * Skips all non-whitespaces at the current position in a ParseBuf
 * structure. */
int ParseBuf_SkipUntilWhitespace(struct ParseBuf *pThis);

/* ParseBuf_SkipNFFWhitespaces(),
 * Skips all whitespaces at the current position including C++
 * like comments such as "// this is a comment".
 */
int ParseBuf_SkipNFFWhitespaces(struct ParseBuf *pThis);

/* ParseBuf_SkipUntilNFFWhitespace(),
 * Skips all non-whitespaces at the current position in a ParseBuf
 * structure. The difference with ParseBuf_SkipUntilWhitespace() is
 * that C++ like comments such as "// this is a comment" are now
 * also considered as a whitespace.
 */
int ParseBuf_SkipUntilNFFWhitespace(struct ParseBuf *pThis);

/* ParseBuf_MatchString(),
 * Returns 1 if the string at the current position in the
 * ParseBuf matches the string specified. */
int ParseBuf_MatchString(struct ParseBuf *pThis, char *sMatch);

/* ParseBuf_GetInt(),
 * Tries to read an integer value. Note that only positive
 * integers are allowed. */
int ParseBuf_GetInt(struct ParseBuf *pThis, int *pnResult);

/* ParseBuf_GetNFFRGB(),
 * Tries to read an RGB value as it appears in an NFF file.
 * There are two tastes for this, 0xRGB or 0xRRGGBB. The value is
 * always returned as 0xRRGGBB. */
int ParseBuf_GetNFFRGB(struct ParseBuf *pThis, unsigned long *pulResult);

/* ParseBuf_GetFloat(),
 * Tries to read a float value.
 * A float is defined as :
 * [] indicates optional.
 * [-|+]<digits>[.][<digits][[E|e][+|-]<digits>].
 */
int ParseBuf_GetFloat(struct ParseBuf *pThis, float *pfResult);

/* ParseBuf_GetLineString(),
 * Retrieves all characters remaining on a line. Doesn't copy
 * the LF or CR character that ends the line.
 * Returns the number of bytes copied.
 */
int ParseBuf_GetLineString(struct ParseBuf *pThis,
									char *sLinestring,
									int nMaxLength);

/* ParseBuf_EndOfBuffer(),
 * Returns 1 if at the end of the buffer, 0 otherwise.
 */
int ParseBuf_EndOfBuffer(struct ParseBuf *pThis);

#endif
