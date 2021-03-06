// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef Stylization_v2_H
#define Stylization_v2_H

#include <QVBoxLayout>
#include "view/nodeWidget.h"
#include "view/widgetParams.h"
#include "core/nodeHandle.h"
#include "core/nodeTexture2D.h"

// **** a widget containing a keyframed slider ****
class Stylization_v2Widget : public NodeWidget {
 public:
 Stylization_v2Widget(NodeInterface *node) :
  NodeWidget(node),
    _splatSize(new FloatSliderWidget(node,"splatSize", 1.,100.,40.)),
    _splatDepthFactor(new FloatSliderWidget(node,"splatDepthFactor", 0.01,10.,1.)),
    _rotateSplat(new IntSliderWidget(node,"rotateSplat", 0,2,1)) {
    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(_splatSize);
    l->addWidget(_splatDepthFactor);
    l->addWidget(_rotateSplat);
    setLayout(l);
    addChildWidget(_splatSize);
    addChildWidget(_splatDepthFactor);
    addChildWidget(_rotateSplat);
  }

  inline FloatSliderWidget *splatSize() const {return _splatSize;}
  inline FloatSliderWidget *splatDepthFactor() const {return _splatDepthFactor;}
  inline IntSliderWidget *rotateSplat() const {return _rotateSplat;}

 private:
  FloatSliderWidget *_splatSize, *_splatDepthFactor;
  IntSliderWidget *_rotateSplat;
};




// **** the node itself, containing the shader and the widget ****
class Stylization_v2Node : public NodeTexture2D {
 public:
  Stylization_v2Node(PbGraph *parent=NULL,NodeHandle *handle=NULL);
  ~Stylization_v2Node();
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
  Stylization_v2Widget *_w;

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
class Stylization_v2Handle : public QObject, public NodeHandleInterface {
  Q_OBJECT
    Q_INTERFACES(NodeHandleInterface)
    Q_PLUGIN_METADATA(IID "Gratin.Stylization_v2")

    public:
  const QString     stringId() const {return "stylization_v2Id";}
  unsigned int      version () const {return 1;}

  const QString     name    () const {return "stylization_v2";}
  const QString     path    () const {return "stylization/";}
  const QString     desc    () const {return "procedural stylisation with image as splat";}
  const QString     help    () const {return tr("procedural style.\n"
						"TODO");}

  const QStringList inputNames () const {return QStringList() << "matrices" << "positionWMap" << "normalWMap" << "colorMap" << "depthMap" << "noiseMap" << "splatMap" << "splatNormalMap";  }
  const QStringList outputNames() const {return (QStringList() << "rendering");}

  NodeInterface *createInstance(PbGraph *parent) {
    return new Stylization_v2Node(parent,new NodeHandle(this));
  }

  NodeInterface *createInstance(PbGraph *parent,
				const QString &name,
				const QString &path,
				const QString &desc,
				const QString &help,
				const QStringList &inputNames,
				const QStringList &outputNames) {
    return new Stylization_v2Node(parent,new NodeHandle(stringId(),version(),name,path,desc,help,
						   inputNames,outputNames));
  }
};

#endif // Stylization_v2
