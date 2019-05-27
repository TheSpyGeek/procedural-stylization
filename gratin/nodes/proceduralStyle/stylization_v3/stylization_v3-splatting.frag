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

//layout(location = 0) out vec4 rendering;

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
uniform sampler2D colorMap;
uniform sampler2D depthMap;
uniform sampler2D noiseMap;
uniform sampler2D splatMap;
uniform sampler2D splatNormalMap;
uniform float splatDepthFactor;
uniform float rotateSplat;

in vec2 splatCoord;
in vec2 anchorCoord;
in vec4 anchorPositionW;
in vec4 anchorNormalW;
in vec4 anchorColor;
in vec4 anchorDepth;
in vec4 anchorNoise;

in mat4 Mmat; // model matrix 
in mat4 Vmat; // view matrix 
in mat4 Pmat; // proj matrix 
in mat3 Nmat; // normal matrix 

#define PI 3.1415926535898

vec2 ts = textureSize(positionWMap,0);

// ******* USEFUL FUNCTIONS (rotation/noises/etc) ******* 
vec3 rotateAxisAngle(in vec3 v,in vec3 axis,in float angle) {
  float ca  = cos(angle);
  float sa = sin(angle);
  float cam = 1.-ca;
  vec3 u = axis;
  
  vec3 c1 = vec3(ca+u.x*u.x*cam, u.x*u.y*cam+u.z*sa, u.x*u.z*cam-u.y*sa);
  vec3 c2 = vec3(u.x*u.y*cam-u.z*sa, ca+u.y*u.y*cam, u.y*u.z*cam+u.x*sa);
  vec3 c3 = vec3(u.x*u.z*cam+u.y*sa, u.y*u.z*cam-u.x*sa, ca+u.z*u.z*cam);
  
  mat3 R = mat3(c1,c2,c3);
  
  return R*v;
}


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


// ******* STYLE FUNCTIONS ******* 
void styleTest01(out vec3 fragColor,out float fragOpacity, out float fragDepth) {
  // SIMPLE TEST: colored splat with a careful handle of noise opacity to discard fragments 
  vec4 ancNoise = anchorNoise; if(ancNoise.w<=0.) discard; // outside silhouettes 
  vec4 ancColor = anchorColor;

  vec4 splatData = texture(splatMap,splatCoord);
  
  float opacity = splatData.w*clamp(ancNoise.x,0.,1.); if(opacity<=1e-3) discard; // not enough opaque

  fragColor = anchorColor.xyz;//*splatCoord.y;
  fragOpacity = opacity;
  fragDepth = gl_FragCoord.z;
}


// ******* MAIN FUNCTION DEALS WITH THE CHAINED LIST ******* 
void main() {
  // compute color and depth
  vec3 fragColor = vec3(0.);
  float fragOpacity = 0.;
  float fragDepth = 1e+10;

  styleTest01(fragColor,fragOpacity,fragDepth);

  uint nodeIdx = atomicCounterIncrement(nextNodeCounter);

  // adding in the linked list the color and depth computed
  if( nodeIdx < maxNodes ) {

    uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);

    // Here we set the color and depth of this new node to the color
    // and depth of the fragment.  The next pointer, points to the
    // previous head of the list.
    nodes[nodeIdx].color = vec4(fragColor,fragOpacity);
    nodes[nodeIdx].depth = fragDepth;
    nodes[nodeIdx].next = prevHead;
  }

  //rendering = vec4(fragColor,fragOpacity);
}
