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
* FILE : parsebuf.c
********************************************************************/

#define PARSEBUF_C

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <error.h>

#include "parsebuf.h"

/********************************************************************
* Function : ParseBuf_Construct()
* Purpose : Initializes a ParseBuf structure.
* Pre : pThis points to an uninitialized ParseBuf structure.
* Post : pThis points to an initialized ParseBuf structure.
********************************************************************/
void ParseBuf_Construct(struct ParseBuf *pThis)
{
	/* Call macro version. */
	ParseBuf_ConstructM(pThis);
}

/********************************************************************
* Function : ParseBuf_Destruct()
* Purpose : Frees all memory associated with a ParseBuf structure.
* Pre : pThis points to an initialized ParseBuf structure.
* Post : pThis points to an uninitialized ParseBuf structure that
*        uses no memory.
********************************************************************/
void ParseBuf_Destruct(struct ParseBuf *pThis)
{
	/* Call macro version */
	ParseBuf_DestructM(pThis);
}

/********************************************************************
* Function : ParseBuf_BuildFromFilename()
* Purpose : Reads a whole file into a buffer from a given filename.
* Pre : pThis points to an initialized ParseBuf. sFilename points to
*       a string that contains the full pathname to the file that is
*       to be read.
* Post : If the returnvalue is 1, pThis now contains the file
*        referenced by sFilename, replacing any other file that may
*        be contained in pThis.
*        If the returnvalue is 0, a memory failure occured or the
*        file could not be opened.
********************************************************************/
int ParseBuf_BuildFromFilename(struct ParseBuf *pThis,
										 char *sFilename)
{
	FILE *fp;
	char *p;
	unsigned long fsize;

	/* Open the file. */
	fp = fopen(sFilename, "rb");

	if (fp != NULL)
	{
		/* Retrieve filesize. */
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate the memory for the buffer. */
		p = (char *)malloc(sizeof(char) * fsize);

		if (p != NULL)
		{
			/* Read the file into the buffer. */
			if (fsize == fread((void *)p, sizeof(char), fsize, fp))
			{
				/* Replace buffer in pThis. */
				if (pThis->pBuf != NULL)
					free((void *)pThis->pBuf);
				
				pThis->pBuf = p;
				pThis->nCount = fsize;
				pThis->nPosition = 0;

				/* Success! */
				fclose(fp);
				return 1;
			}
			/* Failure, free buffer. */
			free((void *)p);
		}
		/* Failure, close file. */
		fclose(fp);
	}

	/* Failure. */
	return 0;
}

/********************************************************************
* Function : ParseBuf_SkipWhitespaces()
* Purpose : Skips all whitespaces at the current position in the
*           buffer.
* Pre : pThis points to an initialized ParseBuf structure.
* Post : If the returnvalue is 1, whitespaces were encountered at
*        the current position in the buffer and were skipped.
*        If the returnvalue is 0, no whitespaces were encountered at
*        the current position. It's also possible that the current
*        position is at the end of the buffer.
********************************************************************/
int ParseBuf_SkipWhitespaces(struct ParseBuf *pThis)
{
	char c;
	int bDone;

	if (pThis->nPosition >= pThis->nCount)
		return 0;	/* At end of buffer. */

	bDone = 0;
	while (!bDone)
	{	
		if (pThis->nPosition >= pThis->nCount)
			return 1;
		
		c = pThis->pBuf[pThis->nPosition];

		/* Check for whitespaces (CR, LF, space, tab or vtab). */
		if (isspace(c))
		{	/* Skip it. */
			pThis->nPosition++;
		} else
		{	/* Not a whitespace, we're done. */
			return 1;
		}
	}
}

/********************************************************************
* Function : ParseBuf_SkipUntilWhitespace()
* Purpose : Skips non-whitespace characters until a whitespace
*           character or the end of the buffer.
* Pre : pThis points to an initialized ParseBuf structure.
* Post : If the returnvalue is 1, non-whitespace characters were
*        encountered and skipped.
*        If the returnvalue is 0, the current position was at a
*        whitespace or at the end of the buffer.
********************************************************************/
int ParseBuf_SkipUntilWhitespace(struct ParseBuf *pThis)
{
	char c;
	int bDone;

	if (pThis->nPosition >= pThis->nCount)
		return 0;		/* At end of buffer. */
	
	bDone = 0;

	while (!bDone)
	{
		if (pThis->nPosition >= pThis->nCount)
			return 1;	/* At end of buffer. */

		c = pThis->pBuf[pThis->nPosition];

		/* Check if it is a whitespace. */
		if (isspace(c))
		{
			/* We've reached the end. */
			return 1;
		}
		pThis->nPosition++;
	}
}

