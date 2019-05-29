// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "advancedGbuffers.h"
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <iostream>

#include "core/pbgraph.h"
#include "misc/extinclude.h"
#include "misc/glutils.h"
#include "misc/mesh.h"
#include "multiObjLoader.h"

using namespace std;

static const float TAASampleScale = 1.0f / 16.0f;
static const int TAASampleCount = 32;
static Vector2f TAASampleOffsets[TAASampleCount] = {
    {-4.0f, -7.0f}, {-7.0f, -5.0f}, {-3.0f, -5.0f}, {-5.0f, -4.0f},
    {-1.0f, -4.0f}, {-2.0f, -2.0f}, {-6.0f, -1.0f}, {-4.0f, 0.0f},
    {-7.0f, 1.0f}, {-1.0f, 2.0f}, {-6.0f, 3.0f}, {-3.0f, 3.0f},
    {-7.0f, 6.0f}, {-3.0f, 6.0f}, {-5.0f, 7.0f}, {-1.0f, 7.0f},
    {5.0f, -7.0f}, {1.0f, -6.0f}, {6.0f, -5.0f}, {4.0f, -4.0f},
    {2.0f, -3.0f}, {7.0f, -2.0f}, {1.0f, -1.0f}, {4.0f, -1.0f},
    {2.0f, 1.0f}, {6.0f, 2.0f}, {0.0f, 4.0f}, {4.0f, 4.0f},
    {2.0f, 5.0f}, {7.0f, 5.0f}, {5.0f, 6.0f}, {3.0f, 7.0f}};


QDir Gbuffers2Widget::_currentPath = QDir::currentPath();

Gbuffers2Widget::Gbuffers2Widget(Gbuffers2Node *node)
    : GenericCustomWidget(node), _load(new QPushButton("Load...")),
      _default(new QPushButton("Reset cam")),
      _rotX(new FloatSliderWidget(node,"eulerAngleX", 0.0f,6.28318530718f,0.0f)),
      _rotY(new FloatSliderWidget(node,"eulerAngleY", 0.0f,6.28318530718f,0.0f)),
      _rotZ(new FloatSliderWidget(node,"eulerAngleZ", 0.0f,6.28318530718f,0.0f))  {

  _camWidget = new TrackballCameraWidget(node, "Camera", node->camera());
  _taaIndex = new IntSpinWidget(node, "TAA index", 0, 100000, 0, true);

  // default parameters
  userLayout()->addWidget(_load);
  userLayout()->addWidget(_rotX);
  userLayout()->addWidget(_rotY);
  userLayout()->addWidget(_rotZ);
  userLayout()->addWidget(_camWidget);
  userLayout()->addWidget(_taaIndex);
  userLayout()->addWidget(_default);

  connect(_load, SIGNAL(clicked()), this, SLOT(loadClicked()));
  connect(_default, SIGNAL(clicked()), this, SLOT(defaultClicked()));

  addChildWidget(_camWidget);
  addChildWidget(_rotX);
  addChildWidget(_rotY);
  addChildWidget(_rotZ);
}

void Gbuffers2Widget::loadClicked() {
  QString filename = QFileDialog::getOpenFileName(0, "Load object",
                                                  _currentPath.absolutePath(),
                                                  "Objects (*.obj);;All (*.*)");

  if (filename.isEmpty()) {
    return;
  }

  QDir d(".");
  _currentPath = d.filePath(filename);

  ((Gbuffers2Node *)node())->loadObject(filename);
  updateGraph();
}

void Gbuffers2Widget::defaultClicked() {
  ((Gbuffers2Node *)node())->initCam();
  updateGraph();
}

Gbuffers2Node::Gbuffers2Node(PbGraph *parent, NodeHandle *handle)
    : GenericCustomNode(true, false, false, true, parent, handle, true, false,
                        Vector2f(0.0f, 0.0f), Vector2f(512.0f, 512.0f)),
      _filename(GRATIN_APPLI_DATA_PATH + "/objs/sphere.obj"),
      _camera(
          new TrackballCamera(Vector2i(512, 512), Vector3f(0, 0, 0), 1.0f, 0)),
      _depth(NULL) {

  _w = new Gbuffers2Widget(this);

  initShaderSource();
  loadObject(_filename);
  _w->updateWidget();
}

