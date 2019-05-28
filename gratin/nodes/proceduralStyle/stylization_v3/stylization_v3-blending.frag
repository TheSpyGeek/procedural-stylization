// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#version 430 core

#define BLEND_MODE_0
#define MAX_FRAGMENTS 300

//layout (early_fragment_tests) in;

layout(location = 0) out vec4 rendering;

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

in vec2 texcoord;

uniform sampler2D image;
uniform float gammaBlend;

NodeType frags[MAX_FRAGMENTS];
int count = 0;

void loadFrags() {
  // Get the index of the head of the list
  uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;

  // Copy the linked list for this fragment into an array
  while( n != 0xffffffff && count < MAX_FRAGMENTS) {
    frags[count] = nodes[n];
    n = frags[count].next;
    count++;
  }
}

void bubbleSort() {
  int j, i;
  NodeType tempNode;
  for(i=0;i<count-1;i++) {
    for(j=0;j<count-i-1;j++) {
      if(frags[j].depth < frags[j+1].depth) {
	tempNode = frags[j];
	frags[j] = frags[j+1];
	frags[j+1] = tempNode;
      }
    }
  }
}


vec4 backToFrontOver(in vec4 backcolor) {
  vec4 color = backcolor;
  for(int i=0;i<count;i++) {
    float a = clamp(pow(frags[i].color.a,1./gammaBlend),0.,1.);
    color = mix(vec4(color.xyz,1.),vec4(frags[i].color.xyz,a), a);
  }
  return color;
}

vec4 over(in vec4 c1,in vec4 c2) {
  float a = (c1.a+(1.-c1.a)*c2.a);
  return vec4((c1.xyz*c1.a + c2.xyz*(1.-c1.a)*c2.a)/a,a);
}

vec4 frontToBackOver(in vec4 backcolor) {
  vec4 color = vec4(0.);
  for(int i=count-1;i>=0;i--) {
    NodeType n = frags[i];
    color = over(color,vec4(n.color.xyz,clamp(pow(n.color.a,1./gammaBlend),0.,1.)));
  }
  return over(color,backcolor);
}

vec4 frontToBackOverDepthTest(in vec4 backcolor) {  
  vec4 color = vec4(0.);

  if(count<=0) return backcolor;
  float d1 = -1.;//frags[count-1].depth;
  
  for(int i=count-1;i>=0;i--) {
    NodeType n = frags[i];
    float d2 = n.depth;
    float d = d2-d1;
    float s = 1.;
    float w =  exp((d*d)/(s*s))*gammaBlend;
    float a = clamp(pow(n.color.a,1./w),0.,1.);
    d1 = d2;
    color = over(color,vec4(n.color.xyz,a));
  }
  return over(color,backcolor);
}


void main() {
  loadFrags();
  bubbleSort();
 vec4 col = texture(image,texcoord);
  vec4 color = mix(vec4(0.5,0.5,0.5,1.0),vec4(col.xyz,1.),col.w);
 
#ifdef BLEND_MODE_0
  color = frontToBackOver(color);
#endif

#ifdef BLEND_MODE_1
  color = backToFrontOver(color);
#endif

#ifdef BLEND_MODE_2
  color = frontToBackOverDepthTest(color);
#endif

#ifdef BLEND_MODE_3
  // weighted average
  vec4 splatCol = vec4(0.);
  float sumAlpha = 0.;
  for(int i=0;i<count;i++) {
    float w = pow(frags[i].color.w,1./50.2);
    splatCol += vec4(frags[i].color.xyz,1.)*w;
    sumAlpha += w;
  }
  float blendAlpha = 0.;
  if(count>0) {
    splatCol = splatCol/sumAlpha;
    blendAlpha = clamp((sumAlpha/float(count)),0.,1.);
    color = mix(texture(image,texcoord),splatCol,blendAlpha);
  }
#endif
  

  rendering = color;
}