/********************************************************************
* Function : ParseBuf_SkipUntilNFFWhitespace()
* Purpose : Skips non-whitespace characters. A whitespace is one of
*           the usual whitespaces or an NFF/C++ style comment.
* Pre : pThis points to an initialized ParseBuf structure.
* Post : If the returnvalue is 1, non-whitespace characters were
*        encountered and skipped.
*        If the returnvalue is 0, the current position was at a
*        whitespace or at the end of the buffer.
********************************************************************/
int ParseBuf_SkipUntilNFFWhitespace(struct ParseBuf *pThis)
{
	char c;
	int bDone;

	if (pThis->nPosition >= pThis->nCount)
		return 0;		/* At end of buffer. */
	
	bDone = 0;

	while (!bDone)
	{
		if (pThis->nPosition >= pThis->nCount)
			return 1;	/* At end of buffer. */

		c = pThis->pBuf[pThis->nPosition];

		/* Check if it is a whitespace. */
		if (isspace(c))
		{
			/* We've reached the end. */
			return 1;
		} else
		/* Check for NFF comment. */
		if (c == '/')
		{
			if ((pThis->nPosition + 1) < pThis->nCount)
			{	if (pThis->pBuf[pThis->nPosition + 1] == '/')
				{	/* Double slash comment encountered.
					 * We've reached the end. */
					return 1;
				}
			}
		}
		pThis->nPosition++;
	}
}

/********************************************************************
* Function : ParseBuf_SkipNFFWhitespaces()
* Purpose : Skips whitespaces, including NFF / C++ style comments
*           formed by double slashes.
* Pre : pThis points to an initialized ParseBuf structure.
* Post : If the returnvalue is 1, whitespaces were encountered at the
*        current position of pThis and were skipped.
*        If the returnvalue is 0, no whitespaces were encountered at
*        the current position of pThis or the current position was at
*        the end of the buffer.
********************************************************************/
int ParseBuf_SkipNFFWhitespaces(struct ParseBuf *pThis)
{
	char c;
	int bDone;
	int bDone2;

	if (pThis->nPosition >= pThis->nCount)
		return 0;	/* At end of buffer. */

	bDone = 0;
	while (!bDone)
	{	
		if (pThis->nPosition >= pThis->nCount)
			return 1;
		
		c = pThis->pBuf[pThis->nPosition];

		/* Check for whitespaces (CR, LF, space, tab or vtab). */
		if (isspace(c))
		{	/* Skip it. */
			pThis->nPosition++;
		} else
		{	/* Check for comments ("//") */
			if (ParseBuf_MatchString(pThis, "//"))
			{
				/* A comment matched, skip until end of line or
				 * end of buffer. */
				bDone2 = 0;
				while (!bDone2)
				{
					if (pThis->nPosition >= pThis->nCount)
						return 1;
					c = pThis->pBuf[pThis->nPosition];

					if ((c == 10) ||
						 (c == 13))
						bDone2 = 1;
				}
			} else
			{	/* Not a whitespace, we're done. */
				return 1;
			}
		}
	}
}


