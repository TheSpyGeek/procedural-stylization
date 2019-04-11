# Introduction

## Context/Backgroung
* Artists want to stylized 3D scenes
* Having a 2D aspect
* good rendering in animation as video games

## Problem statement (Temporal Coherence)
* The trinity (motion coherence, temporal continuity, flatness)
* wide variety of possible style


## Scientific Approach and Investigative Method and Results


## Content of this report

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

#### VRAC

* Our method does not permit to do filtering on the whole image but it can be done after our step of stylizing.
* compute anchor point permit to avoid lines extraction and segmentation of the object
* Does our method permit to highlight silhouettes?
