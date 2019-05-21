// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#version 430 core

#define EPS 1e-5
#define PI 3.1415926535898

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 coord;
layout(location = 2) in vec2 splatpos;

uniform sampler2D matrices;
uniform sampler2D positionWMap;
uniform sampler2D normalWMap;
uniform sampler2D colorMap;
uniform sampler2D depthMap;
uniform sampler2D noiseMap;
uniform sampler2D splatMap;
uniform sampler2D splatNormalMap;
uniform float splatSize;
uniform float rotateSplat;

out vec2 splatCoord;
out vec2 anchorCoord;
out vec4 anchorPositionW;
out vec4 anchorNormalW;
out vec4 anchorColor;
out vec4 anchorDepth;
out vec4 anchorNoise;

out mat4 Mmat; // model matrix 
out mat4 Vmat; // view matrix 
out mat4 Pmat; // proj matrix 
out mat3 Nmat; // normal matrix 

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

// model, view, projection and normal matrices
mat4 model = mat4(m0,m1,m2,m3);
mat4 view = mat4(v0,v1,v2,v3);
mat4 proj = mat4(p0,p1,p2,p3);
mat3 nmat = transpose(inverse(mat3(view*model)));


vec3 rotate3D(in vec3 v,in vec3 axis,in float angle,in vec3 c) {
  // rotation of v around c, along particular axis and angle 
  float ca  = cos(angle);
  float sa = sin(angle);
  float cam = 1.-ca;
  vec3 u = axis;
  
  vec3 c1 = vec3(ca+u.x*u.x*cam, u.x*u.y*cam+u.z*sa, u.x*u.z*cam-u.y*sa);
  vec3 c2 = vec3(u.x*u.y*cam-u.z*sa, ca+u.y*u.y*cam, u.y*u.z*cam+u.x*sa);
  vec3 c3 = vec3(u.x*u.z*cam+u.y*sa, u.y*u.z*cam-u.x*sa, ca+u.z*u.z*cam);
  
  mat3 R = mat3(c1,c2,c3);
  
  return R*(v-c) + c;
}

vec2 rotate2D(in vec2 v,in float angle,in vec2 c) {
  // rotation of v around c with a particular angle
  vec2 T = v-c;
  return vec2(cos(angle)*T.x-sin(angle)*T.y,
	      sin(angle)*T.x+cos(angle)*T.y) + c;
}


void main() {
  // available data at splat center 
  vec2 ancCoord = splatpos*.5+.5; 
  vec4 ancPosW = texture(positionWMap,ancCoord);
  vec4 ancNorW = texture(normalWMap,ancCoord);
  vec4 ancColor = texture(colorMap,ancCoord);
  vec4 ancNoise = texture(noiseMap,ancCoord);
  vec4 ancDepth = texture(depthMap,ancCoord);

  // discard splats outside the object (or without noise impulses)
  if(length(ancNorW)<EPS || ancNoise.w<EPS || ancNoise.x<EPS) {
    gl_Position = vec4(0.);
    return;
  }
  
  // compute vertex position (=vertex + z of projected surface)
  vec3 vp = vec3(vertex,ancDepth);
  vec3 vn = normalize(nmat*ancNorW.xyz);
  
  // scale splat (ancNoise.z is mean curvature^2)
  vp.xy = splatpos+splatSize*(1.-ancNoise.z)*normalize(vertex-splatpos);
  
  // rotate vertex to align with cam space normal
  vp.xy = rotate2D(vp.xy,atan(vn.y,vn.x)-PI/2.,splatpos.xy);

  // rotate vertex along plane axis (3D)
  vp = rotate3D(vp,normalize(vec3(-vn.y,vn.x,0.)),rotateSplat*asin(vn.z),vec3(splatpos,ancDepth));

  gl_Position  = vec4(vp,1.);

  // output stuff to the fragment shader 
  splatCoord = coord; // texcoord in the splat 
  anchorCoord = ancCoord; // splat coord in the image 
  anchorPositionW = ancPosW; // 3D pos at splat center 
  anchorNormalW = ancNorW; // 3D normal at splat center 
  anchorColor = ancColor; // color at splat center 
  anchorDepth = ancDepth; // obj depth at splat center 
  anchorNoise = ancNoise; // noise (opacity) at splat center
  
  Mmat = model; // model matrix 
  Vmat = view; // view matrix 
  Pmat = proj; // proj matrix 
  Nmat = nmat; // normal matrix 
}
