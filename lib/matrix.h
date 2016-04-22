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
////////////////////////////////////////////////////////////////////
// File : Matrix.h
// Desc : The BMatrix structure describes a 3 by 3 matrix which is
//        generally used for rotations.
// History
// 13/7/96 : Initial version.


#ifndef MATRIX_H
#define MATRIX_H

#include <vector.h>

class BMatrix
{
public:
	BMatrix()
	{	// Initialize as unary matrix.
		m_arfComponents[0][0] = 1.f;
		m_arfComponents[0][1] = 0.f;
		m_arfComponents[0][2] = 0.f;
		m_arfComponents[1][0] = 0.f;
		m_arfComponents[1][1] = 1.f;
		m_arfComponents[1][2] = 0.f;
		m_arfComponents[2][0] = 0.f;
		m_arfComponents[2][1] = 0.f;
		m_arfComponents[2][2] = 1.f;
	}

	// Transpose, calculates the Transpose matrix of matSrc and
	// stores it in this.
	// Note : The transpose of an orthogonal matrix is also it's
	//        inverse! Rotation matrices ARE orthogonal!
	void Transpose(BMatrix &matSrc)
	{	m_arfComponents[0][0] = matSrc.m_arfComponents[0][0];
		m_arfComponents[0][1] = matSrc.m_arfComponents[1][0];
		m_arfComponents[0][2] = matSrc.m_arfComponents[2][0];
		m_arfComponents[1][0] = matSrc.m_arfComponents[0][1];
		m_arfComponents[1][1] = matSrc.m_arfComponents[1][1];
		m_arfComponents[1][2] = matSrc.m_arfComponents[2][1];
		m_arfComponents[2][0] = matSrc.m_arfComponents[0][2];
		m_arfComponents[2][1] = matSrc.m_arfComponents[1][2];
		m_arfComponents[2][2] = matSrc.m_arfComponents[2][2];
	}

	// Multiplies vSrc with this and returns new vector.
	// For a rotation matrix, this performs the rotation on a vector.
	BVector Multiply(BVector &vSrc)
	{	return BVector(m_arfComponents[0][0] * vSrc[0] +
							m_arfComponents[0][1] * vSrc[1] +
							m_arfComponents[0][2] * vSrc[2],
							m_arfComponents[1][0] * vSrc[0] +
							m_arfComponents[1][1] * vSrc[1] +
							m_arfComponents[1][2] * vSrc[2],
							m_arfComponents[2][0] * vSrc[0] +
							m_arfComponents[2][1] * vSrc[1] +
							m_arfComponents[2][2] * vSrc[2]);
	}

	// Multiplies vSrc with the inverse of this and returns new
	// vector.
	// For a rotation matrix, this performs the opposite rotation on
	// a vector.
	BVector InvMultiply(BVector &vSrc)
	{	return BVector(m_arfComponents[0][0] * vSrc[0] +
							m_arfComponents[1][0] * vSrc[1] +
							m_arfComponents[2][0] * vSrc[2],
							m_arfComponents[0][1] * vSrc[0] +
							m_arfComponents[1][1] * vSrc[1] +
							m_arfComponents[2][1] * vSrc[2],
							m_arfComponents[0][2] * vSrc[0] +
							m_arfComponents[1][2] * vSrc[1] +
							m_arfComponents[2][2] * vSrc[2]);
	}

	// Data members,
	// Note : They're public so we can easily access them.

	// The First index describes the row, the Second index describes
	// the column.
	// Generally, the X vector is stored in the first column, the
	// Y vector is stored in the second column and the Z vector is
	// stored in the third column.
	float	m_arfComponents[3][3];
};

#endif // MATRIX_H
