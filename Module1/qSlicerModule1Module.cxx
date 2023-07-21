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

// Module1 Logic includes
#include <vtkSlicerModule1Logic.h>

// Module1 includes
#include "qSlicerModule1Module.h"
#include "qSlicerModule1ModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerModule1ModulePrivate
{
public:
  qSlicerModule1ModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerModule1ModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerModule1ModulePrivate::qSlicerModule1ModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerModule1Module methods

//-----------------------------------------------------------------------------
qSlicerModule1Module::qSlicerModule1Module(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerModule1ModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerModule1Module::~qSlicerModule1Module()
{
}

//-----------------------------------------------------------------------------
QString qSlicerModule1Module::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerModule1Module::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerModule1Module::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerModule1Module::icon() const
{
  return QIcon(":/Icons/Module1.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerModule1Module::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerModule1Module::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
#include <QDebug>
#include <qSlicerApplication.h>
#include <qMRMLSliceWidget.h>
#include <vtkMRMLLayoutNode.h>
#include <vtkMRMLLayoutLogic.h>
#include <qSlicerLayoutManager.h>
#include <QFile>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qSlicerSingletonViewFactory.h>
#include <qlabel.h>
#include <qSlicerMainWindow.h>
#include <qwindow.h>
void qSlicerModule1Module::setup()
{
    Q_D(qSlicerModule1Module);
    this->Superclass::setup();

    qSlicerApplication* app = qSlicerApplication::application();
    QMainWindow* mw = app->mainWindow();
    mw->topLevelWidget()->setWindowTitle("123123123");
    mw->topLevelWidget()->findChildren<QWidget*>();
    for(auto w : mw->topLevelWidget()->findChildren<QWidget*>()){
        if(w->objectName()=="pythonConsole"){
            qDebug()<<"find pythonConsole";
            w->setVisible(false);
        }
    }

    
    // mywidget->setMRMLScene(app->mrmlScene());


    // change the mainwaindow layout 
    // app->layoutManager()->setLayout(vtkMRMLLayoutNode::SlicerLayoutConventionalView);
    // qDebug()<<"SlicerLayoutConventionalView";
    // app->layoutManager()->layoutLogic()->GetLayoutNode()->SetViewArrangement( vtkMRMLLayoutNode::SlicerLayoutDual3DView);
    // qDebug()<<"SlicerLayoutDual3DView";
    
    // 读入ui文件，并将其设置为主窗口的中央控件
    QWidget *myWidget = new qSlicerModule1ModuleWidget();
    qSlicerSingletonViewFactory *myviewfactory = new qSlicerSingletonViewFactory();
    myviewfactory->setWidget(new QLabel("123123123"));
    myviewfactory->setTagName("loginWidget");
    app->layoutManager()->registerViewFactory(myviewfactory);
    auto mylayoutnode = app->layoutManager()->layoutLogic()->GetLayoutNode();
    auto mylayout = "<layout type=\"horizontal\"><item><loginWidget></loginWidget></item></layout>" ;
    mylayoutnode->AddLayoutDescription(1234,mylayout);
    app->layoutManager()->setLayout(1234);

}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerModule1Module
::createWidgetRepresentation()
{
  return new qSlicerModule1ModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerModule1Module::createLogic()
{
  return vtkSlicerModule1Logic::New();
}