Gbuffers2Node::~Gbuffers2Node() {
  // delete _vao;
  delete _camera;
  delete _depth;
}

void Gbuffers2Node::apply() {
  Matrix4f M = _camera->modelMatrix();
  Matrix4f V = _camera->viewMatrix();
  Matrix4f P = _camera->projMatrix();
  
  V = rotateFromView(V,Vector3f(1.0f,0.0f,0.0f),_w->rotX()->val());
  V = rotateFromView(V,Vector3f(0.0f,1.0f,0.0f),_w->rotY()->val());
  V = rotateFromView(V,Vector3f(0.0f,0.0f,1.0f),_w->rotZ()->val());

  
  float fWidth = (float)outputTex(0)->w();
  float fHeight = (float)outputTex(0)->h();

  Glutils::setViewport(outputTex(0)->w(), outputTex(0)->h());
  setOutputParams();

  _fbo.bind();
  _glf->glDrawBuffers(nbOutputs(), buffersOfOutputTex(0));

  Vector2f TAAOffset = TAASampleScale * TAASampleOffsets[_w->_taaIndex->val()%TAASampleCount];
  Affine3f projMatrixJitter(Translation3f(2.0f * TAAOffset.x() / fWidth,
					  2.0f * TAAOffset.y() / fHeight, 0.0f));

  // apply projection matrix jitter for TAA
  Matrix4f projMatrixTAA = projMatrixJitter.matrix() * P;

  initOpenGLState();

  enableShaders();
  auto mvp = P*V*M;

  if (first) {
    prevModel = M;
    prevView = V;
    prevProj = projMatrixTAA;
  }
  first = false;

  _p->setUniformMatrix4fv("prevModel", (GLfloat *)prevModel.data());
  _p->setUniformMatrix4fv("prevView", (GLfloat *)prevView.data());
  _p->setUniformMatrix4fv("prevProj", (GLfloat *)prevProj.data());
  _p->setUniformMatrix4fv("model", (GLfloat *)M.data());
  _p->setUniformMatrix4fv("view", (GLfloat *)V.data());
  _p->setUniformMatrix4fv("proj", (GLfloat *)projMatrixTAA.data());
  prevModel = _camera->modelMatrix();
  prevView = _camera->viewMatrix();
  prevProj = projMatrixTAA;

  _p->setUniform1f("zmin", _camera->zmin());
  _p->setUniform1f("zmax", _camera->zmax());

  //std::cerr << "num submeshes = " << _submeshes.size() << "\n";

  for (unsigned int si = 0; si < _submeshes.size(); ++si) {
    Submesh &sm = _submeshes[si];
    // transform aabb in clip space
    sm.vao->bind();
    auto aabb_proj = sm.aabb.transform(mvp);
    /*std::cerr << "aabb_proj=" << aabb_proj.xmin << "," << aabb_proj.xmax << ","
              << aabb_proj.ymin << "," << aabb_proj.ymax << ","
              << aabb_proj.zmin << "," << aabb_proj.zmax << "\n";*/
    _p->setUniform3f("modelAABBMin", aabb_proj.xmin, aabb_proj.ymin,
                     aabb_proj.zmin);
    _p->setUniform3f("modelAABBMax", aabb_proj.xmax, aabb_proj.ymax,
                     aabb_proj.zmax);
    _p->setUniform1i("objectID", si);


    //std::cerr << "num submesh faces = " << sm.mesh->nbFaces() << "\n";

    if(useTesselation()) {
      _glf->glPatchParameteri(GL_PATCH_VERTICES,3);
      sm.vao->drawElements(GL_PATCHES,sm.mesh->nbFaces() * 3);
    } else {
      sm.vao->drawElements(GL_TRIANGLES,sm.mesh->nbFaces() * 3);
    }

    sm.vao->unbind();
  }


  FramebufferObject::unbind();

  // upload camera matrices (output #0)
  outputTex(0)->bind();
  float camData[4*4*3];
  memcpy(camData, M.data(), 4*4*4);
  memcpy(camData+4*4, V.data(), 4*4*4);
  memcpy(camData+2*4*4, projMatrixTAA.data(), 4*4*4);
  _glf->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 3, GL_RGBA, GL_FLOAT, (GLfloat*)camData);

  disableShaders();
  cleanOpenGLState();
}

