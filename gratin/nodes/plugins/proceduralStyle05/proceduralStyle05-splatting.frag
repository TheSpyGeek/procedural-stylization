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
uniform sampler2D imgSplat;
uniform float alphaFactor;
uniform float splatDepthFactor;
uniform float rotation;

in vec2 texcoordCenter;
in vec4 positionWCenter;
in vec4 normalWCenter;
in vec4 shadingCenter;
in vec4 depthCenter;
in vec4 noiseCenter;

in mat4 mvp;
in mat4 mv;
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

float fragDepth = 1e+10;

vec3 modifDirection(in vec3 nV,in float i,in float size) {
  vec3 n = nV;
  n.y -= i/size;
  return normalize(n);
}


float W(float d,float s) {
  // simple gaussian for the weight
  return exp(-(d*d)/(2.*s*s));
}


vec4 computeHair() {

    const int size = 25;

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






bool hasToBeDisplayed(){
    return texture(noiseTex1, texcoordCenter).a > 0;
}

bool displayRotatedSplat(float radAngle, out vec4 color){

    if(radAngle > 1 || radAngle < -1){
        discard;
    }

    float angle = radAngle * 3.14;

    vec2 positionFromCenter = gl_PointCoord.xy - 0.5;


    float x = sin(angle)*positionFromCenter.y - cos(angle)*positionFromCenter.x;
    float y = cos(angle)*positionFromCenter.y + sin(angle)*positionFromCenter.x;

    // float x = 0.5*cos(radAngle)+ positionFromCenter.x;
    // float y = 0.5*sin(radAngle) + positionFromCenter.y;

    // y *= -1;


    vec2 coordNotRotated = vec2(x,y) + 0.5;
    // coordNotRotated.y = 1-coordNotRotated.y;

    color = vec4(coordNotRotated.x , coordNotRotated.y, 0, 1);



    color = texture(imgSplat, coordNotRotated.xy);
    // color.rgb = vec3(1,1,0);

    // color = vec4(coordNotRotated,0,1);

    /* has to be deleted this is just a test */
    if(coordNotRotated.x > 1. || coordNotRotated.x < 0.){
        // color = vec4(0,0,1,1);
        discard;
    }

    if(coordNotRotated.y > 1. || coordNotRotated.y < 0.){
        // color = vec4(0,0,1,1);
        discard;
    }



    // color = vec4(1,0,0,1);
    // color = texture(imgSplat, gl_PointCoord.xy);

    return color.a > 0;

}


vec4 displaySplatFromStroke(){
    vec4 projectedCenterPoint = mv*vec4(positionWCenter.xyz,1);
    float angle;

    vec4 splatColor = texture(imgSplat, gl_PointCoord.xy);

    float zAxisOfSplat = splatColor.x;

    float splatIMG = splatColor.a;
    if(splatColor.xyz == vec3(1)){
        splatIMG = 0;
    }

    vec3 color = texture(shadingMap, texcoordCenter).rgb;


    float alpha = alphaFactor*texture(noiseTex1, texcoordCenter).x;

    fragDepth = projectedCenterPoint.z + pow(zAxisOfSplat-0.5, splatDepthFactor);

    // if the center of the splat is in a positive point of the noise
    if(hasToBeDisplayed()){
        vec4 colorRotated;
        // return vec4(color*1.1, alpha);



        if(displayRotatedSplat(rotation, colorRotated)){
            fragDepth = projectedCenterPoint.z;
            // return vec4(colorRotated.rgb, alpha);
            // return vec4(colorRotated);
            return vec4(color, alpha);
            // return vec4(colorRotated.a,0,0,colorRotated.a);
            // return colorRotated;
        } else {
            discard;
        }
        // return vec4(shadingCenter.rgb*(1-zAxisOfSplat)*1.1, alphaFactor*splatIMG);
        // return vec4(color*(1-zAxisOfSplat)*1.1, alpha);
    } else {
        discard;
    }

}

vec4 displaySplatWithShadingColorAndVariableRadius(float radius){

    if(hasToBeDisplayed()){

        float test1 = 1.-step(radius,distance(gl_PointCoord.xy,vec2(0.5)));
        vec4 test2 = mvp*vec4(positionWCenter.xyz,1);
        fragDepth = test2.z/test2.w;

        return vec4(shadingCenter.rgb, texture(noiseTex1, texcoordCenter).x);
    } else {
        discard;
    }
}

vec4 displayHairsOnNoise(){
    if(hasToBeDisplayed()){
        return computeHair();

    } else {
        discard;
    }
}


vec4 computeStyle() {

    // return displaySplatWithShadingColorAndVariableRadius(0.5);
    return displaySplatFromStroke();
    // return displayHairsOnNoise();



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
