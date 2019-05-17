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
uniform sampler2D colorMap;
uniform sampler2D depthMap;
uniform sampler2D noiseMap;
uniform sampler2D splatMap;
uniform sampler2D splatNormalMap;
uniform float splatDepthFactor;
uniform int rotateSplat;

in vec2 texcoordCenter;
in vec4 positionWCenter;
in vec4 normalWCenter;
in vec4 colorCenter;
in vec4 depthCenter;
in vec4 noiseCenter;

in mat4 mvp;
in mat4 mv;
in mat4 mvpInv;
in mat3 normalMat;
in mat3 normalMatInv;
in vec3 viewDir;

#define PI 3.1415926535898

/*** choice of rendering ***/
#define NOT_ROTATE 0
#define ROTATE 1
#define SHADING 2



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



bool hasToBeDisplayed(){
    return texture(noiseMap, texcoordCenter).a > 0;
}



// compute the color of the current pixel using the image as splat
vec4 displaySplat(){
    vec4 projectedCenterPoint = mv*vec4(positionWCenter.xyz,1);

    vec4 splatColor = texture(splatMap, gl_PointCoord.xy);

    float zAxisOfSplat = splatColor.x;

    float splatIMG = splatColor.a;
    if(splatColor.xyz == vec3(1)){
        splatIMG = 0;
    }

    vec3 color = texture(colorMap, texcoordCenter).rgb;


    float alpha = texture(noiseMap, texcoordCenter).x*splatIMG;

    fragDepth = projectedCenterPoint.z + pow(zAxisOfSplat-0.5, splatDepthFactor);

    // if the center of the splat is in a possitive point of the noise
    if(hasToBeDisplayed()){
        return vec4(color*(1-zAxisOfSplat)*1.1, alpha);
    } else {
        discard;
    }

}

/* return if the current pixel has to be displayed
depending on the rotation of the splat
*/
bool displayRotatedSplat(float radAngle, out vec4 color){

    float angle = radAngle * PI;

    vec2 positionFromCenter = gl_PointCoord.xy - 0.5;


    float x = sin(angle)*positionFromCenter.y - cos(angle)*positionFromCenter.x;
    float y = cos(angle)*positionFromCenter.y + sin(angle)*positionFromCenter.x;


    // multiply by -1 to inverse the image splat axis
    vec2 coordNotRotated = -1*vec2(x,y) + 0.5;

    color = texture(splatMap, coordNotRotated.xy);

    return color.a > 0;

}


// compute the color of the current pixel using the image as splat
// depending of the normal of the vertex
vec4 displaySplatWithRotation(){
    vec4 projectedCenterPoint = mv*vec4(positionWCenter.xyz,1);

    vec4 splatColor;

    float zAxisOfSplat = splatColor.x;

    float splatIMG = splatColor.a;
    if(splatColor.xyz == vec3(1)){
        splatIMG = 0;
    }

    vec3 color = texture(colorMap, texcoordCenter).rgb;

    float alpha = texture(noiseMap, texcoordCenter).x;


    // if the center of the splat is in a positive point of the noise
    if(hasToBeDisplayed()){

        vec2 normal = normalize(normalWCenter.xy);

        //  rotation of the splat
        float angle = 0.5*dot(normal, vec2(0,1)) + 0.5;

        if(normal.x < 0){
            angle *= -1;
        }


        if(displayRotatedSplat(angle, splatColor)){ // if the pixel has to be displayed
            fragDepth = projectedCenterPoint.z + pow(splatColor.x-0.5, splatDepthFactor);
            return vec4(color, alpha);
        } else {
            discard;
        }

    } else {
        discard;
    }

}

vec4 romainStyleTest01() {
  // simplest as possible: display splat

  vec4 noiseData = texture(noiseMap, texcoordCenter);  // noise inpulses
  if(noiseData.w<=0.) discard; // outside silhouettes --> discard

  vec4 splatData = texture(splatMap,gl_PointCoord.xy); // splat shape
  vec4 colorData = texture(colorMap, texcoordCenter);  // rend color (modulated with noise)


  float globalSplatOpacity = noiseData.x;
  float localSplatOpacity = splatData.w;
  float alpha = globalSplatOpacity*localSplatOpacity; // pixel alpha
  if(alpha<=1e-3) discard; // do not consider if opicity is too low


  vec4 splatColor = vec4(colorData.xyz,alpha);

  fragDepth = gl_FragCoord.z;

  /*** TEST
  float depth = texture(depthMap, texcoordCenter).x;
  float zmax = texture(splatNormalMap, texcoordCenter).x;

  fragDepth = depth*zmax;*/

  return splatColor;
}

