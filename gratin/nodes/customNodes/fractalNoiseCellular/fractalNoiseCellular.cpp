// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>

#include "fractalNoiseCellular.h"

//QString FractalNoiseCellularNode::SHADER_PATH = QString("/disc/research/ideasAndNotes/coding-tests/silhouette-stylization/gratin-nodes/coherentStyle01/");
QString FractalNoiseCellularNode::SHADER_PATH = QString("/home/misnel/procedural-stylization/gratin/nodes/customNodes/fractalNoiseCellular/");

FractalNoiseCellularNode::FractalNoiseCellularNode(PbGraph *parent,NodeHandle *handle)
  : NodeTexture2D(parent,handle),
    _pBlend(QString(SHADER_PATH+"fractalNoiseCellular.vert"),
       QString(SHADER_PATH+"fractalNoiseCellular.frag")),
    _w(new FractalNoiseCellularWidget(this)),
    _nbElements(0),
    _sw(1),
    _sh(1),
    _acBuffer(0),
    _llBuffer(0),
    _headTex(0),
    _clBuffer(0) {

    _pBlend.addUniform("positionWMap");
    _pBlend.addUniform("depthMap");


}

FractalNoiseCellularNode::~FractalNoiseCellularNode() {
  cleanOITData();
}

void FractalNoiseCellularNode::apply() {
  // init viewport
  Glutils::setViewport(outputTex(0)->w(),outputTex(0)->h());

  std::cout << "Bonsoir: " << __LINE__ << "\n";

  _glf->glClearColor(0.0f,0.0f,0.0f,0.0f);

  _glf->glDisable(GL_DEPTH_TEST);
  _glf->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  _fbo.bind();

  std::cout << "Bonsoir: " << __LINE__ << "\n";

  _pBlend.enable();
  std::cout << "Bonsoir: " << __LINE__ << "\n";

  _pBlend.setUniformTexture("positionWMap",GL_TEXTURE_2D,inputTex(0)->id());
  _pBlend.setUniformTexture("depthMap",GL_TEXTURE_2D,inputTex(1)->id());

    std::cout << "Bonsoir: " << __LINE__ << "\n";

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




void FractalNoiseCellularNode::initOITData() {


}

void FractalNoiseCellularNode::cleanOITData() {

}

void FractalNoiseCellularNode::initFBO() {
  NodeTexture2D::initFBO();

  if(nbOutputs()>0) {
    _sw = outputTex(0)->w();
    _sh = outputTex(0)->h();
  } else {
    _sw = _sh = 1;
  }
  initOITData();
}

void FractalNoiseCellularNode::cleanFBO() {
  NodeTexture2D::cleanFBO();
  cleanOITData();
}