/********************************************************************
* Function : ParseBuf_MatchString()
* Purpose : Performs matching of a string to the current position in
*           ParseBuf pThis.
* Pre : pThis points to an initialized ParseBuf structure, sMatch
*       points to a '\0' terminated string that specifies the string
*       to be matched.
* Post : If the returnvalue is 1, a match was made, pThis's current
*        position has been moved past the matched string.
*        If the returnvalue is 0, no match was made and pThis's
*        current position remains the same.
********************************************************************/
int ParseBuf_MatchString(struct ParseBuf *pThis, char *sMatch)
{
	char *pScanner;
	long nPos;
	int bDone;
	
	pScanner = pThis->pBuf + pThis->nPosition;
	nPos = 0;
	bDone = 0;

	while (1)	/* We exit inside the loop. */
	{
		/* Check if we've reached the end of sMatch. */
		if (sMatch[nPos] == '\0')
		{	/* A match, we've reached the end of sMatch. */
			/* Update buffer position. */
			pThis->nPosition += nPos;
			return 1;
		}
		/* Check that we're not exceeding the buffer's length. */
		if ((nPos + pThis->nPosition) < pThis->nCount)
		{
			/* Check that current character matches. */
			if (*pScanner != sMatch[nPos])
			{	/* No match. */
				return 0;
			}
			/* Increment to next position. */
			nPos++;
			pScanner++;
		} else
		{	/* No match, end of the buffer was reached before the
			 * sMatch string could be matched. */
			return 0;
		}
	}
}

/********************************************************************
* Function : ParseBuf_GetInt()
* Purpose : Reads a positive integer from the current input position.
* Pre : pThis points to an initialized ParseBuf structure, pnResult
*       points to the integer where the result should be stored.
* Post : If the returnvalue is 1, pnResult contains the integer that
*        was read and the buffer position has been updated to after
*        the integer.
*        If the returnvalue is 0, no integer was found.
********************************************************************/
int ParseBuf_GetInt(struct ParseBuf *pThis, int *pnResult)
{
	char *pScanner;
	long nPos;
	int bDone;
	int nResult;

	/* Check if there are any characters left. */
	if (pThis->nPosition >= pThis->nCount)
	{	/* End of buffer. */
		return 0;
	}

	/* Check if the first character is a digit. */
	if (!isdigit(pThis->pBuf[pThis->nPosition]))
	{	/* Not a digit. */
		return 0;
	}

	nPos = 0;
	pScanner = pThis->pBuf + pThis->nPosition;
	bDone = 0;
	nResult = 0;

	while (!bDone)
	{
		/* Check that we're still inside the buffer. */
		if ((nPos + pThis->nPosition) >= pThis->nCount)
		{
			/* End of buffer reached. */
			bDone = 1;
		}
		if (isdigit(*pScanner))
		{
			nResult = nResult * 10 + *pScanner - '0';
			pScanner++;
			nPos++;
		} else
		{
			/* End of digit sequence reached. */
			bDone = 1;
		}
	}
	*pnResult = nResult;
	pThis->nPosition += nPos;
	return 1;
}

/********************************************************************
* Function : ParseBuf_GetNFFRGB()
* Purpose : Reads an RGB triple in the NFF style format. (The NFF
*           style comes in two flavours, one is "0xRGB" (12 bit) the
*           other is "0xRRGGBB" (24 bit). Both flavours are
*           supported.
* Pre : pThis points to an initialized ParseBuf structure, pulResult
*       points to an unsigned long where the result should be stored.
* Post : If the returnvalue is 1, pulResult contains the RGB value
*        encoded in an 0xRRGGBB format (irrelevant of the NFF flavour
*        encountered) and the current position has been updated.
*        If the returnvalue is 0, neither of the two flavours were
*        encountered.
********************************************************************/
int ParseBuf_GetNFFRGB(struct ParseBuf *pThis, unsigned long *pulResult)
{
	long nPos;
	unsigned long ulResult;
	int bDone;
	int tval;
	char c;

	/* First check against "0x" which is common to both tastes. */
	if ((pThis->nPosition + 2) <= (pThis->nCount))
	{
		if ((pThis->pBuf[pThis->nPosition] == '0') &&
			 (pThis->pBuf[pThis->nPosition + 1] == 'x'))
		{
			/* Incrementally grab all hexadecimal digits. */
			ulResult = 0;
			nPos = 2;
			bDone = 0;
			while (!bDone)
			{
				/* Check if we reached the end of the buffer. */
				if ((nPos + pThis->nPosition) >= (pThis->nCount))
				{
					/* End of buffer. */
					bDone = 1;
				} else
				{	/* Check if we passed the long size. */
					if (nPos >= 8)
					{	/* We're done. */
						bDone = 1;
					} else
					{
						/* Get the character. */
						c = pThis->pBuf[nPos + pThis->nPosition];

						/* Check if it is a digit. */
						if (isxdigit(c))
						{
							/* Build nibble value. */
							if ((c >= '0') && (c <= '9'))
								tval = c - '0';
							else
								if ((c >= 'a') && (c <= 'f'))
									tval = c - 'a' + 10;
								else
									if ((c >= 'A') && (c <= 'F'))
										tval = c - 'A' + 10;

							/* Add the nibble. */
							ulResult = (ulResult << 4) + tval;

							/* Increment position. */
							nPos++;
						} else
						{	/* NonDigit reached. */
							bDone = 1;
						}
					}
				}
			}

			/* Check what taste this was. */
			if (nPos > 5)
			{	/* 6 digits...
				 * This is the long 24 bit style. */
				*pulResult = ulResult;
			} else
				if (nPos <= 5)
				{	/* 4 digits...
					 * This is the short 12 bit style. */
					/* Reconstruct to 24 bit value. */
					/* Construct Red value. */
					*pulResult = (ulResult & 0xf00) << 8;
					*pulResult |= (ulResult & 0xf00) << 12;
					/* Construct Green value. */
					*pulResult |= (ulResult & 0x0f0) << 4;
					*pulResult |= (ulResult & 0x0f0) << 8;
					/* Construct Blue value. */
					*pulResult |= (ulResult & 0x00f) << 4;
					*pulResult |= (ulResult & 0x00f);
				} else
				{	return 0;	/* Not a valid value. */
				}
			/* Success... */
			pThis->nPosition += nPos;
			return 1;
		}
	}
	/* Failure... */
	return 0;
}

