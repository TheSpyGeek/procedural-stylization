// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#version 430 core

//layout (early_fragment_tests) in;

layout(location = 0) out vec4 rendering;

#define MAX_FRAGMENTS 200

struct NodeType {
  vec4 color;
  float depth;
  uint next;
};

layout( binding = 0, r32ui) uniform uimage2D headPointers;
layout( binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;
layout( binding = 0, std430 ) buffer linkedLists {
  NodeType nodes[];
};

uniform uint maxNodes;
uniform sampler2D matrices;
uniform sampler2D positionWMap;
uniform sampler2D normalWMap;
uniform sampler2D shadingMap;
uniform sampler2D depthMap;
uniform sampler2D noiseTex1;
uniform int size;

in vec2 texcoordCenter;
in vec4 positionWCenter;
in vec4 normalWCenter;
in vec4 shadingCenter;
in vec4 depthCenter;
in vec4 noiseCenter;

in mat4 mvp;
in mat4 mvpInv;
in mat3 normalMat;
in mat3 normalMatInv;

#define PI 3.1415926535898

vec2 ts = textureSize(positionWMap,0);

vec3 hash33(vec3 p) {
  vec3 q = vec3(dot(p,vec3(127.1,311.7,74.7)),
		dot(p,vec3(269.5,183.3,246.1)),
		dot(p,vec3(113.5,271.9,124.6)));
  return fract(sin(q)*43758.5453123)*2.-1.;
}

float gnoise(in vec3 x) {
  vec3 p = floor(x);
  vec3 f = fract(x);
  vec3 m = f*f*(3.-2.*f);

  return mix(
	     mix(mix(dot(hash33(p+vec3(0.,0.,0.)),f-vec3(0.,0.,0.)),
		     dot(hash33(p+vec3(1.,0.,0.)),f-vec3(1.,0.,0.)),m.x),
		 mix(dot(hash33(p+vec3(0.,1.,0.)),f-vec3(0.,1.,0.)),
		     dot(hash33(p+vec3(1.,1.,0.)),f-vec3(1.,1.,0.)),m.x),m.y),
	     mix(mix(dot(hash33(p+vec3(0.,0.,1.)),f-vec3(0.,0.,1.)),
		     dot(hash33(p+vec3(1.,0.,1.)),f-vec3(1.,0.,1.)),m.x),
		 mix(dot(hash33(p+vec3(0.,1.,1.)),f-vec3(0.,1.,1.)),
		     dot(hash33(p+vec3(1.,1.,1.)),f-vec3(1.,1.,1.)),m.x),m.y),m.z)*.5+.5;
}


vec3 modifDirection(in vec3 nV,in float i,in float size) {
  vec3 n = nV;
  n.y -= i/size;
  return normalize(n);
}


float W(float d,float s) {
  // simple gaussian for the weight
  return exp(-(d*d)/(2.*s*s));
}

float fragDepth = 1e+10;

vec4 computeStyle() {

  vec4 col = vec4(vec3(0.),1.);
  vec3 pW = positionWCenter.xyz;
  vec3 nW = normalWCenter.xyz;
  vec2 fc = gl_FragCoord.xy; // current pixel coordinate

  float sig = 1.2; // sigma for distance to the curve
  float vs = 0.005; // the volumetric step
  float d = 1e+10;

  // ideally, we should make sure that we never go beyond the size of the square...
  int ss = size;//int(float(size)*noiseCenter.x);
  float s = 0.;

  for(int i=0;i<=ss;++i) {
    vec3 nV = normalMat*nW;
    nV = modifDirection(nV,float(i),float(ss));
    vec3 nWN = normalMatInv*nV;

    vec4 pW = vec4(pW+float(i)*nWN*vs,1.); // modified pos in world space
    vec4 pP = mvp*pW; // position in clip space
    vec2 pS = .5*((pP.xy/pP.w)+1.)*ts; // normalized position in screen space

    float dt = distance(pS,fc);
    if(dt<d) {
      s = 1.-float(i)/float(ss+ss/1.5);
      d = dt;
      fragDepth = pP.z/pP.w;
    }
  }

  float w = W(d,sig*s);
  vec4 c = vec4(shadingCenter.xyz+vec3(noiseCenter.x-.5),1.);
  return vec4(c.xyz,w*noiseCenter.y);
}

void main() {
  vec4 test = vec4(1,0,0,1);

  vec2 splatCoord = gl_PointCoord.xy;
  vec2 centeredSplatCoord = gl_PointCoord.xy-0.5;

  if(length(centeredSplatCoord)>0.5) discard;

  vec4 color = computeStyle();

  if(color.w<0.01) discard;


  uint nodeIdx = atomicCounterIncrement(nextNodeCounter);

  if( nodeIdx < maxNodes ) {
    test = vec4(0,1,0,1);

    uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);

    // Here we set the color and depth of this new node to the color
    // and depth of the fragment.  The next pointer, points to the
    // previous head of the list.
    nodes[nodeIdx].color = color;
    nodes[nodeIdx].depth = fragDepth;
    //nodes[nodeIdx].depth = gl_FragCoord.z;

    nodes[nodeIdx].next = prevHead;

  }

  // should not be necessary
  //rendering = computeStyle();
  rendering = test;
  //vec2 c = vec2(gl_PointCoord.x,1.-gl_PointCoord.y); // position in splat space
  //vec4 fcol = clamp(col,vec4(0.),vec4(1.));
  //float ww = 10*(1.-depthCenter.x);
  //fcol.w *= noiseCenter.x;
  //rendering = fcol;//+fcol*ww;///float(size);
  //rendering = vec4(vec3(W(d,sig*s)),1);
  //rendering = t;//vec4(gl_FragCoord.xy/ts,0,1);
}
