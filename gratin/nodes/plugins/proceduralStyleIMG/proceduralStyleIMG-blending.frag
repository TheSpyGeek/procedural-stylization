// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#version 430 core

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
uniform int nbPixelAverage;

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

  vec4 color = vec4(0);//.5, 0.5, 0.5, 1.0);
  for( int i = 0; i < count; i++ )
    {
      color = mix( color, frags[i].color, frags[i].color.a);
    }

    rendering = color;

  // if(count>0)
  //    rendering = mix(color,frags[count-1].color,frags[count-1].color.a);
  //  else
  //    rendering = vec4(color);

  //rendering = texture(image,texcoord);

  // vec4 d = texture(image,texcoord);

  // if(d.w>1.)
  //   d = d/d.w;

  // d.xyz /= 1.5;
  // //d = d/2.;
  // rendering = d;
  // //rendering = vec4(1,0,0,1);
}