/********************************************************************
* Function : ParseBuf_GetFloat()
* Purpose : Reads a floating point value from a buffer.
* Pre : pThis points to an intialized ParseBuf structure, pfResult
*       points to a float where the result is to be stored.
* Post : If the returnvalue is 1, pfResult contains the floating
*        point value and the position of pThis has been updated.
*        If the returnvalue is 0, no float was found.
* Note : The floating point format accepted is the following :
*        ([] indicates optional, | indicates OR)
*        [+|-]<digits>[.[<digits>][[E|e][+|-]<digits>]]
********************************************************************/
int ParseBuf_GetFloat(struct ParseBuf *pThis, float *pfResult)
{
	char s[128];	/* Temporary float string buffer. */
	char c;
	int nPos;
	int bDone;
	
	/* Because the ANSI libraries do not contain a routine that
	 * reads a float with a given maximum length and returning the
	 * length of the float, we first need to parse & copy the float
	 * from the input buffer and then call scanf. */
	nPos = 0;
	/* Check for optional + or - sign. */
	if ((pThis->nPosition + nPos) < pThis->nCount)
	{	c = pThis->pBuf[pThis->nPosition + nPos];
		if ((c == '+') ||
			 (c == '-'))
		{	s[nPos] = c;
			nPos++;
		}
	}
	/* Retrieve digits. */
	bDone = 0;
	while (!bDone)
	{
		/* Check bounds. */
		if (nPos >= 127)
			bDone = 1;
		else
			/* Retrieve character. */
			if ((pThis->nPosition + nPos) < pThis->nCount)
			{	c = pThis->pBuf[pThis->nPosition + nPos];
				if (isdigit(c))
				{	s[nPos] = c;
					nPos++;
				} else
					bDone = 1;
			} else
			{	bDone = 1;
			}
	}
	/* Retrieve optional dot (.). */
	if (nPos <= 127)
	{
		if ((pThis->nPosition + nPos) < pThis->nCount)
		{	c = pThis->pBuf[pThis->nPosition + nPos];
			if (c == '.')
			{	s[nPos] = c;
				nPos++;

				/* Retrieve optional digits. */
				bDone = 0;
				while (!bDone)
				{
					/* Check bounds. */
					if (nPos >= 127)
						bDone = 1;
					else
						/* Retrieve character. */
						if ((pThis->nPosition + nPos) < pThis->nCount)
						{	c = pThis->pBuf[pThis->nPosition + nPos];
							if (isdigit(c))
							{	s[nPos] = c;
								nPos++;
							} else
								bDone = 1;
						} else
						{	bDone = 1;
						}
				}

				/* Retrieve optional exponent. */
				if (nPos <= 127)
				{
					if ((pThis->nPosition + nPos) < pThis->nCount)
					{	c = pThis->pBuf[pThis->nPosition + nPos];
						if ((c == 'e') ||
							 (c == 'E'))
						{	s[nPos] = c;
							nPos++;

							/* Retrieve optional + or - sign. */
							if (nPos <= 127)
							{	if ((pThis->nPosition + nPos) < pThis->nCount)
								{	c = pThis->pBuf[pThis->nPosition + nPos];
									if ((c == '+') ||
										 (c == '-'))
									{	s[nPos] = c;
										nPos++;
									}
								}
								/* Retrieve mandatory digit sequence. */
								bDone = 0;
								while (!bDone)
								{
									/* Check bounds. */
									if (nPos >= 127)
										bDone = 1;
									else
										/* Retrieve character. */
										if ((pThis->nPosition + nPos) < pThis->nCount)
										{	c = pThis->pBuf[pThis->nPosition + nPos];
											if (isdigit(c))
											{	s[nPos] = c;
												nPos++;
											} else
												bDone = 1;
										} else
										{	bDone = 1;
										}
								}

								/* All done. */
							}
						}
					}
				}
			}
		}
	}

	/* Now try to scan the float. */
	s[nPos] = '\0';	/* Terminate string. */

	if (1 == sscanf(s, "%f", pfResult))
	{
		/* Update position. */
		pThis->nPosition += nPos;
		return 1;
	}

	/* Failed. */
	return 0;
}

