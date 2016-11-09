// This file is a part of the OpenSurgSim project.
// Copyright 2013-2015, SimQuest Solutions Inc.
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

#include "SurgSim/Collision/TriangleMeshTriangleMeshContact.h"

#include "SurgSim/Collision/CollisionPair.h"
#include "SurgSim/Collision/Representation.h"
#include "SurgSim/DataStructures/AabbTree.h"
#include "SurgSim/DataStructures/AabbTreeNode.h"
#include "SurgSim/DataStructures/IndexedLocalCoordinate.h"
#include "SurgSim/DataStructures/TriangleMesh.h"
#include "SurgSim/Math/Geometry.h"
#include "SurgSim/Math/MeshShape.h"
#include "SurgSim/Math/RigidTransform.h"

using SurgSim::DataStructures::Location;
using SurgSim::DataStructures::TriangleMesh;
using SurgSim::Math::MeshShape;
using SurgSim::Math::RigidTransform3d;
using SurgSim::Math::Vector3d;

namespace SurgSim
{
namespace Collision
{

std::pair<int, int> TriangleMeshTriangleMeshContact::getShapeTypes()
{
	return std::pair<int, int>(SurgSim::Math::SHAPE_TYPE_MESH, SurgSim::Math::SHAPE_TYPE_MESH);
}

#ifdef SURGSIM_DEBUG_TRIANGLETRIANGLECONTACT
namespace
{

/// Asserts the points are coplanar, and prints debug output on the failing condition.
/// \param triangle0, triangle1, triangle2 the vertices of the triangle
/// \param point the point to compare against
/// \throws If the points are not coplanar
void assertIsCoplanar(const Vector3d& triangle0,
					  const Vector3d& triangle1,
					  const Vector3d& triangle2,
					  const Vector3d& point)
{
	SURGSIM_ASSERT(SurgSim::Math::isCoplanar(triangle0, triangle1, triangle2, point))
			<< "Coplanar assertion failed with: "
			"t0 [" << triangle0.transpose() << "], "
			"t1 [" << triangle1.transpose() << "], "
			"t2 [" << triangle2.transpose() << "], "
			"pt [" << point.transpose() << "]";
}

/// Asserts the point is inside the triangle, and prints debug output on the failing condition.
/// \param point the point to compare against
/// \param triangle0, triangle1, triangle2 the vertices of the triangle
/// \param normal the unit normal of the triangle
/// \throws If the point is not inside the triangle
void assertIsPointInsideTriangle(const Vector3d& point,
								 const Vector3d& triangle0,
								 const Vector3d& triangle1,
								 const Vector3d& triangle2,
								 const Vector3d& normal)
{
	SURGSIM_ASSERT(SurgSim::Math::isPointInsideTriangle(point, triangle0, triangle1, triangle2, normal))
			<< "Point inside triangle assertion failed with: "
			"t0 [" << triangle0.transpose() << "], "
			"t1 [" << triangle1.transpose() << "], "
			"t2 [" << triangle2.transpose() << "], "
			"n [" << normal.transpose() << "], "
			"pt [" << point.transpose() << "]";
}

/// Asserts the provided normal and depth minimally resolve the interpenetration of the two triangles, and prints debug
/// output on the failing condition.
/// \param normal the unit normal in the direction to resolve the penetration
/// \param penetrationDepth the depth of penetration to check
/// \param triangleA0, triangleA1, triangleA2 the vertices of the first triangle
/// \param triangleB0, triangleB1, triangleB2 the vertices of the second triangle
/// \throws If the normal and depth do not minimally resolve the interpenetration of the two triangles
void assertIsCorrectNormalAndDepth(const Vector3d& normal,
								   double penetrationDepth,
								   const Vector3d& triangleA0,
								   const Vector3d& triangleA1,
								   const Vector3d& triangleA2,
								   const Vector3d& triangleB0,
								   const Vector3d& triangleB1,
								   const Vector3d& triangleB2)
{
	Vector3d correction = normal * (penetrationDepth - SurgSim::Math::Geometry::DistanceEpsilon);

	SURGSIM_ASSERT(SurgSim::Math::doesIntersectTriangleTriangle(
					   (Vector3d)(triangleA0 + correction),
					   (Vector3d)(triangleA1 + correction),
					   (Vector3d)(triangleA2 + correction),
					   triangleB0, triangleB1, triangleB2))
			<< "Correct normal and depth assertion failed with: "
			"n [" << normal.transpose() << "], "
			"d [" << penetrationDepth << "], "
			"a0 [" << triangleA0.transpose() << "], "
			"a1 [" << triangleA1.transpose() << "], "
			"a2 [" << triangleA2.transpose() << "], "
			"b0 [" << triangleB0.transpose() << "], "
			"b1 [" << triangleB1.transpose() << "], "
			"b2 [" << triangleB2.transpose() << "]";

	correction = normal * (penetrationDepth + 2.0 * SurgSim::Math::Geometry::DistanceEpsilon);

	SURGSIM_ASSERT(!SurgSim::Math::doesIntersectTriangleTriangle(
					   (Vector3d)(triangleA0 + correction),
					   (Vector3d)(triangleA1 + correction),
					   (Vector3d)(triangleA2 + correction),
					   triangleB0, triangleB1, triangleB2))
			<< "Correct normal and depth assertion failed with: "
			"n [" << normal.transpose() << "], "
			"d [" << penetrationDepth << "], "
			"a0 [" << triangleA0.transpose() << "], "
			"a1 [" << triangleA1.transpose() << "], "
			"a2 [" << triangleA2.transpose() << "], "
			"b0 [" << triangleB0.transpose() << "], "
			"b1 [" << triangleB1.transpose() << "], "
			"b2 [" << triangleB2.transpose() << "]";
}

} // namespace
#endif //SURGSIM_DEBUG_TRIANGLETRIANGLECONTACT

std::list<std::shared_ptr<Contact>> TriangleMeshTriangleMeshContact::calculateDcdContact(
									 const Math::MeshShape& meshA,
									 const Math::RigidTransform3d& meshAPose,
									 const Math::MeshShape& meshB,
									 const Math::RigidTransform3d& meshBPose) const
{

	std::list<std::shared_ptr<Contact>> contacts;

	std::list<SurgSim::DataStructures::AabbTree::TreeNodePairType> intersectionList
		= meshA.getAabbTree()->spatialJoin(*(meshB.getAabbTree()));

	double depth = 0.0;
	Vector3d normal;
	Vector3d penetrationPointA, penetrationPointB;

	for (auto intersection = intersectionList.begin(); intersection != intersectionList.end(); ++intersection)
	{
		std::shared_ptr<SurgSim::DataStructures::AabbTreeNode> nodeA = intersection->first;
		std::shared_ptr<SurgSim::DataStructures::AabbTreeNode> nodeB = intersection->second;

		std::list<size_t> triangleListA;
		std::list<size_t> triangleListB;

		nodeA->getIntersections(nodeB->getAabb(), &triangleListA);
		nodeB->getIntersections(nodeA->getAabb(), &triangleListB);

		for (auto i = triangleListA.begin(); i != triangleListA.end(); ++i)
		{
			const Vector3d& normalA = meshA.getNormal(*i);
			if (normalA.isZero())
			{
				continue;
			}

			auto verticesA = meshA.getTrianglePositions(*i);

			for (auto j = triangleListB.begin(); j != triangleListB.end(); ++j)
			{
				const Vector3d& normalB = meshB.getNormal(*j);
				if (normalB.isZero())
				{
					continue;
				}

				auto verticesB = meshB.getTrianglePositions(*j);

				// Check if the triangles intersect.
				if (Math::calculateContactTriangleTriangle(verticesA[0], verticesA[1], verticesA[2],
						verticesB[0], verticesB[1], verticesB[2],
						normalA, normalB, &depth,
						&penetrationPointA, &penetrationPointB,
						&normal))
				{
#ifdef SURGSIM_DEBUG_TRIANGLETRIANGLECONTACT
					assertIsCoplanar(verticesA[0], verticesA[1], verticesA[2], penetrationPointA);
					assertIsCoplanar(verticesB[0], verticesB[1], verticesB[2], penetrationPointB);

					assertIsPointInsideTriangle(
						penetrationPointA, verticesA[0], verticesA[1], verticesA[2], normalA);
					assertIsPointInsideTriangle(penetrationPointB, verticesB[0], verticesB[1], verticesB[2], normalB);

					assertIsCorrectNormalAndDepth(normal, depth, verticesA[0], verticesA[1], verticesA[2],
												  verticesB[0], verticesB[1], verticesB[2]);
#endif

					// Create the contact.
					std::pair<Location, Location> penetrationPoints;
					Vector3d barycentricCoordinate;
					Math::barycentricCoordinates(penetrationPointA, verticesA[0], verticesA[1], verticesA[2],
												 normalA, &barycentricCoordinate);
					penetrationPoints.first.triangleMeshLocalCoordinate.setValue(
						DataStructures::IndexedLocalCoordinate(*i, barycentricCoordinate));
					Math::barycentricCoordinates(penetrationPointB, verticesB[0], verticesB[1], verticesB[2],
												 normalB, &barycentricCoordinate);
					penetrationPoints.second.triangleMeshLocalCoordinate.setValue(
						DataStructures::IndexedLocalCoordinate(*j, barycentricCoordinate));

					penetrationPoints.first.rigidLocalPosition.setValue(meshAPose.inverse() * penetrationPointA);
					penetrationPoints.second.rigidLocalPosition.setValue(meshBPose.inverse() * penetrationPointB);

					contacts.emplace_back(std::make_shared<Contact>(
											  COLLISION_DETECTION_TYPE_DISCRETE, std::abs(depth), 1.0,
											  Vector3d::Zero(), normal, penetrationPoints));
				}
			}
		}
	}
	return contacts;
}

std::list<std::shared_ptr<Contact>> TriangleMeshTriangleMeshContact::calculateCcdContact(
	const Math::MeshShape& shape1AtTime0, const Math::RigidTransform3d& pose1AtTime0,
	const Math::MeshShape& shape1AtTime1, const Math::RigidTransform3d& pose1AtTime1,
	const Math::MeshShape& shape2AtTime0, const Math::RigidTransform3d& pose2AtTime0,
	const Math::MeshShape& shape2AtTime1, const Math::RigidTransform3d& pose2AtTime1) const
{
	using Math::calculateCcdContactSegmentSegment;
	using Math::calculateCcdContactPointTriangle;
	double epsilon = Math::Geometry::DistanceEpsilon;

	std::list<std::shared_ptr<Contact>> contacts;

	SURGSIM_ASSERT(shape1AtTime0.getNumTriangles() > 0);
	SURGSIM_ASSERT(shape1AtTime0.getNumTriangles() == shape1AtTime1.getNumTriangles());
	SURGSIM_ASSERT(shape2AtTime0.getNumTriangles() > 0);
	SURGSIM_ASSERT(shape2AtTime0.getNumTriangles() == shape2AtTime1.getNumTriangles());

	for (size_t triangle1Id = 0; triangle1Id < shape1AtTime0.getNumTriangles(); triangle1Id++)
	{
		auto triangle1T0 = shape1AtTime0.getTriangle(triangle1Id);
		auto triangle1T1 = shape1AtTime1.getTriangle(triangle1Id);

		SURGSIM_ASSERT(triangle1T0.verticesId == triangle1T1.verticesId) << "Triangles are different:\n" <<
			"(" << triangle1T0.verticesId[0] << "," << triangle1T0.verticesId[1] << "," <<
			triangle1T0.verticesId[2] << ")\n" <<
			"(" << triangle1T1.verticesId[0] << "," << triangle1T1.verticesId[1] << "," <<
			triangle1T1.verticesId[2] << ")\n" <<
			"triangleT0.valid = " << triangle1T0.isValid << "\ntriangleT1.valid = " << triangle1T1.isValid;

		std::pair<Math::Vector3d, Math::Vector3d> t1v0 = std::make_pair(
			shape2AtTime0.getVertexPosition(triangle1T0.verticesId[0]),
			shape2AtTime1.getVertexPosition(triangle1T1.verticesId[0]));
		std::pair<Math::Vector3d, Math::Vector3d> t1v1 = std::make_pair(
			shape2AtTime0.getVertexPosition(triangle1T0.verticesId[1]),
			shape2AtTime1.getVertexPosition(triangle1T1.verticesId[1]));
		std::pair<Math::Vector3d, Math::Vector3d> t1v2 = std::make_pair(
			shape2AtTime0.getVertexPosition(triangle1T0.verticesId[2]),
			shape2AtTime1.getVertexPosition(triangle1T1.verticesId[2]));

		Math::Aabbd triangle1Aabb;
		triangle1Aabb.extend(t1v0.first);
		triangle1Aabb.extend(t1v0.second);
		triangle1Aabb.extend(t1v1.first);
		triangle1Aabb.extend(t1v1.second);
		triangle1Aabb.extend(t1v2.first);
		triangle1Aabb.extend(t1v2.second);

		for (size_t triangleId = 0; triangleId < shape2AtTime0.getNumTriangles(); triangleId++)
		{
			auto triangle2T0 = shape2AtTime0.getTriangle(triangleId);
			auto triangle2T1 = shape2AtTime1.getTriangle(triangleId);

			SURGSIM_ASSERT(triangle2T0.verticesId == triangle2T1.verticesId) << "Triangles are different:\n" <<
				"(" << triangle2T0.verticesId[0] << "," << triangle2T0.verticesId[1] << "," <<
				triangle2T0.verticesId[2] << ")\n" <<
				"(" << triangle2T1.verticesId[0] << "," << triangle2T1.verticesId[1] << "," <<
				triangle2T1.verticesId[2] << ")\n" <<
				"triangleT0.valid = " << triangle2T0.isValid << "\ntriangleT1.valid = " << triangle2T1.isValid;

			std::pair<Math::Vector3d, Math::Vector3d> t2v0 = std::make_pair(
				shape2AtTime0.getVertexPosition(triangle2T0.verticesId[0]),
				shape2AtTime1.getVertexPosition(triangle2T1.verticesId[0]));
			std::pair<Math::Vector3d, Math::Vector3d> t2v1 = std::make_pair(
				shape2AtTime0.getVertexPosition(triangle2T0.verticesId[1]),
				shape2AtTime1.getVertexPosition(triangle2T1.verticesId[1]));
			std::pair<Math::Vector3d, Math::Vector3d> t2v2 = std::make_pair(
				shape2AtTime0.getVertexPosition(triangle2T0.verticesId[2]),
				shape2AtTime1.getVertexPosition(triangle2T1.verticesId[2]));

			Math::Aabbd triangle2Aabb;
			triangle2Aabb.extend(t2v0.first);
			triangle2Aabb.extend(t2v0.second);
			triangle2Aabb.extend(t2v1.first);
			triangle2Aabb.extend(t2v1.second);
			triangle2Aabb.extend(t2v2.first);
			triangle2Aabb.extend(t2v2.second);

			if (!SurgSim::Math::doAabbIntersect(triangle1Aabb, triangle2Aabb))
			{
				continue;
			}

			double earliestTimeOfImpact = std::numeric_limits<double>::max();
			double triangle1Alpha = -1.0;  //!< Barycentric coordinates of P1 in triangle t1v0t1v1t1v2
			double triangle1Beta = -1.0;  //!< P1 = t1v0 + triangle1Alpha.t1v0t1v1 + triangle1Beta.t1v0t1v2
			double triangle2Alpha = -1.0;  //!< Barycentric coordinates of P2 in triangle t2v0t2v1t2v2
			double triangle2Beta = -1.0;   //!< P2 = t2v0 + triangleAlpha.t2v0t2v1 + triangleBeta.t2v0t2v2

			Math::Vector3d t1n = ((t1v1.first - t1v0.first).cross(t1v2.first - t1v0.first));
			if (t1n.norm() < Math::Geometry::DistanceEpsilon)
			{
				SURGSIM_LOG_WARNING(Framework::Logger::getLogger("TriangleMeshTriangleMeshContact")) <<
					"The triangle mesh contains a degenerate triangle (null normal)";
			}
			
			Math::Vector3d t2n = ((t2v1.first - t2v0.first).cross(t2v2.first - t2v0.first));
			if (t2n.norm() < Math::Geometry::DistanceEpsilon)
			{
				SURGSIM_LOG_WARNING(Framework::Logger::getLogger("TriangleMeshTriangleMeshContact")) <<
					"The triangle mesh contains a degenerate triangle (null normal)";
			}

			t1n.normalize();
			t2n.normalize();
			
			// Check collision at time t = 0
			Math::Vector3d pt1;
			Math::Vector3d pt2;
			bool segmentSegmentCcdFound = false;
			bool t1VertexThroughT2 = false;

			if (Math::distanceTriangleTriangle(
				t1v0.first, t1v1.first, t1v2.first,
				t2v0.first, t2v1.first, t2v2.first,
				&pt1, &pt2) <= 0.0)
			{
				Math::Vector3d baryCoordTriangle1;
				Math::Vector3d baryCoordTriangle2;
				if (!Math::barycentricCoordinates(pt1, t1v0.first, t1v1.first, t1v2.first, &baryCoordTriangle1))
				{
					SURGSIM_LOG_WARNING(Framework::Logger::getLogger("TriangleMeshTriangleMeshContact")) <<
						"[t=0] Could not deduce the barycentric coordinate of (" << pt1.transpose() <<
						") in the triangle (" << t1v0.first.transpose() << ")  (" << t1v1.first.transpose() <<
						") (" << t1v2.first.transpose() << ")";
				}
				if (!Math::barycentricCoordinates(pt2, t2v0.first, t2v1.first, t2v2.first, &baryCoordTriangle2))
				{
					SURGSIM_LOG_WARNING(Framework::Logger::getLogger("TriangleMeshTriangleMeshContact")) <<
						"[t=0] Could not deduce the barycentric coordinate of (" << pt2.transpose() <<
						") in the triangle (" << t2v0.first.transpose() << ")  (" << t2v1.first.transpose() <<
						") (" << t2v2.first.transpose() << ")";
				}
				segmentSegmentCcdFound = false;
				earliestTimeOfImpact = 0.0;
				triangle1Alpha = baryCoordTriangle1[1];
				triangle1Beta = baryCoordTriangle1[2];
				triangle2Alpha = baryCoordTriangle2[1];
				triangle2Beta = baryCoordTriangle2[2];
			}
			else
			{
				// No collision at time t = 0, let's look for collision in the interval ]0..1]

				// Calculate Segment/Segment ccd
				double timeOfImpact;
				double t1Factor, t2Factor;
				if (calculateCcdContactSegmentSegment(t1v0, t1v1, t2v0, t2v1, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = t1Factor;
						triangle1Beta = 0.0;
						triangle2Alpha = t2Factor;
						triangle2Beta = 0.0;
					}
				}

				if (calculateCcdContactSegmentSegment(t1v0, t1v1, t2v1, t2v2, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = t1Factor;
						triangle1Beta = 0.0;
						triangle2Alpha = 1.0 - t2Factor; // P = P0 + P0P1.(1 - tFactor) + P0P2.tFactor
						triangle2Beta = t2Factor;
					}
				}

				if (calculateCcdContactSegmentSegment(t1v0, t1v1, t2v2, t2v0, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = t1Factor;
						triangle1Beta = 0.0;
						triangle2Alpha = 0.0; // P = P0 + P0P2.(1 - tFactor)
						triangle2Beta = 1.0 - t2Factor;
					}
				}

				if (calculateCcdContactSegmentSegment(t1v1, t1v2, t2v0, t2v1, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 1.0 - t1Factor; // P = P0 + P0P1.(1 - tFactor) + P0P2.tFactor
						triangle1Beta = t1Factor;
						triangle2Alpha = t2Factor;
						triangle2Beta = 0.0;
					}
				}

				if (calculateCcdContactSegmentSegment(t1v1, t1v2, t2v1, t2v2, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 1.0 - t1Factor; // P = P0 + P0P1.(1 - tFactor) + P0P2.tFactor
						triangle1Beta = t1Factor;
						triangle2Alpha = 1.0 - t2Factor; // P = P0 + P0P1.(1 - tFactor) + P0P2.tFactor
						triangle2Beta = t2Factor;
					}
				}

				if (calculateCcdContactSegmentSegment(t1v1, t1v2, t2v2, t2v0, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 1.0 - t1Factor; // P = P0 + P0P1.(1 - tFactor) + P0P2.tFactor
						triangle1Beta = t1Factor;
						triangle2Alpha = 0.0; // P = P0 + P0P2.(1 - tFactor)
						triangle2Beta = 1.0 - t2Factor;
					}
				}

				if (calculateCcdContactSegmentSegment(t1v2, t1v0, t2v0, t2v1, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 0.0; // P = P0 + P0P2.(1 - tFactor)
						triangle1Beta = 1.0 - t1Factor;
						triangle2Alpha = t2Factor;
						triangle2Beta = 0.0;
					}
				}

				if (calculateCcdContactSegmentSegment(t1v2, t1v0, t2v1, t2v2, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 0.0; // P = P0 + P0P2.(1 - tFactor)
						triangle1Beta = 1.0 - t1Factor;
						triangle2Alpha = 1.0 - t2Factor; // P = P0 + P0P1.(1 - tFactor) + P0P2.tFactor
						triangle2Beta = t2Factor;
					}
				}

				if (calculateCcdContactSegmentSegment(t1v2, t1v0, t2v2, t2v0, &timeOfImpact, &t1Factor, &t2Factor))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 0.0; // P = P0 + P0P2.(1 - tFactor)
						triangle1Beta = 1.0 - t1Factor;
						triangle2Alpha = 0.0; // P = P0 + P0P2.(1 - tFactor)
						triangle2Beta = 1.0 - t2Factor;
					}
				}

				// Calculate Point/Triangle ccd
				double u, v;
				if (calculateCcdContactPointTriangle(t1v0, t2v0, t2v1, t2v2, &timeOfImpact, &u, &v))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = false;
						t1VertexThroughT2 = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 0.0;
						triangle1Beta = 0.0;
						triangle2Alpha = u;
						triangle2Beta = v;
					}
				}

