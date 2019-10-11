/**
 * Copyright (c) 2018, University Osnabrück
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University Osnabrück nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL University Osnabrück BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * TreeUtils.hpp
 *
 *  @date May 16, 2019
 *  @author Malte Hillmann
 */
#ifndef TREEUTILS_HPP_
#define TREEUTILS_HPP_

#include <lvr2/types/MatrixTypes.hpp>

namespace lvr2
{

/**
 * @brief Sorts a Point array so that all Points smaller than splitValue are on the left
 *
 * Uses the QuickSort Pivot step
 *
 * @param points     The Point array
 * @param n          The number of Points
 * @param axis       The axis to sort by
 * @param splitValue The value to sort by
 *
 * @return int The number of smaller elements. points + this value gives the start of the greater elements
 */
int splitPoints(Vector3f* points, int n, int axis, double splitValue);

/**
 * @brief Reduces a Point Cloud using an Octree with a minimum Voxel size
 *
 * @param points      The Point Cloud
 * @param n           The number of Points in the Point Cloud
 * @param voxelSize   The minimum size of a Voxel
 * @param maxLeafSize When to stop subdividing Voxels
 *
 * @return int the new number of Points in the Point Cloud
 */
int octreeReduce(Vector3f* points, int n, double voxelSize, int maxLeafSize);


/**
 * @brief A struct to calculate the Axis Aligned Bounding Box and Average Point of a Point Cloud
 */
class AABB
{
    Vector3d m_min;
    Vector3d m_max;
    Vector3d m_sum;
    size_t m_count;

public:
    AABB();

    template<typename T>
    AABB(const Vector3<T>* points, size_t count)
        : AABB()
    {
        for (size_t i = 0; i < count; i++)
        {
            addPoint(points[i]);
        }
    }

    /// Returns the "lower left" Corner of the Bounding Box, as in the smallest x, y, z of the Point Cloud.
    const Vector3d& min() const;

    /// Returns the "upper right" Corner of the Bounding Box, as in the largest x, y, z of the Point Cloud.
    const Vector3d& max() const;

    /// Returns the average of all the Points in the Point Cloud.
    Vector3d avg() const;

    /// Returns the number of Points in the Point Cloud
    size_t count() const;

    /// adds a Point to the Point Cloud
    template<typename T>
    void addPoint(const Vector3<T>& point)
    {
        for (int axis = 0; axis < 3; axis++)
        {
            double val = point(axis);
            if (val < m_min(axis))
            {
                m_min(axis) = val;
            }
            if (val > m_max(axis))
            {
                m_max(axis) = val;
            }
            m_sum(axis) += val;
        }
        m_count++;
    }

    /// Calculates the size of the Bounding Box along a certain axis
    double difference(int axis) const;

    /// Calculates the axis that has the largest size of the Bounding Box
    int longestAxis() const;
};

} /* namespace lvr2 */

#endif /* TREEUTILS_HPP_ */