/********************************************************************
* Function : ParseBuf_GetLineString()
* Purpose : Copies a string until an End of Line marker (CR or LF or
*           the end of the buffer.
* Pre : pThis points to an initialized ParseBuf structure,
*       sLineString points to a string where the line should be
*       stored, nMaxLength specifies the length of the sLineString,
*       including null terminator.
* Post : Returnvalue represents the number of characters copied
*        excluding the null terminator. sLineString now contains the
*        string, possibly truncated if it was longer than nMaxLength.
*        The current position of pThis has been placed at the CR or
*        LF (or end of buffer) that ended the string, irrelevant
*        of whether the string was truncated.
*        When the string is truncated, it still has a null terminator
*        at the end.
********************************************************************/
int ParseBuf_GetLineString(struct ParseBuf *pThis,
									char *sLinestring,
									int nMaxLength)
{
	int nPos;
	int bDone;
	char c;
	/* Copy until we've reached a CR, LF or the end of the buffer. */
	nPos = 0;
	bDone = 0;
	while (!bDone)
	{
		if ((pThis->nPosition + nPos) >= pThis->nCount)
		{
			/* End of buffer reached, end the loop. */
			bDone = 1;
		} else
		{
			/* Not the end of the buffer.
			 * Get the current character. */
			c = pThis->pBuf[pThis->nPosition + nPos];

			if ((c == 10) ||
				 (c == 13))
			{
				/* LF or CR reached, end the loop. */
				bDone = 1;
			} else
			{	/* Normal character, just copy it. */
				if (nPos < nMaxLength)
				{	sLinestring[nPos] = c;
				}
				nPos++;
			}
		}
	}

	/* End reached. */
	/* Fill in null terminator. */
	if (nPos < nMaxLength)
	{	/* Current position is still inside string. */
		sLinestring[nPos] = '\0';
	} else
	{	/* Current position is outside string, end at
		 * the last character. */
		if (nMaxLength > 0)
			sLinestring[nMaxLength - 1] = '\0';
	}
	/* Set new position past the string. */
	pThis->nPosition += nPos;
	return nPos;
}

/********************************************************************
* Function : ParseBuf_EndOfBuffer()
* Purpose : Detects wether we've reached the end of the buffer.
* Pre : pThis points to an initialized ParseBuf structure.
* Post : Returns 1 if the current position in the ParseBuf is at the
*        end of the buffer, returns 0 otherwise.
********************************************************************/
int ParseBuf_EndOfBuffer(struct ParseBuf *pThis)
{
	if (pThis->nPosition >= pThis->nCount)
		return 1;
	else
		return 0;
}
