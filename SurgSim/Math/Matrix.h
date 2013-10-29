// This file is a part of the OpenSurgSim project.
// Copyright 2012-2013, SimQuest Solutions Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file
/// Definitions of small fixed-size square matrix types.

#ifndef SURGSIM_MATH_MATRIX_H
#define SURGSIM_MATH_MATRIX_H

#include <vector>

#include <SurgSim/Framework/Assert.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU> 		// needed for determinant() and inverse()

namespace SurgSim
{
namespace Math
{

/// A 2x2 matrix of floats.
/// This type (and any structs that contain it) can be safely allocated via new.
typedef Eigen::Matrix<float,  2, 2, Eigen::DontAlign | Eigen::RowMajor>  Matrix22f;

/// A 3x3 matrix of floats.
/// This type (and any structs that contain it) can be safely allocated via new.
typedef Eigen::Matrix<float,  3, 3, Eigen::DontAlign | Eigen::RowMajor>  Matrix33f;

/// A 4x4 matrix of floats.
/// This type (and any structs that contain it) can be safely allocated via new.
typedef Eigen::Matrix<float,  4, 4, Eigen::DontAlign | Eigen::RowMajor>  Matrix44f;

/// A 2x2 matrix of doubles.
/// This type (and any structs that contain it) can be safely allocated via new.
typedef Eigen::Matrix<double, 2, 2, Eigen::DontAlign | Eigen::RowMajor>  Matrix22d;

/// A 3x3 matrix of doubles.
/// This type (and any structs that contain it) can be safely allocated via new.
typedef Eigen::Matrix<double, 3, 3, Eigen::DontAlign | Eigen::RowMajor>  Matrix33d;

/// A 4x4 matrix of doubles.
/// This type (and any structs that contain it) can be safely allocated via new.
typedef Eigen::Matrix<double, 4, 4, Eigen::DontAlign | Eigen::RowMajor>  Matrix44d;

/// A dynamic size diagonal matrix
typedef Eigen::DiagonalMatrix<double, Eigen::Dynamic> DiagonalMatrix;

/// A dynamic size matrix
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::DontAlign> Matrix;

/// Create a rotation matrix corresponding to the specified angle (in radians) and axis.
/// \tparam T the numeric data type used for arguments and the return value.  Can usually be deduced.
/// \tparam VOpt the option flags (alignment etc.) used for the axis vector argument.  Can be deduced.
/// \param angle the angle of the rotation, in radians.
/// \param axis the axis of the rotation.
/// \returns the rotation matrix.
template <typename T, int VOpt>
inline Eigen::Matrix<T, 3, 3> makeRotationMatrix(const T& angle, const Eigen::Matrix<T, 3, 1, VOpt>& axis)
{
	return Eigen::AngleAxis<T>(angle, axis).toRotationMatrix();
}

/// Get the angle (in radians) and axis corresponding to a rotation matrix.
/// \tparam T the numeric data type used for arguments and the return value.  Can usually be deduced.
/// \tparam MOpt the option flags (alignment etc.) used for the rotation matrix argument.  Can be deduced.
/// \tparam VOpt the option flags (alignment etc.) used for the axis vector argument.  Can be deduced.
/// \param matrix the rotation matrix to inspect.
/// \param [out] angle the angle of the rotation, in radians.
/// \param [out] axis the axis of the rotation.
template <typename T, int MOpt, int VOpt>
inline void computeAngleAndAxis(const Eigen::Matrix<T, 3, 3, MOpt>& matrix,
                                T* angle, Eigen::Matrix<T, 3, 1, VOpt>* axis)
{
	Eigen::AngleAxis<T> angleAxis(matrix);
	*angle = angleAxis.angle();
	*axis = angleAxis.axis();
}

/// Get the angle corresponding to a quaternion's rotation, in radians.
/// If you don't care about the rotation axis, this is more efficient than computeAngleAndAxis().
/// \tparam T the numeric data type used for arguments and the return value.  Can usually be deduced.
/// \tparam MOpt the option flags (alignment etc.) used for the rotation matrix argument.  Can be deduced.
/// \param matrix the rotation matrix to inspect.
/// \returns the angle of the rotation, in radians.
template <typename T, int MOpt>
inline T computeAngle(const Eigen::Matrix<T, 3, 3, MOpt>& matrix)
{
	// TODO(bert): there has to be a more efficient way...
	Eigen::AngleAxis<T> angleAxis(matrix);
	return angleAxis.angle();
}

/// Makes view matrix from eye, center and up vectors.
/// \tparam	typename T	T the numeric data type used for arguments and the return value. Can usually be deduced.
/// \tparam	int VOpt  	VOpt the option flags (alignment etc.) used for the axis vector argument.  Can be deduced.
/// \param	eye   	The eyepoint of the camera.
/// \param	center	The center of the camera view direction.
/// \param	up	  	The up vector for the camera.
/// \return	a matrix transforming the world into one that has the camera at the center.
template <typename T, int VOpt>
inline Eigen::Matrix<T, 4, 4> makeViewMatrix(const Eigen::Matrix<T, 3, 1, VOpt>& eye,
											 const Eigen::Matrix<T, 3, 1, VOpt>& center,
											 const Eigen::Matrix<T, 3, 1, VOpt>& up)
{
	Eigen::Matrix<T, 3, 1, VOpt> f = (center - eye).normalized();
	Eigen::Matrix<T, 3, 1, VOpt> s = (f.cross(up)).normalized();
	Eigen::Matrix<T, 3, 1, VOpt> u = s.cross(f).normalized();

	Eigen::Matrix<T, 4, 4> rotation;
	rotation << s[0],  s[1],  s[2], 0.0,
		        u[0],  u[1],  u[2], 0.0, 
		       -f[0], -f[1], -f[2], 0.0,
		         0.0,   0.0,   0.0, 1.0;  

	Eigen::Matrix<T, 4, 4> translation;
	translation << 1.0, 0.0, 0.0, -eye.x(),
				   0.0, 1.0, 0.0, -eye.y(),
				   0.0, 0.0, 1.0, -eye.z(),
				   0.0, 0.0, 0.0,    1.0;

	return rotation*translation;
}

/// Helper method to add a sub-matrix into a matrix, for the sake of clarity
/// \tparam Matrix The matrix type
/// \tparam SubMatrix The sub-matrix type
/// \param subMatrix The sub-matrix
/// \param blockIdRow, blockIdCol The block indices in matrix
/// \param blockSizeRow, blockSizeCol The block size (size of the sub-matrix)
/// \param[out] matrix The matrix to add the sub-matrix into
template <class Matrix, class SubMatrix>
void addSubMatrix(const SubMatrix& subMatrix, unsigned int blockIdRow, unsigned int blockIdCol,
	unsigned int blockSizeRow, unsigned int blockSizeCol, Matrix* matrix)
{
	matrix->block(blockSizeRow * blockIdRow, blockSizeCol * blockIdCol, blockSizeRow, blockSizeCol) += subMatrix;
}

/// Helper method to add a sub-matrix made of squared-blocks into a matrix, for the sake of clarity
/// \tparam Matrix The matrix type
/// \tparam SubMatrix The sub-matrix type
/// \param subMatrix The sub-matrix (containing all the squared-blocks)
/// \param blockIds Vector of block indices (for accessing matrix) corresponding to the blocks in sub-matrix
/// \param blockSize The blocks size
/// \param[out] matrix The matrix to add the sub-matrix blocks into
template <class Matrix, class SubMatrix>
void addSubMatrix(const SubMatrix& subMatrix, const std::vector<unsigned int> blockIds,
	unsigned int blockSize, Matrix* matrix)
{
	const unsigned int numBlocks = blockIds.size();

	for (unsigned int block0 = 0; block0 < numBlocks; block0++)
	{
		unsigned int blockId0 = blockIds[block0];

		for (unsigned int block1 = 0; block1 < numBlocks; block1++)
		{
			unsigned int blockId1 = blockIds[block1];

			matrix->block(blockSize * blockId0, blockSize * blockId1, blockSize, blockSize)
				+= subMatrix.block(blockSize * block0, blockSize * block1, blockSize, blockSize);
		}
	}
}

/// Helper method to set a sub-matrix into a matrix, for the sake of clarity
/// \tparam Matrix The matrix type
/// \tparam SubMatrix The sub-matrix type
/// \param subMatrix The sub-matrix
/// \param blockIdRow, blockIdCol The block indices for row and column in matrix
/// \param blockSizeRow, blockSizeCol The size of the sub-matrix
/// \param[out] matrix The matrix to set the sub-matrix into
template <class Matrix, class SubMatrix>
void setSubMatrix(const SubMatrix& subMatrix, unsigned int blockIdRow, unsigned int blockIdCol,
	unsigned int blockSizeRow, unsigned int blockSizeCol, Matrix* matrix)
{
	matrix->block(blockSizeRow * blockIdRow, blockSizeCol * blockIdCol,
		blockSizeRow, blockSizeCol) = subMatrix;
}

/// Helper method to access a sub-matrix from a matrix, for the sake of clarity
/// \tparam Matrix The matrix type to get the sub-matrix from
/// \param matrix The matrix to get the sub-matrix from
/// \param blockIdRow, blockIdCol The block indices
/// \param blockSizeRow, blockSizeCol The block size
/// \return The requested sub-matrix
/// \note Disable cpplint warnings for use of non-const reference
/// \note Eigen has a specific type for Block that we want to return with read/write access
/// \note therefore the Matrix from which the Block is built from must not be const
template <class Matrix>
Eigen::Block<Matrix> getSubMatrix(Matrix& matrix, unsigned int blockIdRow, unsigned int blockIdCol,  // NOLINT
	unsigned int blockSizeRow, unsigned int blockSizeCol)
{
	return matrix.block(blockSizeRow * blockIdRow, blockSizeCol * blockIdCol, blockSizeRow, blockSizeCol);
}

/// Helper methods to resize/allocate a matrix with a given size (if necessary), and potentially zero it out
/// \param[in,out] A matrix to resize
/// \param numRow, numCol The size to account for
/// \param zeroOut True if the vector v should be filled up with 0, False if not
/// \note This template method is useful to account for different matrix class having different API
template <class Matrix>
void resize(Matrix* A, unsigned int numRow, unsigned int numCol, bool zeroOut = false)
{
	if (A == nullptr)
	{
		return;
	}
	if (A->rows() != static_cast<int>(numRow) && A->cols() != static_cast<int>(numCol))
	{
		A->resize(static_cast<int>(numRow), static_cast<int>(numCol));
	}
	if (zeroOut)
	{
		A->setZero();
	}
}

template <>
void resize<DiagonalMatrix>(DiagonalMatrix* A, unsigned int numRow, unsigned int numCol, bool zeroOut);

};  // namespace Math
};  // namespace SurgSim

#endif  // SURGSIM_MATH_MATRIX_H
