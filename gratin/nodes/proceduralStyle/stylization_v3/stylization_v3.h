// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef Stylization_v3_H
#define Stylization_v3_H

#include <QVBoxLayout>
#include "view/nodeWidget.h"
#include "view/widgetParams.h"
#include "core/nodeHandle.h"
#include "core/nodeTexture2D.h"
#include <cstdlib>

// **** a widget containing a keyframed slider ****
class Stylization_v3Widget : public NodeWidget {
 public:
 Stylization_v3Widget(NodeInterface *node) :
  NodeWidget(node),
    _alphaFactor(new FloatSliderWidget(node, "alphaFactor", 0,1,0.9)),
    _splatSize(new FloatSliderWidget(node,"splatSize", 0.0f,1.0f,0.05f)),
    _splatDepthFactor(new FloatSliderWidget(node,"splatDepthFactor", 0.01,10.,1.)),
    _rotateSplat(new FloatSliderWidget(node,"rotateSplat", 0,1,1)) {
    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(_alphaFactor);
    l->addWidget(_splatSize);
    l->addWidget(_splatDepthFactor);
    l->addWidget(_rotateSplat);
    setLayout(l);
    addChildWidget(_alphaFactor);
    addChildWidget(_splatSize);
    addChildWidget(_splatDepthFactor);
    addChildWidget(_rotateSplat);
  }

  inline FloatSliderWidget *alphaFactor() const {return _alphaFactor;}
  inline FloatSliderWidget *splatSize() const {return _splatSize;}
  inline FloatSliderWidget *splatDepthFactor() const {return _splatDepthFactor;}
  inline FloatSliderWidget *rotateSplat() const {return _rotateSplat;}

 private:
  FloatSliderWidget *_alphaFactor, *_splatSize, *_splatDepthFactor, *_rotateSplat;
};




// **** the node itself, containing the shader and the widget ****
class Stylization_v3Node : public NodeTexture2D {
 public:
  Stylization_v3Node(PbGraph *parent=NULL,NodeHandle *handle=NULL);
  ~Stylization_v3Node();
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

  inline float randRange(float v1,float v2) const {
    return v1 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(v2-v1)));
  }
  
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
  Stylization_v3Widget *_w;

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
class Stylization_v3Handle : public QObject, public NodeHandleInterface {
  Q_OBJECT
    Q_INTERFACES(NodeHandleInterface)
    Q_PLUGIN_METADATA(IID "Gratin.Stylization_v3")

    public:
  const QString     stringId() const {return "stylization_v3Id";}
  unsigned int      version () const {return 1;}

  const QString     name    () const {return "stylization_v3";}
  const QString     path    () const {return "stylization/";}
  const QString     desc    () const {return "procedural stylisation with image as splat";}
  const QString     help    () const {return tr("procedural style.\n"
						"TODO");}

  const QStringList inputNames () const {return QStringList() << "matrices" << "positionWMap" << "normalWMap" << "colorMap" << "depthMap" << "noiseMap" << "splatMap" << "splatNormalMap";  }
  const QStringList outputNames() const {return (QStringList() << "rendering");}

  NodeInterface *createInstance(PbGraph *parent) {
    return new Stylization_v3Node(parent,new NodeHandle(this));
  }

  NodeInterface *createInstance(PbGraph *parent,
				const QString &name,
				const QString &path,
				const QString &desc,
				const QString &help,
				const QStringList &inputNames,
				const QStringList &outputNames) {
    return new Stylization_v3Node(parent,new NodeHandle(stringId(),version(),name,path,desc,help,
						   inputNames,outputNames));
  }
};

#endif // Stylization_v3
