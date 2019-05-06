// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#version 430 core

#define MAX_FREQ 15


uniform sampler2D positionWMap;
uniform sampler2D depthMap;
uniform sampler2D depthMinMax;

uniform float amplitude;
uniform int nbSamples;
uniform float frequency;
uniform float style;


layout(location = 0) out vec4 rendering;



const int nboctaves = 1;
const float persistence = 0.000;



in vec2 texcoord;



vec3 hash33(vec3 p) {
	vec3 q = vec3(	dot(p,vec3(127.1,311.7,74.7)),
							dot(p,vec3(269.5,183.3,246.1)),
							dot(p,vec3(113.5,271.9,124.6)));
	return fract(sin(q)*43758.5453123);
}

float wnoise(in vec3 x) {
	vec3 p = floor(x);
	vec3 f = fract(x);

	float id = 0.0;
	vec2 res = vec2( 100.0 );

	for( int k=-1; k<=1; k++ )
		for( int j=-1; j<=1; j++ )
			for( int i=-1; i<=1; i++ ) {
				vec3 b = vec3( float(i), float(j), float(k) );
				vec3 r = vec3( b ) - f + hash33( p + b );
				float d = dot( r, r );

				if( d < res.x ) {
					id = dot( p+b, vec3(1.0,57.0,113.0 ) );
					res = vec2( d, res.x );
				} else if( d < res.y ) {
            res.y = d;
        }
    }

    return sqrt(res.x);
}

float fnoise(in vec3 p,in float amplitude,in float frequency,in float persistence, in int nboctaves) {
	float a = amplitude;
	float f = frequency;
	float n = 0.0;

	for(int i=0;i<nboctaves;++i) {
		n = n+a*wnoise(p*f);
		f = f*2.;
		a = a*persistence;
	}

	return n;
}

vec4 noiseColor(vec2 textureCoord, float mult, in float frag_scale){
	vec2 ps = .5*(1./textureSize(positionWMap,0).xy); // half pixel size
	vec2 psStep  = ps/float(nbSamples+1);
	float n = 0.;
	float nb = 0.;
	float a = 0.;

	float z;
	float f;

	// float myStyle = frequency/30;

	for(int i=-nbSamples;i<=nbSamples;++i) {
		for(int j=-nbSamples;j<=nbSamples;++j) {
			vec2 coord = textureCoord+vec2(float(i),float(j))*psStep;
			vec4 pos = texture(positionWMap,coord);
			vec4 data = mult * pos/frag_scale;

			z = texture(depthMap, texcoord).x;
			f = 1.2*(frequency/MAX_FREQ);

			n +=  1.-smoothstep(0.,style*f,fnoise(data.xyz,amplitude,frequency,persistence,nboctaves));
			a += data.w;
			nb += 1.;
		}
	}

	float finalA = a/nb;
	float finalN = n/nb;

	return vec4(vec3(finalN), finalA);
}

void main() {
	float zmax = texture(depthMinMax, texcoord).x;

	float depth = texture(depthMap, texcoord).x;

	float z = log2(depth);
	float s = z-floor(z);
	// float s = fract(depth);

	//float s = 0.3*fract(depth);

	float frag_scale = pow(2.0, floor(z));
	// float frag_scale = pow(2.0, 2*floor(z));

	// octave weight
	float alpha1 = s/2.0;
	float alpha2 = 1.0/2.0 - s/6.0;
	float alpha3 = 1.0/3.0 - s/6.0;
	float alpha4 = 1.0/6.0 - s/6.0;

	vec4 oct1 = alpha1 * noiseColor(texcoord, 1.0, frag_scale);
	vec4 oct2 = alpha2 * noiseColor(texcoord, 2.0, frag_scale);
	vec4 oct3 = alpha3 * noiseColor(texcoord, 4.0, frag_scale);
	vec4 oct4 = alpha4 * noiseColor(texcoord, 8.0, frag_scale);

	vec4 n = oct1+oct2+oct3+oct4;
	// vec4 n = oct1;

	//finalN = 1.-smoothstep(0.,style,finalN);

	rendering = n;
	// rendering = noiseColor(texcoord, 1.0, 1.0);
}
