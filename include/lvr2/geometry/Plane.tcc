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
 * Plane.tcc
 *
 *  @date 17.07.2017
 *  @author @author Johan M. von Behren <johan@vonbehren.eu>
 */

#include "Plane.hpp"

namespace lvr2
{

template<typename BaseVecT>
float Plane<BaseVecT>::distance(const Point<BaseVecT>& other) const
{
    // Distance betweeen plane and query point (calculated by hesse normal form)
    // Credits: https://oberprima.com/mathematik/abstand-berechnen/
    return (other - this->pos).dot(this->normal.asVector());
}

template<typename BaseVecT>
Point<BaseVecT> Plane<BaseVecT>::project(const Point<BaseVecT>& other) const
{
    // Credits to: https://stackoverflow.com/questions/9605556/#answer-41897378
    return other - (this->normal.asVector() * (this->normal.dot(other - this->pos)));
}

} // namespace lvr2