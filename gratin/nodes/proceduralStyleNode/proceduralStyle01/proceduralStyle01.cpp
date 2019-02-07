// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>

#include "proceduralStyle01.h"

//QString ProceduralStyleNode::SHADER_PATH = QString("/disc/research/ideasAndNotes/coding-tests/silhouette-stylization/gratin-nodes/coherentStyle01/");
QString ProceduralStyleNode::SHADER_PATH = QString("/home/misnel/procedural-stylization/gratin/nodes/proceduralStyleNode/proceduralStyle01/");

ProceduralStyleNode::ProceduralStyleNode(PbGraph *parent,NodeHandle *handle)
  : NodeTexture2D(parent,handle),
    _pSplat(QString(SHADER_PATH+"proceduralStyle01-splatting.vert"),
       QString(SHADER_PATH+"proceduralStyle01-splatting.frag")),
    _pBlend(QString(SHADER_PATH+"proceduralStyle01-blending.vert"),
       QString(SHADER_PATH+"proceduralStyle01-blending.frag")),
    _w(new ProceduralStyleWidget(this)),
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
    _pSplat.addUniform("shadingMap");
    _pSplat.addUniform("depthMap");
    _pSplat.addUniform("noiseTex1");
    _pSplat.addUniform("size");
    _pSplat.addUniform("maxNodes");
    _pSplat.addUniform("test");

    _pBlend.addUniform("image");

    initSprites();
}

ProceduralStyleNode::~ProceduralStyleNode() {
  delete _vaoSplat;
  cleanOITData();
}

void ProceduralStyleNode::apply() {
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
  _pSplat.setUniformTexture("shadingMap",GL_TEXTURE_2D,inputTex(3)->id());
  _pSplat.setUniformTexture("depthMap",GL_TEXTURE_2D,inputTex(4)->id());
  _pSplat.setUniformTexture("noiseTex1",GL_TEXTURE_2D,inputTex(5)->id());
  _pSplat.setUniform1i("size",_w->halfsize()->val());
  _pSplat.setUniform1f("test",_w->test()->val());
  _glf->glUniform1ui(_glf->glGetUniformLocation(_pSplat.id(),"maxNodes"),_maxNodes);
  _vaoSplat->drawArrays(GL_POINTS,0,_nbElements);
  _pSplat.disable();
  _vaoSplat->unbind();

  // second pass
  _glf->glDisable(GL_BLEND);
  _glf->glDisable(GL_DEPTH_TEST);
  _glf->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  drawOutputs(buffersOfOutputTex(0),1,false,false);
  _glf->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _pBlend.enable();
  _pBlend.setUniformTexture("image",GL_TEXTURE_2D,tmpTex(0)->id());
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



/*
void ProceduralStyleNode::initSprites() {
  vector<Vector2f> vertices;
  unsigned int w = _sw;
  unsigned int h = _sh;

  for(unsigned int i=0;i<h;++i) {
    for(unsigned int j=0;j<w;++j) {
      const float x = 2.0f*(((float)j+0.5f)/(float)w)-1.0f;
      const float y = 2.0f*(((float)i+0.5f)/(float)h)-1.0f;

      vertices.push_back(Vector2f(x,y));
    }
}
*/


void ProceduralStyleNode::initSprites() {
  vector<Vector2f> vertices;
  unsigned int w = _sw;
  unsigned int h = _sh;

  float x,y;

  int width = 4;

  for(unsigned int i=0;i<h;++i) {

    for(unsigned int j=0;j<w;++j){


        x = 0.5 + sin(i)/4.0;
        y = 0.5 + cos(j)/4.0;

        vertices.push_back(Vector2f(x,y));
    }
}


  delete _vaoSplat;
  _vaoSplat = new VertexarrayObject();

  unsigned int nbVert = static_cast<unsigned int>(vertices.size());

  _vaoSplat->addAttrib(nbVert*sizeof(Vector2f),vertices[0].data(),2);
  _nbElements = nbVert;
}

void ProceduralStyleNode::initOITData() {
  cleanOITData();

  _glf->glGenBuffers(1, &_acBuffer);
  _glf->glGenBuffers(1, &_llBuffer);
  _glf->glGenTextures(1, &_headTex);
  _glf->glGenBuffers(1, &_clBuffer);

  //glGenBuffers(2, buffers);
  _maxNodes = 100 * _sw * _sh;
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

void ProceduralStyleNode::cleanOITData() {
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

void ProceduralStyleNode::initFBO() {
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

void ProceduralStyleNode::cleanFBO() {
  NodeTexture2D::cleanFBO();
  delete _vaoSplat; _vaoSplat = NULL;
  cleanOITData();
}
