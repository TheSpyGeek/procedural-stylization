// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef PROCEDURALSTYLEIMG_H
#define PROCEDURALSTYLEIMG_H

#include <QVBoxLayout>
#include "view/nodeWidget.h"
#include "view/widgetParams.h"
#include "core/nodeHandle.h"
#include "core/nodeTexture2D.h"

// **** a widget containing a keyframed slider ****
class ProceduralStyleIMGWidget : public NodeWidget {
 public:
 ProceduralStyleIMGWidget(NodeInterface *node) :
  NodeWidget(node),
    _halfsize(new IntSliderWidget(node,"size",1,100,32)),
    _test(new FloatSliderWidget(node,"test",0,1,0.95)),
    _alphaFactor(new FloatSliderWidget(node, "alphaFactor", 0,1,0.9)),
    _splatSize(new FloatSliderWidget(node,"splatSize", 1.,100.,40.)) {
    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(_halfsize);
    l->addWidget(_test);
    l->addWidget(_alphaFactor);
    l->addWidget(_splatSize);
    setLayout(l);
    addChildWidget(_halfsize);
    addChildWidget(_test);
    addChildWidget(_alphaFactor);
    addChildWidget(_splatSize);
  }

  inline IntSliderWidget *halfsize() const {return _halfsize;}
  inline FloatSliderWidget *test() const {return _test;}
  inline FloatSliderWidget *alphaFactor() const {return _alphaFactor;}
  inline FloatSliderWidget *splatSize() const {return _splatSize;}

 private:
  IntSliderWidget *_halfsize;
  FloatSliderWidget *_test, *_alphaFactor, *_splatSize;
};




// **** the node itself, containing the shader and the widget ****
class ProceduralStyleIMGNode : public NodeTexture2D {
 public:
  ProceduralStyleIMGNode(PbGraph *parent=NULL,NodeHandle *handle=NULL);
  ~ProceduralStyleIMGNode();
  void apply();

  inline NodeWidget  *widget() {return _w;}
  void initSprites();
  inline virtual void reload() {_pSplat.reload();_pBlend.reload();}
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

  GPUProgram       _pSplat;
  GPUProgram       _pBlend;
  ProceduralStyleIMGWidget *_w;

  VertexarrayObject   *_vaoSplat;
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
    Q_PLUGIN_METADATA(IID "Gratin.ProceduralStyleIMG")

    public:
  const QString     stringId() const {return "proceduralStyleIMGId";}
  unsigned int      version () const {return 1;}

  const QString     name    () const {return "proceduralStyleIMG";}
  const QString     path    () const {return "Custom/";}
  const QString     desc    () const {return "procedural stylisation with image as splat";}
  const QString     help    () const {return tr("procedural style.\n"
						"TODO");}

  const QStringList inputNames () const {return QStringList() << "matrices" << "positionWMap" << "normalWMap" << "shadingMap" << "depthMap" << "noiseTex1" << "imgSplat";  }
  const QStringList outputNames() const {return (QStringList() << "rendering");}

  NodeInterface *createInstance(PbGraph *parent) {
    return new ProceduralStyleIMGNode(parent,new NodeHandle(this));
  }

  NodeInterface *createInstance(PbGraph *parent,
				const QString &name,
				const QString &path,
				const QString &desc,
				const QString &help,
				const QStringList &inputNames,
				const QStringList &outputNames) {
    return new ProceduralStyleIMGNode(parent,new NodeHandle(stringId(),version(),name,path,desc,help,
						   inputNames,outputNames));
  }
};

#endif // PROCEDURALSTYLEIMG