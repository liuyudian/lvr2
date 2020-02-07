/**
 * Copyright (c) 2020, University Osnabrück
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
 * ChunkingPipeline.hpp
 *
 * @date 24.01.2020
 * @author Marcel Wiegand
 */

#ifndef LVR2_LSR_OPTIONS_YAML_EXTENSIONS
#define LVR2_LSR_OPTIONS_YAML_EXTENSIONS

#include <yaml-cpp/yaml.h>

#include "lvr2/reconstruction/LargeScaleReconstruction.hpp"

namespace YAML
{
template<>
struct convert<lvr2::LSROptions>
{
    static Node encode(const lvr2::LSROptions& options)
    {
        Node node;

        node["bigMesh"] = options.bigMesh;
        node["debugChunks"] = options.debugChunks;
        node["useGPU"] = options.useGPU;
        node["voxelSizes"] = options.voxelSizes;
        node["bgVoxelSize"] = options.bgVoxelSize;
        node["scale"] = options.scale;
        node["chunkSize"] = options.chunkSize;
        node["nodeSize"] = options.nodeSize;
        node["partMethod"] = options.partMethod;
        node["Ki"] = options.Ki;
        node["Kd"] = options.Kd;
        node["Kn"] = options.Kn;
        node["useRansac"] = options.useRansac;
        node["extrude"] = options.extrude;
        node["removeDanglingArtifacts"] = options.removeDanglingArtifacts;
        node["cleanContours"] = options.cleanContours;
        node["fillHoles"] = options.fillHoles;
        node["optimizePlanes"] = options.optimizePlanes;
        node["getNormalThreshold"] = options.getNormalThreshold;
        node["planeIterations"] = options.planeIterations;
        node["MinPlaneSize"] = options.MinPlaneSize;
        node["SmallRegionThreshold"] = options.SmallRegionThreshold;
        node["retesselate"] = options.retesselate;
        node["LineFusionThreshold"] = options.LineFusionThreshold;

        return node;
    }

    static bool decode(const Node& node, lvr2::LSROptions& options)
    {
        if (!node.IsMap())
        {
            return false;
        }

        if (node["bigMesh"])
        {
            options.bigMesh = node["bigMesh"].as<bool>();
        }

        if (node["debugChunks"])
        {
            options.debugChunks = node["debugChunks"].as<bool>();
        }

        if (node["useGPU"])
        {
            options.useGPU = node["useGPU"].as<bool>();
        }


        if (node["voxelSizes"])
        {
            options.voxelSizes = node["voxelSizes"].as<std::vector<float>>();
        }

        if (node["bgVoxelSize"])
        {
            options.bgVoxelSize = node["bgVoxelSize"].as<float>();
        }

        if (node["scale"])
        {
            options.scale = node["scale"].as<float>();
        }

        if (node["chunkSize"])
        {
            options.chunkSize = node["chunkSize"].as<size_t>();
        }

        if (node["nodeSize"])
        {
            options.nodeSize = node["nodeSize"].as<uint>();
        }

        if (node["partMethod"])
        {
            options.partMethod = node["partMethod"].as<int>();
        }

        if (node["Ki"])
        {
            options.Ki = node["Ki"].as<int>();
        }

        if (node["Kd"])
        {
            options.Kd = node["Kd"].as<int>();
        }

        if (node["Kn"])
        {
            options.Kn = node["Kn"].as<int>();
        }

        if (node["useRansac"])
        {
            options.useRansac = node["useRansac"].as<bool>();
        }

        if (node["extrude"])
        {
            options.extrude = node["extrude"].as<bool>();
        }

        if (node["removeDanglingArtifacts"])
        {
            options.removeDanglingArtifacts = node["removeDanglingArtifacts"].as<int>();
        }

        if (node["cleanContours"])
        {
            options.cleanContours = node["cleanContours"].as<int>();
        }

        if (node["fillHoles"])
        {
            options.fillHoles = node["fillHoles"].as<int>();
        }

        if (node["optimizePlanes"])
        {
            options.optimizePlanes = node["optimizePlanes"].as<bool>();
        }

        if (node["getNormalThreshold"])
        {
            options.getNormalThreshold = node["getNormalThreshold"].as<float>();
        }

        if (node["planeIterations"])
        {
            options.planeIterations = node["planeIterations"].as<int>();
        }

        if (node["MinPlaneSize"])
        {
            options.MinPlaneSize = node["MinPlaneSize"].as<int>();
        }

        if (node["SmallRegionThreshold"])
        {
            options.SmallRegionThreshold = node["SmallRegionThreshold"].as<int>();
        }

        if (node["retesselate"])
        {
            options.retesselate = node["retesselate"].as<bool>();
        }

        if (node["LineFusionThreshold"])
        {
            options.LineFusionThreshold = node["LineFusionThreshold"].as<float>();
        }

        return true;
    }
};
}  // namespace YAML

#endif
