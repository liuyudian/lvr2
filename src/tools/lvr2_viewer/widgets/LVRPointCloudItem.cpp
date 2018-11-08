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
 * LVRPointCloudItem.cpp
 *
 *  @date Feb 7, 2014
 *  @author Thomas Wiemann
 */
#include "LVRPointCloudItem.hpp"
#include "LVRItemTypes.hpp"

namespace lvr2
{

LVRPointCloudItem::LVRPointCloudItem(PointBufferBridgePtr& ptr, QTreeWidgetItem* item) :
       QTreeWidgetItem(item, LVRPointCloudItemType), m_parent(item), m_pointBridge(ptr)
{
    // Setup tree widget icon
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/qv_pc_tree_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    setIcon(0, icon);
    setText(0, "Point Cloud");
    setExpanded(true);

    // Setup Infos
    QTreeWidgetItem* numItem = new QTreeWidgetItem(this);
    QString num;
    numItem->setText(0, "Num Points:");
    numItem->setText(1, num.setNum(ptr->getNumPoints()));
    addChild(numItem);

    QTreeWidgetItem* normalItem = new QTreeWidgetItem(this);
    normalItem->setText(0, "Has Normals:");
    if(ptr->hasNormals())
    {
        normalItem->setText(1, "yes");
    }
    else
    {
        normalItem->setText(1, "no");
    }
    addChild(normalItem);

    QTreeWidgetItem* colorItem = new QTreeWidgetItem(this);
    colorItem->setText(0, "Has Colors:");
    if(ptr->hasColors())
    {
        colorItem->setText(1, "yes");
    }
    else
    {
        colorItem->setText(1, "no");
    }
    addChild(colorItem);

    QTreeWidgetItem* specItem = new QTreeWidgetItem(this);
    specItem->setText(0, "Has Spectraldata:");
    if(ptr->getPointBuffer()->getFloatChannel("spectral_channels"))
    {
        specItem->setText(1, "yes");
    }
    else
    {
        specItem->setText(1, "no");
    }
    addChild(specItem);

    // set initial values
    m_opacity = 1;
    m_pointSize = 1;
    m_color = QColor(255,255,255);
    m_visible = true;
}

QColor LVRPointCloudItem::getColor()
{
	return m_color;
}

void LVRPointCloudItem::setColor(QColor &c)
{
    m_pointBridge->setBaseColor(c.redF(), c.greenF(), c.blueF());
    m_color = c;
}

void LVRPointCloudItem::setSelectionColor(QColor& c)
{
    m_pointBridge->setBaseColor(c.redF(), c.greenF(), c.blueF());
}

void LVRPointCloudItem::resetColor()
{
    m_pointBridge->setBaseColor(m_color.redF(), m_color.greenF(), m_color.blueF());
}

int LVRPointCloudItem::getPointSize()
{
	return m_pointSize;
}

void LVRPointCloudItem::setPointSize(int &pointSize)
{
    m_pointBridge->setPointSize(pointSize);
    m_pointSize = pointSize;
}

float LVRPointCloudItem::getOpacity()
{
	return m_opacity;
}

void LVRPointCloudItem::setOpacity(float &opacity)
{
    m_pointBridge->setOpacity(opacity);
    m_opacity = opacity;
}

bool LVRPointCloudItem::getVisibility()
{
	return m_visible;
}

void LVRPointCloudItem::setVisibility(bool &visibility)
{
	m_pointBridge->setVisibility(visibility);
	m_visible = visibility;
}

size_t LVRPointCloudItem::getNumPoints()
{
    return m_pointBridge->getNumPoints();
}

PointBufferPtr LVRPointCloudItem::getPointBuffer()
{
    return m_pointBridge->getPointBuffer();
}

PointBufferBridgePtr LVRPointCloudItem::getPointBufferBridge()
{
    return m_pointBridge;
}

vtkSmartPointer<vtkActor> LVRPointCloudItem::getActor()
{
	return m_pointBridge->getPointCloudActor();
}

LVRPointCloudItem::~LVRPointCloudItem()
{
    // TODO Auto-generated destructor stub
}

} /* namespace lvr2 */
