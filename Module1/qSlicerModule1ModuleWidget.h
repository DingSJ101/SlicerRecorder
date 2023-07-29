/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSlicerModule1ModuleWidget_h
#define __qSlicerModule1ModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerModule1ModuleExport.h"

#include <recordingthread.h>
#include <mytask.h>
#include <QTime>
#include <QDir>

class qSlicerModule1ModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_MODULE1_EXPORT qSlicerModule1ModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerModule1ModuleWidget(QWidget *parent=0);
  virtual ~qSlicerModule1ModuleWidget();

public slots:
    void on_pushButton_clicked();
    // get info  and append it to textBrowser
    void echoInfo(QString info);
    void startRecording();
    void finishRecording();
    void reproduceRecording();
    void on_screen_shot();
    void stopReproduce();
    void continueReproduce();


protected:
  QScopedPointer<qSlicerModule1ModuleWidgetPrivate> d_ptr;
  // vtkMRMLNode* m_EventNode=nullptr;
  void setup() override;
  void enter() override;
  void exit() override;

Q_SIGNALS:
    void createRecordingThreads(const int );

private:
  Q_DECLARE_PRIVATE(qSlicerModule1ModuleWidget);
  Q_DISABLE_COPY(qSlicerModule1ModuleWidget);
  QThread *m_thread;
  myTask* m_task = nullptr; 
  QString ScreenShotDir="D:\\SlicerScreenShot\\";
};

#endif

