/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerModule1FooBarWidget.h"
#include "ui_qSlicerModule1FooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Module1
class qSlicerModule1FooBarWidgetPrivate
  : public Ui_qSlicerModule1FooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerModule1FooBarWidget);
protected:
  qSlicerModule1FooBarWidget* const q_ptr;

public:
  qSlicerModule1FooBarWidgetPrivate(
    qSlicerModule1FooBarWidget& object);
  virtual void setupUi(qSlicerModule1FooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerModule1FooBarWidgetPrivate
::qSlicerModule1FooBarWidgetPrivate(
  qSlicerModule1FooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerModule1FooBarWidgetPrivate
::setupUi(qSlicerModule1FooBarWidget* widget)
{
  this->Ui_qSlicerModule1FooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerModule1FooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerModule1FooBarWidget
::qSlicerModule1FooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerModule1FooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerModule1FooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerModule1FooBarWidget
::~qSlicerModule1FooBarWidget()
{
}
