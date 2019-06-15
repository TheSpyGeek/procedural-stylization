# Introduction

## Context/Backgroung
* Artists want to stylized 3D scenes
* Having a 2D aspect
* good rendering in animation as video games

## Problem statement (Temporal Coherence)
* The trinity (motion coherence, temporal continuity, flatness)
* wide variety of possible style

### procedural noise

* continuous and non-perdiodic
* Noise is a stationary and normal random process
* In computer graphics we use Gabor Noise, Perlin Noise, Worley Noise, etc.


## Scientific Approach and Investigative Method and Results


### Content of this report

* Stylize 3D Models
* ensure temporal coherence
* widest variety of style

# State of the art

As shown in the state of the art Report on Temporal Coherence for stylized Animations by Benard et al. [State of the Art Report] stylizing is done in two differents space in a pipeline rendering: Image Space and Object space and there are two main way to rendered styles: Texture mapping and Stroke based stylization. As explained in the work of Benard et al.


* Description of all axes
* Each of these axes offer a way to do stylization and permit to do different styles.
    * Stroke based also called splatting can easy make paint brushes style or hand drawing effect [vergne_implicit_2011].
    * Watercolor can be done with texture mapping [Dynamic Solid Textures].


**< inserer axe orthogonaux >**



## Object space

### Texture based



### Mark based

## Image space



* Stylizing in image space is mainly used to do non-photorealistic rendering on videos and images. To do this they either extract important lines or they compute point distribution to anchor textures or strokes.

* [Image and Video-Based Artistic Stylisation, 2013] apply stroke but suffer of temporal continuity and it can only stylize with paint brush strokes
* [vergne_implicit_2011] use convolution of points to have hand drawing effects. Limited to hand drawing effect.
* [MNPR Framework] permit control on the whole image as filtering to do effects like paper effect.
* [Meier's painterly algorithm] paint strokes in image space strokes are anchored to a point distribution **on 3D Object** => **motion coherence**. It is the closest work from ours.

## Object space

* OverCoat => splatting 2D image of the surface of the object, **manually placed** the strokes, good **motion coherence** but **flatness** not that good.
* [MNPR Framework] let the control to the user to change himselft the stylization at some point of the 3D model. For this, they use infos on vertices (RGBA).

## Texture mapping

* [MNPR Framework] allow to do a global operation on the image like changing the colors according to a procedural noise.
* [Dynamic Solid Textures] infinite zoom effect =>  improve **Temporal continuity**,  (possible styles : watercolor, charcoal, stippling)

## Stroke based

Also called splatting.

* [Meier's painterly algorithm] drawing strokes in image-space permit a good flatness.
* [Active Strokes, Coherent...] convolution of strokes in this lines (like arcs, overdrawn lines, ...)  

# Realisation

## Overview

* take the better of the two worlds between object space and image space
    * image space with G-Buffer (cf: Bléron)
* splatting because texture based is very limited (cf: Pierre bénard: Dynamic Solid Textures... )
* problem of anchor point => procedural noise
    * easy to manipulate
    * easy to implement
    * coherent during the motion
* splatting => splat properties:
    * size
    * density
    * opacity
    * orientation


## Procedural noise and fractalisation

* Worley and Perlin +
* PN controls the density of splat and a part of their opacity +
* threshold in order to have less splats in the screen
* Worley
* explain Worley
    * adapt the threshold depending on the distance from the camera
    * adapt the threshold depending on the frequency of the noise


* fractalization (cf: Pierre Bénard)
    * quasi constant size of splats => ++ flatness
    * infinite zoom effect => ++ temporal continuity
    * problem:
        * alter original pattern
        * impossible to have certain pattern (example: damier)

## Splatting

* apply 2D image on the screen
    * it can be created proceduraly
    * paint brushes
    * hairs
    * leaves
    * **control size of the splat**
    * **control their rotation**

* blending:

    * alpha blending (cf: overcoat follow up)
    * **control transparency**


## Stylization

* talk about how can we make a specific style
* in order to make a specific style the user has to manipulate some parameters:
    * splat
        * content (hairs, leaves, paint strokes)
        * size
        * orientation
    * procedural noise
        * frequency
        * threshold
    * gamma blending


Some examples:

* realist hair:
    * small point for the noise (Worley)
    * high density
    * hair as splats
    * size of splat: medium (and varying on the object)
    * shading in order to add 3D impression
* painted style:
    * small point for the noise
    * low density
    * brushes strokes as splats
    * size of splat: high


# Technical implementation

* use Gratin: pipeline rendering software

* NOT NEED spécific structure of rendering
* can be done in compositing

* splatting principle on GPU

* algo fractalization
* aliasing +

* order independent transparency on GPU
* blending with the distance from the camera

# Results

* all styles possible to render
    * hairs
    * leaves
    * paintbrushes strokes
    * pointillism
    * clouds ?

# go further and discussion

* depth in the splat itself
* watercolor
* adapt with the flow motion





#### VRAC

* Our method does not permit to do filtering on the whole image but it can be done after our step of stylizing.
* compute anchor point permit to avoid lines extraction and segmentation of the object
* Does our method permit to highlight silhouettes?
