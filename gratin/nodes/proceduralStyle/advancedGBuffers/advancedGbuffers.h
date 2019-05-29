// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef GBUFFERSCUSTOM_H
#define GBUFFERSCUSTOM_H

#include "genericCustomNode.h"
#include "genericCustomWidget.h"
#include "core/nodeHandle.h"
#include "misc/gpuProgram.h"
#include "misc/vertexarrayObject.h"
#include <QObject>
#include "AABB.h"
#include "misc/mesh.h"
#include <Eigen/Geometry>

class Gbuffers2Node;

class Gbuffers2Widget : public GenericCustomWidget {
  Q_OBJECT

public:
  Gbuffers2Widget(Gbuffers2Node *node);
  IntSpinWidget *_taaIndex;

  inline FloatSliderWidget *rotX() const {return _rotX;}
  inline FloatSliderWidget *rotY() const {return _rotY;}
  inline FloatSliderWidget *rotZ() const {return _rotZ;}
		  
public slots:
  void loadClicked();
  void defaultClicked();

private:
  QPushButton *_load;
  QPushButton *_default;
  FloatSliderWidget *_rotX;
  FloatSliderWidget *_rotY;
  FloatSliderWidget *_rotZ;
  TrackballCameraWidget *_camWidget;
  static QDir _currentPath;
};

class Gbuffers2Node : public GenericCustomNode {
public:
  Gbuffers2Node(PbGraph *parent = NULL, NodeHandle *handle = NULL);
  ~Gbuffers2Node();

  void apply();

  void mousePressEvent(const Vector2f &m, QMouseEvent *);
  void mouseMoveEvent(const Vector2f &m, QMouseEvent *);
  void wheelEvent(const Vector2f &, QWheelEvent *);

  inline NodeWidget *widget() { return _w; }

  virtual const QString constantVertHead();
  virtual const QString defaultVertBody();
  virtual const QString constantFragHead();
  virtual const QString defaultFragBody();
  virtual const QString constantTessCHead();
  virtual const QString defaultTessCBody();
  virtual const QString constantTessEHead();
  virtual const QString defaultTessEBody();
  virtual const QString constantGeomHead();
  virtual const QString defaultGeomBody();

  bool save(SceneSaver *saver);
  bool load(SceneLoader *loader);

  void loadObject(const QString &filename);
  void initCam();

  inline TrackballCamera *camera() { return _camera; }

protected:
  const QStringList defaultUniformNames() const {
    return QStringList() << "model"
                         << "view"
                         << "proj"
                         << "prevModel"
                         << "prevView"
                         << "prevProj"
                         << "zmin"
                         << "zmax"
                         << "modelAABBMin"
                         << "modelAABBMax"
                         << "objectID";
  }
  const QStringList defaultUniformTypes() const {
    return QStringList() << "mat4"
                         << "mat4"
                         << "mat4"
                         << "mat4"
                         << "mat4"
                         << "mat4"
                         << "float"
                         << "float"
                         << "vec3"
                         << "vec3"
                         << "int";
  }

  void initFBO();
  void cleanFBO();
  
  inline Matrix4f rotateFromView(const Matrix4f &V,const Vector3f &axis,float angle) {
    const Vector4f a = V.inverse()*Vector4f(axis[0],axis[1],axis[2],0.0f);
    Affine3f T(AngleAxisf(angle,Vector3f(a[0],a[1],a[2])));
    return V*T.matrix();
  }
  
private:
  QString _filename;
  TrackballCamera *_camera;
  bool first = true;
  Matrix4f prevProj;
  Matrix4f prevView;
  Matrix4f prevModel;
  FloatTexture2D* _depth;
  Gbuffers2Widget *_w;
  struct Submesh {
	  Mesh* mesh;
	  AABB aabb;
      VertexarrayObject *vao;
      unsigned int nbElements;
  };
  std::vector<Submesh> _submeshes;
};

class Gbuffers2Handle : public QObject, public NodeHandleInterface {
  Q_OBJECT
  Q_INTERFACES(NodeHandleInterface)
  Q_PLUGIN_METADATA(IID "Gratin.advancedGbuffers")

public:
  const QString stringId() const { return "advancedGbuffers"; }
  unsigned int version() const { return 1; }

  const QString name() const { return "Advanced GBuffers"; }
  const QString path() const { return "Custom/"; }
  const QString desc() const { return "Generic node for 3D object processing"; }
  const QString help() const {
    return tr(
        "This node was designed to create custom shaders\n"
        "The settings widget allows to modify input, output parameters\n"
        "as well as the output image sizes.\n\n"
        "WARNING1: you will be able to modify the number of inputs/outputs\n"
        "only if the node is entirely disconnected.\n"
        "WARNING2: input/output names will not be modified if a custom node is "
        "created\n"
        "from this node because these names should be edited directly in the "
        "setting widget\n"
        "WARNING3: modifying the settings of a node will remove all "
        "previously\n"
        "defined keyframes\n\n"
        "* Special data allows to modify the output image (using a multiple "
        "of\n"
        "the input size, plus an offset vector\n"
        "Check use mouse/keyboard if you want to interact with the mouse\n\n"
        "* Input data allows to add/remove/displace input data (only if "
        "disconnected)\n\n"
        "* Output data allows to add/remove/displace output data (only if "
        "disconnected)\n\n"
        "* Param data allows to add/remove parameters that will automatically "
        "be \n"
        "included in your shader.\n"
        "For each of them you may choose a proper name, type, and "
        "min/max/default values.\n"
        "Check keyframes if you want your parameter to be accessible\n"
        " through the animation widget\n\n"
        "In the node interface itself:\n"
        "Source tab contains the head and body of your GLSL source (only the "
        "body is editable)\n"
        "Params tab contains the defined widgets according to your setting "
        "choices\n"
        "Log tab contains compilation errors\n\n"
        "Once your settings and source completed, click on apply to see the "
        "result\n");
  }

  const QStringList inputNames() const { return QStringList(); }
  const QStringList outputNames() const {
    return QStringList() << "cameraParams";
  }

  NodeInterface *createInstance(PbGraph *parent) {
    return new Gbuffers2Node(parent, new NodeHandle(this));
  }

  NodeInterface *createInstance(PbGraph *parent, const QString &name,
                                const QString &path, const QString &desc,
                                const QString &help,
                                const QStringList &inputNames,
                                const QStringList &outputNames) {
    return new Gbuffers2Node(
        parent, new NodeHandle(stringId(), version(), name, path, desc, help,
                               inputNames, outputNames));
  }
};

#endif // GBUFFERSCUSTOM_H
