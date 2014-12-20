/*
* Phoenix 3D 游戏引擎 Version 2.0
*
* Copyright (C) 2009-2011 http://www.Phoenix3d.org/
*
* 文件名称	：	PX2GMatrix.hpp
*
* 版本		:	1.0 (2012/11/12)
*
* 作者		：	more
*
*/

#ifndef PX2GMATRIX_HPP
#define PX2GMATRIX_HPP

#include "PX2MathematicsPre.hpp"
#include "PX2GVector.hpp"

namespace PX2
{

	// Matrices are stored in row-major order, matrix[row][col].
	template <typename Real>
	class GMatrix
	{
	public:
		// Construction and destruction.
		GMatrix (int numRows = 0, int numColumns = 0);
		GMatrix (int numRows, int numColumns, const Real* entry);
		GMatrix (int numRows, int numColumns, const Real** matrix);
		GMatrix (const GMatrix& mat);
		~GMatrix ();

		// Member access.
		void SetSize (int numRows, int numColumns);
		inline void GetSize (int& numRows, int& numColumns) const;
		inline int GetNumRows () const;
		inline int GetNumColumns () const;
		inline int GetNumElements () const;
		inline const Real* GetElements () const;
		inline Real* GetElements ();
		inline const Real* operator[] (int row) const;
		inline Real* operator[] (int row);
		inline const Real& operator() (int row, int col) const;
		inline Real& operator() (int row, int col);
		void SetRow (int row, const GVector<Real>& vec);
		GVector<Real> GetRow (int row) const;
		void SetColumn (int col, const GVector<Real>& vec);
		GVector<Real> GetColumn (int col) const;
		void SetMatrix (int numRows, int numColumns, const Real* entry);
		void SetMatrix (int numRows, int numColumns, const Real** matrix);

		// Support for swapping rows and columns.
		void SwapRows (int row0, int row1);
		void SwapColumns (int col0, int col1);

		// Assignment.
		GMatrix& operator= (const GMatrix& mat);

		// Comparison (for use by STL containers).  The matrices must be the
		// same size (equal number of rows and equal number of columns).
		bool operator== (const GMatrix& mat) const;
		bool operator!= (const GMatrix& mat) const;
		bool operator<  (const GMatrix& mat) const;
		bool operator<= (const GMatrix& mat) const;
		bool operator>  (const GMatrix& mat) const;
		bool operator>= (const GMatrix& mat) const;

		// Arithmetic operations.  The matrices must be the same size (equal
		// number of rows and equal number of columns) for the addition and
		// subtraction of matrices.  The number of columns of 'this' must
		// equal the number of rows of 'mat' for multiplication of matrices.
		GMatrix operator+ (const GMatrix& mat) const;
		GMatrix operator- (const GMatrix& mat) const;
		GMatrix operator* (const GMatrix& mat) const;
		GMatrix operator* (Real scalar) const;
		GMatrix operator/ (Real scalar) const;
		GMatrix operator- () const;

		// Arithmetic updates.  The matrices must be the same size (equal
		// number of rows and equal number of columns) for the addition and
		// subtraction updates.
		GMatrix& operator+= (const GMatrix& mat);
		GMatrix& operator-= (const GMatrix& mat);
		GMatrix& operator*= (Real scalar);
		GMatrix& operator/= (Real scalar);

		// M*v (numColumns(M) = size(v) is required)
		GVector<Real> operator* (const GVector<Real>& vec) const;

		// u^T*M*v (numColumns(M) = size(v) and numRows(M) = size(u) is required)
		Real QForm (const GVector<Real>& u, const GVector<Real>& v) const;

		// M^T
		GMatrix Transpose () const;

		// M^T*mat (numRows(M) = numRows(mat) is required)
		GMatrix TransposeTimes (const GMatrix& mat) const;

		// M*mat^T (numColumns(M) = numColumns(mat) is required)
		GMatrix TimesTranspose (const GMatrix& mat) const;

		// M^T*mat^T (numRows(M) = numColumns(mat) is required)
		GMatrix TransposeTimesTranspose (const GMatrix& mat) const;

		// c * M
		friend GMatrix<Real> operator* (Real scalar, const GMatrix<Real>& mat)
		{
			return mat*scalar;
		}

		// v^T * M (numRows(M) = size(v) is required)
		friend GVector<Real> operator* (const GVector<Real>& vec,
			const GMatrix<Real>& mat)
		{
#ifdef PX2_ASSERT_GMATRIX_OUT_OF_RANGE
			assertion(vec.GetSize() == mat.GetNumRows(),
				"Mismatch in operator*\n");
#endif
			GVector<Real> product(mat.GetNumColumns());
			Real* entry = product;
			for (int c = 0; c < mat.GetNumColumns(); ++c)
			{
				for (int r = 0; r < mat.GetNumRows(); ++r)
				{
					entry[c] += vec[r]*mat[r][c];
				}
			}
			return product;
		}

	protected:
		// The matrix is stored in row-major form as a 1-dimensional array.
		int mNumRows, mNumColumns, mNumElements;
		Real** mEntry;
	};


#include "PX2GMatrix.inl"

	typedef GMatrix<float> GMatrixf;
	typedef GMatrix<double> GMatrixd;

}

#endif