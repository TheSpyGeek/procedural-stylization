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
    _pNoise(QString(SHADER_PATH+"fractalNoiseCellular.vert"),
       QString(SHADER_PATH+"fractalNoiseCellular.frag")),
    _w(new FractalNoiseCellularWidget(this)) {

    _pNoise.addUniform("positionWMap");
    _pNoise.addUniform("depthMap");


}

FractalNoiseCellularNode::~FractalNoiseCellularNode() {
}

void FractalNoiseCellularNode::apply() {
  // init viewport
  Glutils::setViewport(outputTex(0)->w(),outputTex(0)->h());


  // openGL settings
  _glf->glClearColor(0.0f,0.0f,0.0f,0.0f);
  _glf->glDisable(GL_DEPTH_TEST);
  _glf->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // bind fbo
  _fbo.bind();


  // enable the pass
  _pNoise.enable();


  // send uniform to shader
  _pNoise.setUniformTexture("positionWMap",GL_TEXTURE_2D,inputTex(0)->id());
  _pNoise.setUniformTexture("depthMap",GL_TEXTURE_2D,inputTex(1)->id());
  _pNoise.setUniform1f("frequency",_w->frequency()->val());
  _pNoise.setUniform1f("style",_w->style()->val());
  _pNoise.setUniform1f("amplitude",_w->amplitude()->val());
  _pNoise.setUniform1i("nbSamples",_w->nbSamples()->val());

  // draw quad
  _unitSquareVao->bind();
  _unitSquareVao->drawArrays(GL_TRIANGLES,0,6);
  _unitSquareVao->unbind();
  _pNoise.disable();

  _glf->glBindTexture(GL_TEXTURE_2D, 0);

  FramebufferObject::unbind();
}
