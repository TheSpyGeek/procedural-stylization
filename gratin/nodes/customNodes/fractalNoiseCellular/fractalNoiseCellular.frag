// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#version 430 core


uniform sampler2D positionWMap;
uniform sampler2D depthMap;

uniform float depthDistance;


layout(location = 0) out vec4 rendering;





in vec2 texcoord;



void main() {

  rendering = vec4(depthDistance,0,0,1);
  // rendering = texture(positionWMap, texcoord) * depthDistance;
  // rendering.a = 1;



}
