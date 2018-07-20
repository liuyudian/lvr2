#ifndef SPECTRALDIALOG_H_
#define SPECTRALDIALOG_H_

#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkCameraRepresentation.h>
#include <vtkCameraInterpolator.h>
#include <vtkCommand.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkWindowToImageFilter.h>


#include <lvr/io/PointBuffer.hpp>
#include <lvr/io/ModelFactory.hpp>
#include <lvr/io/Model.hpp>
#include <lvr/io/DataStruct.hpp>

#include <lvr/registration/ICPPointAlign.hpp>
//#include <lvr/io/boost/shared_ptr.hpp>

#include "../vtkBridge/LVRPointBufferBridge.hpp"

#include <QtWidgets>
#include<QDialog>
#include "ui_LVRSpectralSettings.h"
#include "LVRRecordedFrameItem.hpp"

using Ui::SpectralDialog;

namespace lvr
{

class LVRSpectralDialog : public QObject
{
    Q_OBJECT

public:
    LVRSpectralDialog(QTreeWidget* treeWidget, QMainWindow* mainWindow, PointBufferBridgePtr points);
    virtual ~LVRSpectralDialog();

public Q_SLOTS:
    void valueChangeFinished();
    
private:
    void connectSignalsAndSlots();

    QMainWindow*         m_mainWindow;
    SpectralDialog*      m_spectralDialog;
    QDialog*             m_dialog;
    PointBufferBridgePtr m_points;
    unsigned char        m_r, m_g, m_b;
};

} // namespace lvr

#endif /* SPECTRALDIALOG_H_ */