				if (calculateCcdContactPointTriangle(t1v1, t2v0, t2v1, t2v2, &timeOfImpact, &u, &v))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = false;
						t1VertexThroughT2 = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 1.0;
						triangle1Beta = 0.0;
						triangle2Alpha = u;
						triangle2Beta = v;
					}
				}

				if (calculateCcdContactPointTriangle(t1v2, t2v0, t2v1, t2v2, &timeOfImpact, &u, &v))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = false;
						t1VertexThroughT2 = true;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = 0.0;
						triangle1Beta = 1.0;
						triangle2Alpha = u;
						triangle2Beta = v;
					}
				}

				if (calculateCcdContactPointTriangle(t2v0, t1v0, t1v1, t1v2, &timeOfImpact, &u, &v))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = false;
						t1VertexThroughT2 = false;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = u;
						triangle1Beta = v;
						triangle2Alpha = 0.0;
						triangle2Beta = 0.0;
					}
				}

				if (calculateCcdContactPointTriangle(t2v1, t1v0, t1v1, t1v2, &timeOfImpact, &u, &v))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = false;
						t1VertexThroughT2 = false;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = u;
						triangle1Beta = v;
						triangle2Alpha = 1.0;
						triangle2Beta = 0.0;
					}
				}

				if (calculateCcdContactPointTriangle(t2v2, t1v0, t1v1, t1v2, &timeOfImpact, &u, &v))
				{
					if (timeOfImpact < earliestTimeOfImpact)
					{
						segmentSegmentCcdFound = false;
						t1VertexThroughT2 = false;
						earliestTimeOfImpact = timeOfImpact;
						triangle1Alpha = u;
						triangle1Beta = v;
						triangle2Alpha = 0.0;
						triangle2Beta = 1.0;
					}
				}
			}

			// False positive from the AABB, no collision found
			if (earliestTimeOfImpact == std::numeric_limits<double>::max())
			{
				continue;
			}

			SURGSIM_ASSERT(triangle1Alpha >= -epsilon && triangle1Beta >= -epsilon &&
				triangle1Alpha + triangle1Beta <= (1.0 + epsilon + epsilon)) <<
				"earliestTimeOfImpact = " << earliestTimeOfImpact <<
				"; triangleAlpha = " << triangle1Alpha <<
				"; triangleBeta = " << triangle1Beta <<
				"; triangleAlpha + triangleBeta = " << triangle1Alpha + triangle1Beta;
			SURGSIM_ASSERT(triangle2Alpha >= -epsilon && triangle2Beta >= -epsilon &&
				triangle2Alpha + triangle2Beta <= (1.0 + epsilon + epsilon)) <<
				"earliestTimeOfImpact = " << earliestTimeOfImpact <<
				"; triangleAlpha = " << triangle2Alpha <<
				"; triangleBeta = " << triangle2Beta <<
				"; triangleAlpha + triangleBeta = " << triangle2Alpha + triangle2Beta;

			Math::Vector3d T1, T2, normal;
			double penentrationDepthAtT1;
			
			Math::Vector3d t1T0T1 = t1v1.second - t1v0.second;
			Math::Vector3d t1T0T2 = t1v2.second - t1v0.second;
			T1 = t1v0.second + triangle2Alpha * t1T0T1 + triangle2Beta * t1T0T2;
			
			Math::Vector3d t2T0T1 = t2v1.second - t2v0.second;
			Math::Vector3d t2T0T2 = t2v2.second - t2v0.second;
			T2 = t2v0.second + triangle2Alpha * t2T0T1 + triangle2Beta * t2T0T2;

			normal = (segmentSegmentCcdFound) ? (T1 - T2).normalized() :
				((t1VertexThroughT2) ? t2T0T1.cross(t2T0T2).normalized() : -t1T0T1.cross(t1T0T2).normalized());
			penentrationDepthAtT1 = (T2 - T1).dot(normal);
			
			Math::Vector triangle1BaryCoord(3);
			triangle1BaryCoord << 1.0 - triangle1Alpha - triangle1Beta, triangle1Alpha, triangle1Beta;
			DataStructures::IndexedLocalCoordinate localCoordinateTriangle1(triangleId, triangle1BaryCoord);
			// The location related to the TriangleMesh can carry a TRIANGLE information
			// e.g. part of a Mass-Spring with deformable triangulation for collision
			DataStructures::Location locationTriangle1(localCoordinateTriangle1, Location::TRIANGLE);
			// The location related to the TriangleMesh can carry an ELEMENT information
			// e.g. part of an Fem2D for example
			locationTriangle1.elementMeshLocalCoordinate = locationTriangle1.triangleMeshLocalCoordinate;
			// The location related to the TriangleMesh can carry a RIGID LOCAL POSITION information
			// e.g. part of a rigid body
			locationTriangle1.rigidLocalPosition = pose2AtTime1.inverse() * T1;

			Math::Vector triangle2BaryCoord(3);
			triangle2BaryCoord << 1.0 - triangle2Alpha - triangle2Beta, triangle2Alpha, triangle2Beta;
			DataStructures::IndexedLocalCoordinate localCoordinateTriangle2(triangleId, triangle2BaryCoord);
			// The location related to the TriangleMesh can carry a TRIANGLE information
			// e.g. part of a Mass-Spring with deformable triangulation for collision
			DataStructures::Location locationTriangle2(localCoordinateTriangle2, Location::TRIANGLE);
			// The location related to the TriangleMesh can carry an ELEMENT information
			// e.g. part of an Fem2D for example
			locationTriangle2.elementMeshLocalCoordinate = locationTriangle2.triangleMeshLocalCoordinate;
			// The location related to the TriangleMesh can carry a RIGID LOCAL POSITION information
			// e.g. part of a rigid body
			locationTriangle2.rigidLocalPosition = pose2AtTime1.inverse() * T1;

			auto contact = std::make_shared<Contact>(
				COLLISION_DETECTION_TYPE_CONTINUOUS,
				penentrationDepthAtT1,
				earliestTimeOfImpact,
				(T1 + T2) * 0.5,
				normal,
				std::make_pair(locationTriangle1, locationTriangle2));

			contacts.push_back(std::move(contact));
		}
	}

	return contacts;
}

}; // namespace Collision
}; // namespace SurgSim