void Gbuffers2Node::loadObject(const QString &filename) {
  _submeshes.clear();   // TODO delete...
  QString f = filename;
  QDir d(".");
  f = d.absoluteFilePath(f);

  vector<Mesh *> outMeshes;
  Mesh *mesh = MultiObjLoader::load(f.toStdString(), outMeshes);
  for (unsigned int mi = 0; mi < outMeshes.size(); ++mi) {
    Submesh sm;
    sm.mesh = outMeshes[mi];
    sm.nbElements = sm.mesh->nbFaces() * 3;
    sm.vao = 0;
    _submeshes.push_back(sm);
  }

  if (!mesh || mesh->nbVertices() == 0 || mesh->nbFaces() == 0)
    return;

  _filename = f;

  // init camera
  _camera->setSceneParams(mesh->barycenter(), mesh->radius());

  // init VAOs
  makeCurrent();
  for (unsigned int mi = 0; mi < _submeshes.size(); ++mi) {
    Submesh &sm = _submeshes[mi];
    delete sm.vao;
    sm.vao = new VertexarrayObject();
    sm.vao->addAttrib(sm.mesh->nbVertices() * sizeof(Vector3f),
                      sm.mesh->verticesPtr(), 3);
    sm.vao->addAttrib(sm.mesh->nbVertices() * sizeof(Vector3f),
                      sm.mesh->normalsPtr(), 3);
    sm.vao->addAttrib(sm.mesh->nbVertices() * sizeof(Vector3f),
                      sm.mesh->tangentsPtr(), 3);
    sm.vao->addAttrib(sm.mesh->nbVertices() * sizeof(Vector2f),
                      sm.mesh->uvcoordsPtr(), 2);
    sm.vao->setIndices(sm.mesh->nbFaces() * sizeof(Vector3i),
                       sm.mesh->facesPtr());
    sm.nbElements = sm.mesh->nbFaces() * 3;
  }
  delete mesh;
}

void Gbuffers2Node::initCam() {
  Vector2i size(outputSize()[0], outputSize()[1]);
  _camera->reinit(size, _camera->sceneCenter(), _camera->sceneRadius());
}

void Gbuffers2Node::mousePressEvent(const Vector2f &p, QMouseEvent *me) {
  if (me->button() == Qt::LeftButton) {
    _camera->initRotation(p);
  } else if (me->button() == Qt::RightButton) {
    _camera->initMoveXY(p);
  } else if (me->button() == Qt::MidButton) {
    _camera->initMoveZ(p);
  }

  GenericCustomNode::mousePressEvent(p, me);
}

void Gbuffers2Node::mouseMoveEvent(const Vector2f &p, QMouseEvent *me) {
  _camera->move(p);
  GenericCustomNode::mouseMoveEvent(p, me);
}

void Gbuffers2Node::wheelEvent(const Vector2f &p, QWheelEvent *we) {
  GenericCustomNode::wheelEvent(p, we);

  const Vector2f v(0.0f, 60.0f);

  _camera->initMoveZ(p);
  if (we->delta() > 0) {
    _camera->move(p + v);
  } else {
    _camera->move(p - v);
  }
  update();
}

void Gbuffers2Node::initFBO() {
  NodeTexture2D::initFBO();

  _depth = new FloatTexture2D(
      TextureFormat(GL_TEXTURE_2D, outputSize()[0], outputSize()[1],
                    GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT),
      TextureParams(GL_NEAREST, GL_NEAREST));
  // contains camera parameters
  /*_cameraParams = new FloatTexture2D(
      TextureFormat(GL_TEXTURE_2D, outputSize()[0], outputSize()[1], GL_RGBA32F,
                    GL_RGBA, GL_FLOAT),
      TextureParams(GL_NEAREST, GL_NEAREST));*/

  _fbo.bind();
  _fbo.attachTexture(GL_TEXTURE_2D, _depth->id(), GL_DEPTH_ATTACHMENT);
  //_fbo.attachTexture(GL_TEXTURE_2D, _cameraParams->id(), GL_COLOR_ATTACHMENT0);
  _fbo.isValid();
  FramebufferObject::unbind();
}

void Gbuffers2Node::cleanFBO() {
  NodeTexture2D::cleanFBO();
  delete _depth;
  _depth = NULL;
}

