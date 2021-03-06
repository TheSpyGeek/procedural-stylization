// This file is part of Gratin, a programmable Node-based System 
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#version 330

layout(location = 0) out vec4 outbuff;

in vec2 texcoord;

uniform mat4 mdv;
uniform mat4 proj;

vec4 getValues(in mat4 m) {
  int c = min(int(floor(texcoord.x*4.)),3);
  return m[c].xyzw;
}

void main() {
  if(texcoord.y>=0.5)
    outbuff = getValues(mdv);
  else
    outbuff = getValues(proj);
}
