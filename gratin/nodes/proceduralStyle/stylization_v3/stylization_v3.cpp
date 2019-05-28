// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>

#include "stylization_v3.h"

//QString Stylization_v3Node::SHADER_PATH = QString("/home/romain/research/proceduralStylization/gratin/nodes/proceduralStyle/stylization_v3/");
// QString Stylization_v3Node::SHADER_PATH = QString("/home/misnel/procedural-stylization/gratin/nodes/proceduralStyle/stylization_v3/");
QString Stylization_v3Node::SHADER_PATH = QString("/home/vergne/projects/procedural-stylization/gratin/nodes/proceduralStyle/stylization_v3/");


Stylization_v3Node::Stylization_v3Node(PbGraph *parent,NodeHandle *handle)
  : NodeTexture2D(parent,handle),
    _pSplat(QString(SHADER_PATH+"stylization_v3-splatting.vert"),
       QString(SHADER_PATH+"stylization_v3-splatting.frag")),
    _pBlend(QString(SHADER_PATH+"stylization_v3-blending.vert"),
       QString(SHADER_PATH+"stylization_v3-blending.frag")),
    _w(new Stylization_v3Widget(this)),
    _vaoSplat(NULL),
    _nbElements(0),
    _sw(1),
    _sh(1),
    _acBuffer(0),
    _llBuffer(0),
    _headTex(0),
    _clBuffer(0) {

    _pSplat.addUniform("matrices");
    _pSplat.addUniform("positionWMap");
    _pSplat.addUniform("normalWMap");
    _pSplat.addUniform("colorMap");
    _pSplat.addUniform("depthMap");
    _pSplat.addUniform("noiseMap");
    _pSplat.addUniform("splatMap");
    _pSplat.addUniform("splatNormalMap");
    _pSplat.addUniform("maxNodes");
    _pSplat.addUniform("splatSize");
    _pSplat.addUniform("splatDepthFactor");

    _pBlend.addUniform("image");
    _pBlend.addUniform("gammaBlend");



    initSprites();






}

Stylization_v3Node::~Stylization_v3Node() {
  delete _vaoSplat;
  cleanOITData();
}


void Stylization_v3Node::apply() {


  // init viewport
  Glutils::setViewport(outputTex(0)->w(),outputTex(0)->h());

  // init first pass opengl settings
  _glf->glClearColor(0.0f,0.0f,0.0f,0.0f);
  _glf->glDisable(GL_DEPTH_TEST);
  _glf->glDisable(GL_BLEND);


  GLuint zero = 0;
  _glf->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _llBuffer);
  _glf->glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, _acBuffer);
  _glf->glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),&zero);
  _glf->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _clBuffer);
  _glf->glBindTexture(GL_TEXTURE_2D,_headTex);
  _glf->glTexSubImage2D(GL_TEXTURE_2D,0,0,0,_sw,_sh,GL_RED_INTEGER,GL_UNSIGNED_INT, NULL);



  // THIS WILL HAVE TO BE REMOVED
  //_glf->glEnable(GL_BLEND);
  //_glf->glBlendFunc(GL_ONE,GL_ONE);
  // ****************************

  _fbo.bind();
  _glf->glDrawBuffers(1,buffersOfTmpTex(0));
  _glf->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _vaoSplat->bind();
  _pSplat.enable();
  _pSplat.setUniformTexture("matrices",GL_TEXTURE_2D,inputTex(0)->id());
  _pSplat.setUniformTexture("positionWMap",GL_TEXTURE_2D,inputTex(1)->id());
  _pSplat.setUniformTexture("normalWMap",GL_TEXTURE_2D,inputTex(2)->id());
  _pSplat.setUniformTexture("colorMap",GL_TEXTURE_2D,inputTex(3)->id());
  _pSplat.setUniformTexture("depthMap",GL_TEXTURE_2D,inputTex(4)->id());
  _pSplat.setUniformTexture("noiseMap",GL_TEXTURE_2D,inputTex(5)->id());
  _pSplat.setUniformTexture("splatMap",GL_TEXTURE_2D,inputTex(6)->id());
  _pSplat.setUniformTexture("splatNormalMap",GL_TEXTURE_2D,inputTex(7)->id());


  _pSplat.setUniform1f("splatSize",_w->splatSize()->val());
  _pSplat.setUniform1f("splatDepthFactor",_w->splatDepthFactor()->val());
  _glf->glUniform1ui(_glf->glGetUniformLocation(_pSplat.id(),"maxNodes"),_maxNodes);
  _vaoSplat->drawArrays(GL_TRIANGLES,0,_nbElements);
  _pSplat.disable();
  _vaoSplat->unbind();

  // second pass
  _glf->glDisable(GL_BLEND);
  _glf->glDisable(GL_DEPTH_TEST);
  _glf->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  drawOutputs(buffersOfOutputTex(0),1,false,false);
  _glf->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _pBlend.enable();
  //_pBlend.setUniformTexture("image",GL_TEXTURE_2D,tmpTex(0)->id());
  _pBlend.setUniformTexture("image",GL_TEXTURE_2D,inputTex(3)->id());
  _pBlend.setUniform1f("gammaBlend",_w->gammaBlend()->val());
  
  _unitSquareVao->bind();
  _unitSquareVao->drawArrays(GL_TRIANGLES,0,6);
  _unitSquareVao->unbind();
  _pBlend.disable();

  _glf->glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
  _glf->glBindTexture(GL_TEXTURE_2D, 0);
  _glf->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
  _glf->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  FramebufferObject::unbind();
  //_glf->glDisable(GL_BLEND);
}