vec4 romainStyleTest02() {
  // add rotation


  vec4 noiseData = texture(noiseMap, texcoordCenter);  // noise inpulses
  if(noiseData.w<=0.) discard; // outside silhouettes --> discard

  //// ROTATE
  vec2 coord = gl_PointCoord.xy;
  vec3 n = normalize(normalMat*normalWCenter.xyz);
  float a = atan(n.y,n.x)+PI/2.;
  vec2 T = coord-0.5;
  coord.x = cos(a)*T.x-sin(a)*T.y+0.5;
  coord.y = sin(a)*T.x+cos(a)*T.y+0.5;
  // END ROTATE

  vec4 splatData = texture(splatMap,coord); // splat shape
  vec4 colorData = texture(colorMap, texcoordCenter);  // rend color (modulated with noise)


  float globalSplatOpacity = clamp(noiseData.x*1.,0.,1.);
  float localSplatOpacity = splatData.w;
  float alpha = globalSplatOpacity*localSplatOpacity; // pixel alpha
  if(alpha<=1e-3) discard;
  //alpha *= alphaFactor;
  //if(alpha<0.01) discard; // do not consider if opacity is too low

  vec4 splatColor = vec4(colorData.xyz,alpha);
  //vec4 splatColor = vec4(texture(colorMap, texcoordCenter).xxx,alpha);

  //if(globalSplatOpacity*localSplatOpacity<=0.) splatColor = vec4(1,0,0,1);
  //else if(alphaFactor*globalSplatOpacity*localSplatOpacity<=0.01) splatColor = vec4(0,1,0,1);


  fragDepth = gl_FragCoord.z;// texcoordCenter.x;//

  return splatColor;
}

vec4 romainStyleTest03() {
  // add shading


  vec4 noiseData = texture(noiseMap, texcoordCenter);  // noise inpulses
  if(noiseData.w<=0.) discard; // outside silhouettes --> discard

  //// ROTATE
  vec2 coord = gl_PointCoord.xy;
  vec3 n = normalize(normalMat*normalWCenter.xyz);
  vec2 pn = length(n.xy)>1e-10 ? normalize(n.xy) : n.xy;
  float a = atan(n.y,n.x)+PI/2.;
  vec2 T = coord-0.5;
  coord.x = cos(a)*T.x-sin(a)*T.y+0.5;
  coord.y = sin(a)*T.x+cos(a)*T.y+0.5;
  // END ROTATE

  vec4 splatData = texture(splatMap,coord); // splat shape
  vec4 colorData = texture(colorMap, texcoordCenter);  // rend color (modulated with noise)

  // SHADING
  vec4 normalData = texture(splatNormalMap,coord);
  vec3 sl = normalize(vec3(-0.8,0.8,0.5)); // should be the same dir as for the base shading...
  vec3 sn = vec3(cos(a)*normalData.x-sin(a)*normalData.y,
  		 sin(a)*normalData.x+cos(a)*normalData.y,
  		 normalData.z); // rotated too to account for the splat rotation
  colorData.xyz *= max(dot(sn,sl),0.)*2.;
  // END SHADING

  float globalSplatOpacity = noiseData.x;
  float localSplatOpacity = splatData.w;
  float alpha = globalSplatOpacity*localSplatOpacity; // pixel alpha
  if(alpha<=1e-3) discard; // do not consider if opacity is too low


  vec4 splatColor = vec4(colorData.xyz,alpha);

  fragDepth = gl_FragCoord.z;

  return splatColor;
}



vec4 computeStyle() {
  // return romainStyleTest02();

    // return displaySplatFromStroke();
    if(rotateSplat == ROTATE){
        return romainStyleTest02();
    } else if(rotateSplat == NOT_ROTATE){
        return romainStyleTest01();
    } else if(rotateSplat == 2){
        return romainStyleTest03();
    }
}


void main() {
  vec4 test = vec4(1,0,0,1);

  vec2 centeredSplatCoord = gl_PointCoord.xy-0.5;

  //if(length(centeredSplatCoord)>0.5) discard;

  // compute the color and depth
  vec4 color = computeStyle();

  //if(color.w<0.01) discard;


  uint nodeIdx = atomicCounterIncrement(nextNodeCounter);

  // adding in the linked list the color and depth computed
  if( nodeIdx < maxNodes ) {

    uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);

    // Here we set the color and depth of this new node to the color
    // and depth of the fragment.  The next pointer, points to the
    // previous head of the list.
    nodes[nodeIdx].color = color;
    nodes[nodeIdx].depth = fragDepth;
    //nodes[nodeIdx].depth = gl_FragCoord.z;

    nodes[nodeIdx].next = prevHead;

  }

  rendering = test;
}