const QString Gbuffers2Node::constantVertHead() {
  return QObject::tr("layout(location = 0) in vec3 inVertex;\n"
                     "layout(location = 1) in vec3 inNormal;\n"
                     "layout(location = 2) in vec3 inTangent;\n"
                     "layout(location = 3) in vec2 inTexcoord;\n");
}

const QString Gbuffers2Node::defaultVertBody() {
  return QObject::tr("out vec3  normalV;\n"
                     "out float depthV;\n\n"
                     "void main() {\n"
                     "\tmat4 mdv    = view*model;\n"
                     "\tmat4 mvp    = proj*mdv;\n\n"
                     "\tnormalV     = (mdv*vec4(inNormal,0)).xyz;\n"
                     "\tdepthV      = "
                     "(clamp(-(mdv*vec4(inVertex,1)).z,zmin,zmax)-zmin)/"
                     "(zmax-zmin);\n"
                     "\tgl_Position = mvp*vec4(inVertex,1);\n"
                     "}\n");
}

const QString Gbuffers2Node::constantFragHead() { return QString(); }

const QString Gbuffers2Node::defaultFragBody() {
  return QObject::tr("in vec3  normalV;\n"
                     "in float depthV;\n\n"
                     "void main() {\n"
                     "\toutBuffer0 = vec4(normalize(normalV),depthV);\n"
                     "}\n");
}

const QString Gbuffers2Node::constantTessCHead() {
  return QObject::tr("layout(vertices = 3) out;\n");
}

const QString Gbuffers2Node::defaultTessCBody() {
  return QObject::tr("void main(void) {\n"
                     "\tgl_TessLevelOuter[0] = 1;\n"
                     "\tgl_TessLevelOuter[1] = 1;\n"
                     "\tgl_TessLevelOuter[2] = 1;\n"
                     "\tgl_TessLevelOuter[3] = 1;\n"
                     "\tgl_TessLevelInner[0] = 1;\n"
                     "\tgl_TessLevelInner[1] = 1;\n"
                     "\tgl_out[gl_InvocationID].gl_Position = "
                     "gl_in[gl_InvocationID].gl_Position;\n"
                     "}\n");
}

const QString Gbuffers2Node::constantTessEHead() { return QString(); }

const QString Gbuffers2Node::defaultTessEBody() {
  return QObject::tr("layout(triangles,equal_spacing,ccw) in;\n\n"
                     "void main() {\n"
                     "\tvec4 p1 = gl_TessCoord.x*gl_in[0].gl_Position;\n"
                     "\tvec4 p2 = gl_TessCoord.y*gl_in[1].gl_Position;\n"
                     "\tvec4 p3 = gl_TessCoord.z*gl_in[2].gl_Position;\n"
                     "\tgl_Position = p1+p2+p3;\n"
                     "}\n");
}

const QString Gbuffers2Node::constantGeomHead() {
  return QObject::tr("layout(triangles) in;\n");
}

const QString Gbuffers2Node::defaultGeomBody() {
  return QObject::tr("layout(triangle_strip, max_vertices = 3) out;\n\n"
                     "void main() {\n"
                     "\tgl_Position = gl_in[0].gl_Position; EmitVertex();\n"
                     "\tgl_Position = gl_in[1].gl_Position; EmitVertex();\n"
                     "\tgl_Position = gl_in[2].gl_Position; EmitVertex();\n"
                     "\tEndPrimitive();\n"
                     "}\n");
}

bool Gbuffers2Node::save(SceneSaver *saver) {
  bool ret = true;

  // obj file and camera
  QString name = saver->absoluteToRelativePath(_filename);
  if (!saver->saveString("filename", name))
    ret = false;
  if (!saver->saveIOData("camera", _camera))
    ret = false;
  if (!GenericCustomNode::save(saver))
    ret = false;

  return ret;
}

bool Gbuffers2Node::load(SceneLoader *loader) {
  bool ret = true;

  _filename = GRATIN_APPLI_DATA_PATH + "/objs/sphere.obj";
  QString name;
  if (!loader->loadString("filename", name))
    ret = false;

  if (ret) {
    _filename = loader->relativeToAbsolutePath(name);
  }

  if (!loader->loadIOData("camera", _camera))
    ret = false;
  loadObject(_filename);
  if (!GenericCustomNode::load(loader))
    ret = false;

  return ret;
}