void Stylization_v3Node::initSprites() {
  vector<Vector2f> vertices;
  vector<Vector2f> splatcoord;
  vector<Vector2f> texcoord;

  unsigned int w = _sw;
  unsigned int h = _sh;

  const float hsize = 0.1f;
  
  for(unsigned int i=0;i<h;++i) {
    for(unsigned int j=0;j<w;++j) {
      const float x = 2.0f*(((float)j+0.5f)/(float)w)-1.0f;
      const float y = 2.0f*(((float)i+0.5f)/(float)h)-1.0f;

      Vector2f v1 = Vector2f(x-hsize,y-hsize);
      Vector2f v2 = Vector2f(x+hsize,y-hsize);
      Vector2f v3 = Vector2f(x+hsize,y+hsize);
      Vector2f v4 = Vector2f(x-hsize,y+hsize);

      vertices.push_back(v1);
      vertices.push_back(v2);
      vertices.push_back(v3);
      vertices.push_back(v1);
      vertices.push_back(v3);
      vertices.push_back(v4);
      
      texcoord.push_back(Vector2f(0.0f,0.0f));
      texcoord.push_back(Vector2f(1.0f,0.0f));
      texcoord.push_back(Vector2f(1.0f,1.0f));
      texcoord.push_back(Vector2f(0.0f,0.0f));
      texcoord.push_back(Vector2f(1.0f,1.0f));
      texcoord.push_back(Vector2f(0.0f,1.0f));

      splatcoord.push_back(Vector2f(x,y));
      splatcoord.push_back(Vector2f(x,y));
      splatcoord.push_back(Vector2f(x,y));
      splatcoord.push_back(Vector2f(x,y));
      splatcoord.push_back(Vector2f(x,y));
      splatcoord.push_back(Vector2f(x,y));
    }
  }

  delete _vaoSplat;
  _vaoSplat = new VertexarrayObject();

  unsigned int nbVert = static_cast<unsigned int>(vertices.size());

  _vaoSplat->addAttrib(nbVert*sizeof(Vector2f),vertices[0].data(),2);
  _vaoSplat->addAttrib(nbVert*sizeof(Vector2f),texcoord[0].data(),2);
  _vaoSplat->addAttrib(nbVert*sizeof(Vector2f),splatcoord[0].data(),2);

  cout << "Nb elem : " << _nbElements << endl;
  
  _nbElements = nbVert;
}

void Stylization_v3Node::initOITData() {
  cleanOITData();

  _glf->glGenBuffers(1, &_acBuffer);
  _glf->glGenBuffers(1, &_llBuffer);
  _glf->glGenTextures(1, &_headTex);
  _glf->glGenBuffers(1, &_clBuffer);

  //glGenBuffers(2, buffers);
  _maxNodes = 300 * _sw * _sh;
  GLint nodeSize = sizeof(Vector4f)+sizeof(GLfloat)+sizeof(GLuint);

  // Our atomic counter
  _glf->glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, _acBuffer);
  _glf->glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

  // The buffer for the head pointers, as an image texture
  //glGenTextures(1, &headPtrTex);
  _glf->glBindTexture(GL_TEXTURE_2D, _headTex);
  _glf->glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, _sw, _sh);
  _glf->glBindImageTexture(0, _headTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

  // The buffer of linked lists
  _glf->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _llBuffer);
  _glf->glBufferData(GL_SHADER_STORAGE_BUFFER, _maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW);

  //prog.setUniform("MaxNodes", maxNodes);

  std::vector<GLuint> clBuf(_sw*_sh, 0xffffffff);
  //glGenBuffers(1, &clearBuf);
  _glf->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _clBuffer);
  _glf->glBufferData(GL_PIXEL_UNPACK_BUFFER, clBuf.size() * sizeof(GLuint),&clBuf[0], GL_STATIC_COPY);

  // unbind everything
  _glf->glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
  _glf->glBindTexture(GL_TEXTURE_2D, 0);
  _glf->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
  _glf->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Stylization_v3Node::cleanOITData() {
  if(_glf->glIsBuffer(_acBuffer)) {
    _glf->glDeleteBuffers(1,&_acBuffer);
  }

  if(_glf->glIsBuffer(_llBuffer)) {
    _glf->glDeleteBuffers(1,&_llBuffer);
  }

  if(_glf->glIsTexture(_headTex)) {
    _glf->glDeleteTextures(1,&_headTex);
  }

  if(_glf->glIsBuffer(_clBuffer)) {
    _glf->glDeleteBuffers(1,&_clBuffer);
  }

  _acBuffer = 0;
  _llBuffer = 0;
  _headTex  = 0;
  _clBuffer = 0;
}

void Stylization_v3Node::initFBO() {
  NodeTexture2D::initFBO();

  if(nbOutputs()>0) {
    _sw = outputTex(0)->w();
    _sh = outputTex(0)->h();
  } else {
    _sw = _sh = 1;
  }
  initSprites();
  initOITData();
}

void Stylization_v3Node::cleanFBO() {
  NodeTexture2D::cleanFBO();
  delete _vaoSplat; _vaoSplat = NULL;
  cleanOITData();
}
