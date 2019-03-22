// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#version 430 core

#define BLEND_MODE_1

#define MAX_FRAGMENTS 200

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
  for(i = 0; i < count - 1; i++)
    {
      for(j = 0; j < count - i - 1; j++)
	{
	  if(frags[j].depth < frags[j+1].depth)
	    {
	      tempNode = frags[j];
	      frags[j] = frags[j+1];
	      frags[j+1] = tempNode;
	    }
	}
    }
}

void main() {
  loadFrags();
  bubbleSort();

  vec4 color = vec4(texture(image,texcoord).xyz,1.);
  //color.xyz *= 0.7;
  color = vec4(0,0,0,1);

#ifdef BLEND_MODE_1
  // rescale alphas before the blend
  for(int i=0;i<count;i++) {
    float a = clamp(frags[i].color.a*10.,0.,1.);
    color = mix(vec4(color.xyz,1),vec4(frags[i].color.xyz,a*1), a);
    //color = frags[count-1].color*a*100;
    //color.a = a;
  }
  //if(count==0) color = vec4(1,0,0,1);
#endif

#ifdef BLEND_MODE_12
  // rescale alphas before the blend
  for(int i=0;i<count;i++) {
    float a = clamp(frags[i].color.a*1.,0.,1.);
    color = mix(color,vec4(frags[i].color.xyz,a), a);
    //color = frags[count-1].color;
  }
#endif

#ifdef BLEND_MODE_2
  // rescale alphas after the blend
  if(count>0) {
    vec4 splatCol = vec4(frags[0].color.xyz,1.);
    float alpha = frags[0].color.w;
    for(int i=1;i<count;i++) {
      splatCol = mix( splatCol, vec4(frags[i].color.xyz,1.), frags[i].color.a);
      alpha += frags[i].color.a;
    }

    alpha = clamp(alpha*2.,0.,1.);
    color = mix(texture(image,texcoord),splatCol,alpha);
  }
#endif

#ifdef BLEND_MODE_3
  // weighted average
  vec4 splatCol = vec4(0.);
  float sumAlpha = 0.;
  for(int i=0;i<count;i++) {
    float w = frags[i].color.w*200.;
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
