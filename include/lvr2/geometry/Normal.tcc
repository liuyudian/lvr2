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
 * Vector.tcc
 *
 *  @date 03.06.2017
 *  @author Lukas Kalbertodt <lukas.kalbertodt@gmail.com>
 */

#include "Vector.hpp"

#include <lvr2/util/Panic.hpp>


namespace lvr2
{

template <typename BaseVecT>
Normal<BaseVecT>::Normal(BaseVecT base)
    : Vector<BaseVecT>(base)
{
    this->normalize();
}

template <typename BaseVecT>
Normal<BaseVecT>::Normal(Vector<BaseVecT> vec)
    : Vector<BaseVecT>(vec)
{
    this->normalize();
}

template <typename BaseVecT>
Normal<BaseVecT>::Normal(
    typename BaseVecT::CoordType x,
    typename BaseVecT::CoordType y,
    typename BaseVecT::CoordType z
)
    : Vector<BaseVecT>(x, y, z)
{
    this->normalize();
}

template<typename BaseVecT>
template<typename CollectionT>
Normal<BaseVecT> Normal<BaseVecT>::average(const CollectionT& normals)
{
    if (normals.empty())
    {
        panic("average() of 0 normals");
    }

    Vector<BaseVecT> acc(0, 0, 0);
    for (auto n: normals)
    {
        static_assert(
            std::is_same<typeof(n), Normal<BaseVecT>>::value,
            "Collection has to contain Vectors"
        );
        acc += n.asVector();
    }
    return acc.normalized();
}

template <typename BaseVecT>
Vector<BaseVecT> Normal<BaseVecT>::asVector() const
{
    return static_cast<BaseVecT>(*this);
}

template <typename BaseVecT>
Normal<BaseVecT> Normal<BaseVecT>::operator-() const
{
    return Normal(-this->x, -this->y, -this->z);
}

} // namespace lvr2