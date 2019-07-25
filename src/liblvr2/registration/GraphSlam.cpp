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
 * GraphSlam.cpp
 *
 *  @date July 22, 2019
 *  @author Malte Hillmann
 */
#include <lvr2/registration/GraphSlam.hpp>

#include <lvr2/registration/KDTree.hpp>
#include <lvr2/io/IOUtils.hpp>

#include <limits>
#include <Eigen/SparseCholesky>

using namespace std;
using namespace Eigen;

namespace lvr2
{

void EulerToMatrix4(const Vector3f& pos, const Vector3f& theta, Matrix4f& mat);
void Matrix4ToEuler(const Matrix4f mat, Vector3f& rPosTheta, Vector3f& rPos);

GraphSlam::GraphSlam(const SlamOptions* options)
    : m_options(options)
{
}

void GraphSlam::addEdge(int start, int end)
{
    m_graph.push_back(make_pair(start, end));
}

void GraphSlam::doGraphSlam(vector<ScanPtr>& scans, int last)
{
    // ignore first scan, keep last scan => n = last - 1 + 1
    int n = last;

    GraphMatrix A(6 * n, 6 * n);
    GraphVector B(6 * n);
    GraphVector X(6 * n);

    for (int iteration = 0;
            iteration < m_options->slamIterations;
            iteration++)
    {
        cout << "GraphSLAM Iteration " << iteration << " of " << m_options->slamIterations << endl;

        // scoped to garbage collect the Matrix
        // Construct the linear equation system A * X = B..
        fillEquation(scans, A, B);

        X = SimplicialCholesky<GraphMatrix>().compute(A).solve(B);

        double sum_position_diff = 0.0;

        // Start with second Scan
        #pragma omp parallel for reduction(+:sum_position_diff)
        for (int i = 1; i <= last; i++)
        {
            ScanPtr& scan = scans[i];

            // Now update the Poses
            Matrix6f Ha = Matrix6f::Identity();

            Matrix4f initialPose = scan->getPose();
            Vector3f pos, theta;
            Matrix4ToEuler(initialPose, theta, pos);
            if (m_options->verbose)
            {
                cout << "Start of " << i << ": " << pos.transpose() << ", " << theta.transpose() << endl;
            }

            float ctx, stx, cty, sty;
            sincosf(theta.x(), &stx, &ctx);
            sincosf(theta.y(), &sty, &cty);

            // Fill Ha
            Ha(0, 4) = -pos.z() * ctx + pos.y() * stx;
            Ha(0, 5) = pos.y() * cty * ctx + pos.z() * stx * cty;

            Ha(1, 3) = pos.z();
            Ha(1, 4) = -pos.x() * stx;
            Ha(1, 5) = -pos.x() * ctx * cty + pos.z() * sty;


            Ha(2, 3) = -pos.y();
            Ha(2, 4) = pos.x() * ctx;
            Ha(2, 5) = -pos.x() * cty * stx - pos.y() * sty;

            Ha(3, 5) = sty;

            Ha(4, 4) = stx;
            Ha(4, 5) = ctx * cty;

            Ha(5, 4) = ctx;
            Ha(5, 5) = -stx * cty;

            // Correct pose estimate
            Vector6f result = Ha.inverse() * X.block<6, 1>((i - 1) * 6, 0);

            // Update the Pose
            pos -= result.block<3, 1>(0, 0);
            theta -= result.block<3, 1>(3, 0);
            Matrix4f transform;
            EulerToMatrix4(pos, theta, transform);

            if (m_options->verbose)
            {
                cout << "End: " << pos.transpose() << ", " << theta.transpose() << endl;
            }

            transform = transform * initialPose.inverse();

            scan->transform(transform, true, ScanUse::GRAPHSLAM);

            sum_position_diff += result.block<3, 1>(0, 0).norm();
        }

        // add Frames to unused Scans
        scans[0]->addFrame(ScanUse::GRAPHSLAM);
        for (size_t i = last + 1; i < scans.size(); i++)
        {
            scans[i]->addFrame(ScanUse::INVALID);
        }

        cout << "Sum of Position differences = " << sum_position_diff << endl;

        double avg = sum_position_diff / n;
        if (avg < m_options->slamEpsilon)
        {
            break;
        }
    }
}

/**
 * A function to fill the linear system mat * x = vec.
 */
void GraphSlam::fillEquation(const vector<ScanPtr>& scans, GraphMatrix& mat, GraphVector& vec)
{
    mat.setZero();
    vec.setZero();

    map<pair<int, int>, Matrix6f> result;

    #pragma omp parallel for
    for (size_t i = 0; i < m_graph.size(); i++)
    {
        int a, b;
        std::tie(a, b) = m_graph[i];

        ScanPtr firstScan  = scans[a];
        ScanPtr secondScan = scans[b];

        Matrix6f coeffMat;
        Vector6f coeffVec;
        eulerCovariance(firstScan, secondScan, coeffMat, coeffVec);

        #pragma omp critical
        {
            // first scan is not part of Matrix => ignore any a or b of 0

            int offsetA = (a - 1) * 6;
            int offsetB = (b - 1) * 6;

            if (offsetA >= 0)
            {
                vec.block<6, 1>(offsetA, 0) += coeffVec;
                auto key = make_pair(offsetA, offsetA);
                if (result.find(key) == result.end())
                {
                    result.insert(make_pair(key, Matrix6f::Zero()));
                }
                result[key] += coeffMat;
            }
            if (offsetB >= 0)
            {
                vec.block<6, 1>(offsetB, 0) -= coeffVec;
                auto key = make_pair(offsetB, offsetB);
                if (result.find(key) == result.end())
                {
                    result.insert(make_pair(key, Matrix6f::Zero()));
                }
                result[key] += coeffMat;
            }
            if (offsetA >= 0 && offsetB >= 0)
            {
                auto key = make_pair(offsetA, offsetB);
                auto key2 = make_pair(offsetB, offsetA);
                if (result.find(key) == result.end())
                {
                    result.insert(make_pair(key, Matrix6f::Zero()));
                }
                if (result.find(key2) == result.end())
                {
                    result.insert(make_pair(key2, Matrix6f::Zero()));
                }
                result[key] -= coeffMat;
                result[key2] -= coeffMat;
            }
        }
    }

    mat.reserve(result.size() * 6 * 6);
    int x, y;
    for (auto& e : result)
    {
        tie(x, y) = e.first;
        Matrix6f& m = e.second;
        for (int dx = 0; dx < 6; dx++)
        {
            for (int dy = 0; dy < 6; dy++)
            {
                mat.insert(x + dx, y + dy) = m(dx, dy);
            }
        }
    }
}

void GraphSlam::eulerCovariance(ScanPtr a, ScanPtr b, Matrix6f& outMat, Vector6f& outVec) const
{
    size_t n = b->count();

    auto tree = KDTree::create(a->points(), a->count(), m_options->maxLeafSize);
    Vector3f* points = b->points();
    Vector3f** results = new Vector3f*[n];
    size_t pairs = getNearestNeighbors(tree, points, results, n, m_options->slamMaxDistance);

    Vector6f mz = Vector6f::Zero();
    Vector3f sum;
    float xy, yz, xz, ypz, xpz, xpy;
    sum = Vector3f::Zero();
    xy = yz = xz = ypz = xpz = xpy = 0.0f;

    for (size_t i = 0; i < n; i++)
    {
        if (results[i] == nullptr)
        {
            continue;
        }
        Vector3f mid = (points[i] + *results[i]) / 2.0f;
        Vector3f d = *results[i] - points[i];

        float x = mid.x(), y = mid.y(), z = mid.z();

        sum += mid;

        xpy += x * x + y * y;
        xpz += x * x + z * z;
        ypz += y * y + z * z;

        xy += x * y;
        xz += x * z;
        yz += y * z;

        mz.block<3, 1>(0, 0) += d;

        mz(3) += -z * d.y() + y * d.z();
        mz(4) += -y * d.x() + x * d.y();
        mz(5) += z * d.x() - x * d.z();
    }

    Matrix6f mm = Matrix6f::Zero();
    mm(0, 0) = mm(1, 1) = mm(2, 2) = pairs;
    mm(3, 3) = ypz;
    mm(4, 4) = xpy;
    mm(5, 5) = xpz;

    mm(0, 4) = mm(4, 0) = -sum.y();
    mm(0, 5) = mm(5, 0) = sum.z();
    mm(1, 3) = mm(3, 1) = -sum.z();
    mm(1, 4) = mm(4, 1) = sum.x();
    mm(2, 3) = mm(3, 2) = sum.y();
    mm(2, 5) = mm(5, 2) = -sum.x();

    mm(3, 4) = mm(4, 3) = -xz;
    mm(3, 5) = mm(5, 3) = -xy;
    mm(4, 5) = mm(5, 4) = -yz;

    Vector6f d = mm.inverse() * mz;

    float ss = 0.0f;

    for (size_t i = 0; i < n; i++)
    {
        if (results[i] == nullptr)
        {
            continue;
        }

        Vector3f mid = (points[i] + *results[i]) / 2.0f;
        Vector3f delta = *results[i] - points[i];

        ss += powf(delta.x() + (d(0) - mid.y() * d(4) + mid.z() * d(5)), 2.0f)
              + powf(delta.y() + (d(1) - mid.z() * d(3) + mid.x() * d(4)), 2.0f)
              + powf(delta.z() + (d(2) + mid.y() * d(3) - mid.x() * d(5)), 2.0f);
    }

    delete results;

    ss = ss / (2.0f * pairs - 3.0f);

    ss = 1.0f / ss;

    outMat = mm * ss;
    outVec = mz * ss;
}

void EulerToMatrix4(const Vector3f& pos,
                    const Vector3f& theta,
                    Matrix4f& mat)
{
    float sx = sinf(theta[0]);
    float cx = cosf(theta[0]);
    float sy = sinf(theta[1]);
    float cy = cosf(theta[1]);
    float sz = sinf(theta[2]);
    float cz = cosf(theta[2]);

    mat << cy* cz,
        sx* sy* cz + cx* sz,
        -cx* sy* cz + sx* sz,
        0.0,
        -cy* sz,
        -sx* sy* sz + cx* cz,
        cx* sy* sz + sx* cz,
        0.0,
        sy,
        -sx* cy,
        cx* cy,

        0.0,

        pos[0],
        pos[1],
        pos[2],
        1;

    mat.transposeInPlace();
}

void Matrix4ToEuler(const Matrix4f inputMat,
                    Vector3f& rPosTheta,
                    Vector3f& rPos)
{
    Matrix4f mat = inputMat.transpose();

    float _trX, _trY;

    // Calculate Y-axis angle
    rPosTheta[1] = asinf(max(min(mat(2, 0), 1.0f), -1.0f)); // asin returns nan for any number outside [-1, 1]
    if (mat(0, 0) <= 0.0)
    {
        rPosTheta[1] = M_PI - rPosTheta[1];
    }

    float  C    =  cosf( rPosTheta[1] );
    if ( fabsf( C ) > 0.005 )                    // Gimbal lock?
    {
        _trX      =  mat(2, 2) / C;             // No, so get X-axis angle
        _trY      =  -mat(2, 1) / C;
        rPosTheta[0]  = atan2f( _trY, _trX );
        _trX      =  mat(0, 0) / C;              // Get Z-axis angle
        _trY      = -mat(1, 0) / C;
        rPosTheta[2]  = atan2f( _trY, _trX );
    }
    else                                        // Gimbal lock has occurred
    {
        rPosTheta[0] = 0.0;                       // Set X-axis angle to zero
        _trX      =  mat(1, 1);  //1                // And calculate Z-axis angle
        _trY      =  mat(0, 1);  //2
        rPosTheta[2]  = atan2f( _trY, _trX );
    }

    rPos = inputMat.block<3, 1>(0, 3);
}

} /* namespace lvr2 */
