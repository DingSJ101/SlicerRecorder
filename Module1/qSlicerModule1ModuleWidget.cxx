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

// Qt includes
#include <QDebug>

// Slicer includes
#include "qSlicerModule1ModuleWidget.h"
#include "ui_qSlicerModule1ModuleWidget.h"

// #include "vtkMRMLScene.h"
// #include "vtkMRMLColorNode.h"

#include <QProcess>
#include <QDebug>

//#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QByteArray>
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerModule1ModuleWidgetPrivate: public Ui_qSlicerModule1ModuleWidget
{
public:
  qSlicerModule1ModuleWidgetPrivate();
private slots:

};

//-----------------------------------------------------------------------------
// qSlicerModule1ModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerModule1ModuleWidgetPrivate::qSlicerModule1ModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerModule1ModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerModule1ModuleWidget::qSlicerModule1ModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerModule1ModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerModule1ModuleWidget::~qSlicerModule1ModuleWidget()
{
}

//----------------------------------------------------------------------------
#include <qSlicerSingletonViewFactory.h>
#include <qSlicerWidget.h>
#include <qSlicerApplication.h>
#include <qSlicerAbstractModule.h>
#include <qSlicerLayoutManager.h>
#include <QWidget>
#include <QShortcut>
void qSlicerModule1ModuleWidget::setup()
{
  Q_D(qSlicerModule1ModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
  
  // // ---------------------setStyleSheet---------------------
  // QFile file(":/style.qss");
  // file.open(QFile::ReadOnly);
  // if (file.isOpen())
  // {
  //       QByteArray message = file.readAll();
  //       qDebug()<<message;
  //       this->setStyleSheet(message);
  //       file.close();
  // }
  // //---------------------setStyleSheet---------------------
  new QShortcut(QKeySequence("F11"), this, SLOT(on_screen_shot()));
  new QShortcut(QKeySequence("F5"), this, SLOT(stopReproduce()));
  new QShortcut(QKeySequence("F6"), this, SLOT(continueReproduce()));
  // new QShortcut(QKeySequence("Ctrl+Shift+Q"), this, &qSlicerModule1ModuleWidget::on_screen_shot);
  
  echoInfo(QString("[in] qSlicerModuleWidget::setup()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
  qDebug()<<"qSlicerModuleWidget::setup()"<<this->thread()->currentThreadId();
  m_task =new myTask;
  qDebug()<<"myTask:"<<m_task->thread()->currentThreadId();
  m_thread = new QThread(this);
  m_task->moveToThread(m_thread);

  connect(d->startButton,&QPushButton::clicked,this,&qSlicerModule1ModuleWidget::startRecording);
  connect(d->stopButton,&QPushButton::clicked,this,&qSlicerModule1ModuleWidget::finishRecording);
  connect(d->reproduceButton,&QPushButton::clicked,this,&qSlicerModule1ModuleWidget::reproduceRecording);

  connect(this,&qSlicerModule1ModuleWidget::createRecordingThreads,m_task,&myTask::initialize);
  
  connect(m_task,&myTask::echoInfoImp,this,&qSlicerModule1ModuleWidget::echoInfo);
  connect(m_task,&myTask::finished,m_thread,&QThread::quit);

  m_thread->start();
  echoInfo("[out] qSlicerModuleWidget::setup()");
}

void qSlicerModule1ModuleWidget::enter(){}

void qSlicerModule1ModuleWidget::exit(){}

void qSlicerModule1ModuleWidget::on_screen_shot()
{
  Q_D(qSlicerModule1ModuleWidget);
  // 获取日期时间
  QDateTime current_date_time =QDateTime::currentDateTime();
  QString current_date =current_date_time.toString("yyyy-MM-dd_hh-mm-ss");
  QString fileName = ScreenShotDir+current_date + ".png";
  // 如果ScreenShotDir不存在，则创建
  // QDir dir;
  // if (!dir.exists(ScreenShotDir))    dir.mkpath(ScreenShotDir);
  CaptureScreenAndSave(fileName.toStdString().c_str());
  d->textBrowser->append("save screenshot at :"+fileName);
  qDebug()<<"save screenshot at :"<<fileName;
}

void qSlicerModule1ModuleWidget::stopReproduce(){
    setDeltaTime(getDeltaTime()-QDateTime::currentMSecsSinceEpoch());
}
void qSlicerModule1ModuleWidget::continueReproduce(){
    setDeltaTime(getDeltaTime()+QDateTime::currentMSecsSinceEpoch());
}

void qSlicerModule1ModuleWidget::on_pushButton_clicked()
{
   QProcess *myProcess = new QProcess(this);
//    myProcess->start();
   qDebug()  <<"on_pushButton_clicked()";
   Q_D(qSlicerModule1ModuleWidget);
   QString word = d->lineEdit->text();
   qDebug() <<word;
   myProcess->start(word);
}

void qSlicerModule1ModuleWidget::startRecording()
{
  echoInfo(QString("[in] qSlicerModuleWidget::startRecoding()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
  Q_D(qSlicerModule1ModuleWidget);
  if (m_task == nullptr)
  {
    qDebug() <<"Error : m_task == nullptr";
  }
  emit createRecordingThreads(0);
  echoInfo("[out] qSlicerModuleWidget::startRecoding()");
}

void qSlicerModule1ModuleWidget::finishRecording()
{
    echoInfo(QString("[in] qSlicerModuleWidget::finishRecording()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    echoInfo("  [call] m_task->finish();");
    m_task->finish();
    echoInfo("[out] qSlicerModuleWidget::finishRecording()");
}

void qSlicerModule1ModuleWidget::reproduceRecording()
{
    echoInfo(QString("[in] qSlicerModuleWidget::reproduceRecording()")+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId())));
    echoInfo("  [call] m_task->reproduce();");
    m_task->reproduce();
    echoInfo("[out] qSlicerModuleWidget::reproduceRecording()");
}

void qSlicerModule1ModuleWidget::echoInfo(QString info)
{
  Q_D(qSlicerModule1ModuleWidget);
  // qDebug()<<"qSlicerModuleWidget::echoInfo("<<info<<")";
  d->textBrowser->append(info);
}

