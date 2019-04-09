// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef COHERENTSTYLE01_H
#define COHERENTSTYLE01_H

#include <QVBoxLayout>
#include "view/nodeWidget.h"
#include "view/widgetParams.h"
#include "core/nodeHandle.h"
#include "core/nodeTexture2D.h"

// **** a widget containing a keyframed slider ****
class FractalNoiseCellularWidget : public NodeWidget {
 public:
 FractalNoiseCellularWidget(NodeInterface *node) :
  NodeWidget(node),
    _halfsize(new IntSliderWidget(node,"size",1,100,0)),
    _test(new FloatSliderWidget(node,"test",0,1,0)) {
    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(_halfsize);
    l->addWidget(_test);
    setLayout(l);
    addChildWidget(_halfsize);
    addChildWidget(_test);
  }

  inline IntSliderWidget *halfsize() const {return _halfsize;}
  inline FloatSliderWidget *test() const {return _test;}

 private:
  IntSliderWidget *_halfsize;
  FloatSliderWidget *_test;
};




// **** the node itself, containing the shader and the widget ****
class FractalNoiseCellularNode : public NodeTexture2D {
 public:
  FractalNoiseCellularNode(PbGraph *parent=NULL,NodeHandle *handle=NULL);
  ~FractalNoiseCellularNode();
  void apply();

  inline NodeWidget  *widget() {return _w;}
  inline virtual void reload() {_pBlend.reload();}
  static QString SHADER_PATH;


 protected:
  // need 1 tmp texture (because of the 2 passes)
  inline unsigned int nbTmps()  const {return 1; }
  void initFBO();
  void cleanFBO();

 private:
  void initOITData();
  void cleanOITData();

  enum BufferNames {
    COUNTER_BUFFER = 0,
    LINKED_LIST_BUFFER = 1
  };

  struct ListNode {
    Vector4f color;
    GLfloat depth;
    GLuint next;
  } ListNode;

  GPUProgram       _pBlend;
  FractalNoiseCellularWidget *_w;

  unsigned int         _nbElements;
  unsigned int         _sw;
  unsigned int         _sh;

  // order-independent transparency data
  // https://github.com/gangliao/Order-Independent-Transparency-GPU/tree/master/source%20code/src
  GLuint _acBuffer; // atomic counter buffer
  GLuint _llBuffer; // linked list buffer
  GLuint _headTex;  // head pointer texture
  GLuint _clBuffer; // clear buffer
  GLuint _maxNodes;
};




// **** the node handle, containing information displayed in the interface ****
class CoherentStyleHandle : public QObject, public NodeHandleInterface {
  Q_OBJECT
    Q_INTERFACES(NodeHandleInterface)
    Q_PLUGIN_METADATA(IID "Gratin.FractalNoiseCellular")

    public:
  const QString     stringId() const {return "fractalNoiseCellular";}
  unsigned int      version () const {return 1;}

  const QString     name    () const {return "fractalNoiseCellular";}
  const QString     path    () const {return "CustomNoises/";}
  const QString     desc    () const {return "procedural fractal cellular noise";}
  const QString     help    () const {return tr("fractal noise");}

  const QStringList inputNames () const {return QStringList() << "positionWMap" << "depthMap"; }
  const QStringList outputNames() const {return (QStringList() << "rendering");}

  NodeInterface *createInstance(PbGraph *parent) {
    return new FractalNoiseCellularNode(parent,new NodeHandle(this));
  }

  NodeInterface *createInstance(PbGraph *parent,
				const QString &name,
				const QString &path,
				const QString &desc,
				const QString &help,
				const QStringList &inputNames,
				const QStringList &outputNames) {
    return new FractalNoiseCellularNode(parent,new NodeHandle(stringId(),version(),name,path,desc,help,
						   inputNames,outputNames));
  }
};

#endif // COHERENTSTYLE01
