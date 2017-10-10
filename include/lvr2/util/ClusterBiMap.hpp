/* Copyright (C) 2011 Uni Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */


/*
 * ClusterBiMap.hpp
 *
 *  @date 17.06.2017
 *  @author Johan M. von Behren <johan@vonbehren.eu>
 */

#ifndef LVR2_UTIL_CLUSTERBIMAP_H_
#define LVR2_UTIL_CLUSTERBIMAP_H_

#include <lvr2/geometry/Handles.hpp>
#include <lvr2/util/Cluster.hpp>

#include <lvr2/attrmaps/AttrMaps.hpp>
#include <lvr2/attrmaps/StableVector.hpp>

namespace lvr2
{

/**
 * @brief Iterator over cluster handles in this cluster map
 *
 * Important: This is NOT a fail fast iterator. If the cluster map is changed while using an instance of this
 * iterator the behavior is undefined!
 */
 template<typename HandleT>
class ClusterBiMapIterator
{
public:
    ClusterBiMapIterator(StableVectorIterator<ClusterHandle, Cluster<HandleT>> iterator) : m_iterator(iterator) {};
    ClusterBiMapIterator& operator++();
    bool operator==(const ClusterBiMapIterator& other) const;
    bool operator!=(const ClusterBiMapIterator& other) const;
    ClusterHandle operator*() const;

private:
    StableVectorIterator<ClusterHandle, Cluster<HandleT>> m_iterator;
};

/**
 * @brief A map of clusters, which also saves a back-reference from handle to cluster.
 * @tparam Type of handles in the cluster map.
 */
template<typename HandleT>
class ClusterBiMap
{
public:
    ClusterBiMap() : m_numHandles(0) {};

    /// Creates a cluster and returns its handle.
    ClusterHandle createCluster();

    /**
     * @brief Removes the cluster behind the given handle.
     *
     * This method does a clean up in the back-reference collection from handle to cluster such that all
     * handle -> cluster connections to the removed cluster are also removed.
     *
     * Important: If the given handle does not exist, the behavior of this method is undefined!
     */
    void removeCluster(ClusterHandle clusterHandle);

    /**
     * @brief Adds the given handle to the cluster behind the given cluster handle.
     *
     * This method creates a back-reference from cluster -> handle.
     *
     * Important: If the given handle or cluster handle does not exist, the behavior of this method is undefined!
     */
    ClusterHandle addToCluster(ClusterHandle clusterHandle, HandleT handle);

    /**
     * @brief Removes the given handle from the cluster behind the given cluster handle.
     *
     * This method cleans up the back-reference from cluster -> handle.
     *
     * Important: If the given handle or cluster handle does not exist, the behavior of this method is undefined!
     */
    ClusterHandle removeFromCluster(ClusterHandle clusterHandle, HandleT handle);

    /**
     * @brief Returns a handle to the cluster to which the given handle is referenced.
     *
     * DEPRECATED: use `getClusterOf()` instead.
     *
     * Important: If the given cluster handle does not exist, the behavior of this method is undefined!
     */
    ClusterHandle getClusterH(HandleT handle) const;

    /**
     * @brief Returns the handle of the cluster to which the given `handle`
     *        belongs to.
     *
     * @return The cluster handle or None if the given `handle` is not
     *         associated with any cluster.
     */
    OptionalClusterHandle getClusterOf(HandleT handle) const;

    /// Returns the number of cluster in this set.
    size_t numCluster() const;

    ClusterBiMapIterator<HandleT> begin() const;
    ClusterBiMapIterator<HandleT> end() const;

    /// Get cluster behind the cluster handle.
    const Cluster<HandleT>& getCluster(ClusterHandle clusterHandle) const;

    /// Request the value behind the given key
    const Cluster<HandleT>& operator[](ClusterHandle clusterHandle) const;

    /**
     * @see StableVector::reserve(size_t)
     */
    void reserve(size_t newCap);

    /// Returns the number of handles in all clusters in the set.
    size_t numHandles() const;

private:
    /// Number of handels in all clusters
    size_t m_numHandles;

    /// Clusters
    StableVector<ClusterHandle, Cluster<HandleT>> m_cluster;

    /// Map from handle -> cluster to save the back-reference for stored handles.
    DenseAttrMap<HandleT, ClusterHandle> m_clusterMap;

    /// Private helper to get cluster behind the cluster handle.
    Cluster<HandleT>& getC(ClusterHandle clusterHandle);
};

} // namespace lvr2

#include <lvr2/util/ClusterBiMap.tcc>

#endif /* LVR2_UTIL_CLUSTERBIMAP_H_ */