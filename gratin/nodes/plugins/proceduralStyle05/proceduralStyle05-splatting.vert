// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#version 430 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec4 color;

uniform sampler2D matrices;
uniform sampler2D positionWMap;
uniform sampler2D normalWMap;
uniform sampler2D shadingMap;
uniform sampler2D depthMap;
uniform sampler2D noiseTex1;
uniform float splatSize;

out vec2 texcoordCenter;
out vec4 positionWCenter;
out vec4 normalWCenter;
out vec4 shadingCenter;
out vec4 depthCenter;
out vec4 noiseCenter;

out mat4 mvp;
out mat4 mv;
out mat4 mvpInv;
out mat3 normalMat;
out mat3 normalMatInv;
out vec3 viewDir;

// LOAD MATRICES
vec4 m0 = texelFetch(matrices, ivec2(0, 0), 0);
vec4 m1 = texelFetch(matrices, ivec2(1, 0), 0);
vec4 m2 = texelFetch(matrices, ivec2(2, 0), 0);
vec4 m3 = texelFetch(matrices, ivec2(3, 0), 0);

vec4 v0 = texelFetch(matrices, ivec2(0, 1), 0);
vec4 v1 = texelFetch(matrices, ivec2(1, 1), 0);
vec4 v2 = texelFetch(matrices, ivec2(2, 1), 0);
vec4 v3 = texelFetch(matrices, ivec2(3, 1), 0);

vec4 p0 = texelFetch(matrices, ivec2(0, 2), 0);
vec4 p1 = texelFetch(matrices, ivec2(1, 2), 0);
vec4 p2 = texelFetch(matrices, ivec2(2, 2), 0);
vec4 p3 = texelFetch(matrices, ivec2(3, 2), 0);

// model, view and projection matrices
mat4 model = mat4(m0,m1,m2,m3);
mat4 view = mat4(v0,v1,v2,v3);
mat4 proj = mat4(p0,p1,p2,p3);

float hash12(vec2 p) {
  return fract(sin(dot(p,vec2(12.9898,78.233)))*43758.5453123);
}

void main() {
  //float test = 0.99; // to be removed
  vec2 p = vertex;  // vertex coord
  vec2 c = p*.5+.5; // texture coordinates
  mat4 projected = proj*view*model;
  vec4 posW = texture(positionWMap,c);
  vec4 nW = texture(normalWMap,c);




  float l = 1.;
  vec4 nC = texture(noiseTex1,c);
  //if(nC.y<0.8) l = 0.;
  //nC.x = (nC.x-0.8)*5.;

  // the z position of the splat should be the one of the (projected) object
  //gl_Position  = vec4(p,(projected*posW).z,length(nW.xyz));
  vec4 pP = projected*posW;

  gl_Position  = vec4(p.xy,pP.z/pP.w,length(nW.xyz)*l);
  //gl_PointSize = 11.0;
  // gl_PointSize = 40.;//texture(noiseTex1,c).x*40.0;
  gl_PointSize = splatSize;

  shadingCenter = texture(shadingMap,c);
  positionWCenter = posW;
  normalWCenter = nW;
  depthCenter = texture(depthMap,c);
  noiseCenter = nC;
  texcoordCenter = c;
  mvp = projected;
  mv = view*model;
  mvpInv = inverse(projected);
  normalMat = transpose(inverse(mat3(view*model)));
  normalMatInv = inverse(normalMat);

  mat4 inverseView = inverse(view);
  viewDir = vec3(inverseView[2][0], inverseView[2][1], inverseView[2][2]);

}
