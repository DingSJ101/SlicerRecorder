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

#ifndef __qSlicerModule1FooBarWidget_h
#define __qSlicerModule1FooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerModule1ModuleWidgetsExport.h"

class qSlicerModule1FooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_Module1
class Q_SLICER_MODULE_MODULE1_WIDGETS_EXPORT qSlicerModule1FooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerModule1FooBarWidget(QWidget *parent=0);
  ~qSlicerModule1FooBarWidget() override;

protected slots:

protected:
  QScopedPointer<qSlicerModule1FooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerModule1FooBarWidget);
  Q_DISABLE_COPY(qSlicerModule1FooBarWidget);
};

#endif
