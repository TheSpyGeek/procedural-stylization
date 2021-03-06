# Research Paper

## A Dynamic Noise Primitive for Coherent Stylization

* "Texture-based approaches are mostly used for continuous
textures (canvas, watercolor)"

## Active Strokes: Coherent Line Stylization for Animated 3D Models

## An Image-Based Shading Pipeline for 2D Animation

## AnIntroductionToStylizedRendering

## Coherent noise for non-photorealistic rendering

## Consistent Media Model for Real-Time Scene Rendering

* produces a coherent stylization avoiding the shower door and popping effects
* fractalization for infinite zoom

## Dynamic 2D Patterns for Shading 3D Scenes

## Dynamic Canvas for Non-photorealistic Walkthroughs

* infinite zoom effect
* animation of background to improve the immersion when the camera is moving.
* static backgrounds give a "shower door" effect
* add motion to background with moving strokes according to camera motion

## Dynamic Solid Textures for Real-Time Coherent Stylization

* In this paper we extend the Dynamic Canvas infinite zoom mec
## Consistent media model for real time scene renderinghanism to object space textures, which allows the real-time stylization of dynamic objects without any sliding effects.
* This 2D multiscale texture synthesis algorithm generates texture elements on the fly during the zoom

## Fin textures for real-time painterly aesthetics

## From image parsing to painterly rendering

## Image and video based painterly animation
Hertzmann's
* process a segment of video
* extract motion information from one source and apply it to another image
* image space (video)
* compute motion for each strokes drawn

## Implicit Brushes for Stylized Line-based Rendering

* line extraction
* convolution of points

## Improving noise

* perlin noise
* for each vertices generate **pseudorandom gradient vector**
* compute dot product between these 4 vectors
* Interpolate the values obtained with fade function (ease curve: 6t^5 - 15t^4 + 10t^3)

##  Interactive watercolor rendering with temporal coherence and abstraction


## Image and Vidéo-Based Artistic Stylisation (Paul Rosing) - Book

* Hertzmann's curve stroke algorithm
* Huang's approach (Importance map)
* Genetic Painting process of Collomosse (Genetic algorithm)
* Decomposition of an image to color it differently => compute orientation strokes


## MNPR: a framework for real-time expressive non-photorealistic rendering of 3D computer graphics
#### Questions
* how to build higher levels of control on top of the locally painted parameters to cover the full interaction spectrum?
* how to design sufficiently generic, yet semantically meaningful parameters to allow cross-stylistic art-direction?

#### Notes
* They use noise (RGB) to control the UV inclination, the substracte distortion, pigments density .
* Mapped control considered like most useful level of control
* three distinct stylizations: watercolor, oil paint and charcoal.analisys

## Motion-coherent stylization with screen-space image filters

* a **plausible painted fur** should extend outside the silhouette of the object.
* image space: to compute the object motion we need to compute the ___optical flow___ of the animation
    * but complicated to compute a correct and noise-free optical flow
* **G-buffers** in screen-space = 2.5D
* use **segmentation** to detect overlapping (use K-means algorithm)
* **LIC** = Line Integral Convolution

## OPENNPAR: a system for developing, programming, and designing non-photorealistic animation and rendering

* softawre as gratin
* **do not give how they do**

## OverCoat: An Implicit Canvas for 3D Painting

**OverCoat** is a software to draw strokes from a 3D model using a method that the artists can 'inflate' the 3D model to change where they draw in depth. It also allow to draw hair or leaves starting from a point on the 3D mesh.

## Painterly rendering for animation


## Pen and ink textures for real-time rendering

* from an input image is based on the distribution of strokes according to a probability function.
* draw outlines perpendicular to the direction of the gradient of the input image

## Procedural noise using sparse Gabor convolution



## Processing images and video for an impressionist effect

## Programmable rendering of line drawing from 3D scenes

* line extraction in image space
* for each style sheet done by the user, it apply stroke at some points of the final image
* style sheets define by where draw, what is drawn, ...

## Quality Assessment of Fractalized NPR Textures: a Perceptual Objective Metric

* goal is to define some estimate of the quality of NPR renderings based on texture fractalization
* rank fractal stylization like (hatching, stippling, ...)

## Real-time coherent stylization for augmented reality

## Schm12.pdf

## State of the "Art”: A Taxonomy of Artistic Stylization Techniques for Images and Video

## State-of-the-Art Report on Temporal Coherence for Stylized Animations

* Avoid artifacts(flickering, popping, sliding) in animation => <span style="color:red;font-weight:bold;">Temporal Coherence</span>
* <span style="font-weight:bold;color:blue;">marks</span> (brush strokes, hatches, watercolor pigments, ...) with <span style="color:blue;font-weight:bold;">attributes</span> (color, thickness,
opacity, ...) to draw <span style="color:blue;font-weight:bold;">2D primitives</span> (points, lines and regions)
* Temporal Coherence: <span style="color:blue;font-weight:bold;">Flatness, Motion coherence, Temporal continuity</span>

* Flatness: ___size___ and ___distribution___ of marks should be
___independent___ of the
underlying geometry of the scene (marks size do not increase during zoom)
* bad Motion coherence gives the impression to see the scene through a shower door
* **Depth peeling** solve transparency with two Z buffers
* parametrization frame to frame can helps to temporally coherence
* <span style="color:blue;font-weight:bold;">SLAM</span> (Self-Similar ine Artmap) solve the problem of resizing an object without creating artifacts. Cost few minutes of preprocessing.
* Bénard et al. extend the **Gabor noise** to create
complex non-photorealistic patterns (<a href="http://www-sop.inria.fr/reves/Basilic/2009/LLDD09/LLDD09PNSGC_paper.pdf" style="font-weight:bold;">source</a> and <a href="http://graphics.cs.kuleuven.be/publications/LLDD09PNSGC/" style="font-weight:bold;">example</a>)
* <span style="color:blue;font-weight:bold;">Applying texture</span> in the image space:
    * planar methods that deform the texture with global as-rigid-as-possible transformations
    * texture advection and filtering approaches that work at the pixel level
* Fractalization => inifinite zoom ( -- modify the brushes, strokes)

## Stroke Based Painterly Rendering

* algorithm in image space

## Temporal Coherence Methods in Real-Time Rendering

## Video Stylization: Painterly Rendering and Optimization with Content Extraction

## Video watercolorization using bidirectional texture advection

## View-Dependent Particles for Interactive Non-Photorealistic Rendering

* rendering in screen space
* compute particles => define density of particles to define the level of precision
* draw strokes from particles

## WYSIWYG NPR: Drawing Strokes Directly on 3D Models

* Interactive (different modes)
* Select a silhouette to draw
* draw strokes in 3D Space


# Other research

## Maya Painter Effect ([video](https://www.youtube.com/watch?v=ybCqeHklt0M))

* from a 3D mesh
* and brushes (in the video: grass (in the png format i think))
* they create of grass on the 3D model

## Software

* Maya with Painter Effects
* OverCoat
* Disney's Deep canvas
* [Freestyle](freestyle.sourceforge.net) (Stéphane Grabli)[Grabli et al. 2010]
* RenderMan Pixar


## Personal Search

* <a href="https://forums.tigsource.com/index.php?topic=40832.msg1363742#msg1363742" style="color:red;font-weight:bold;">Dithering process</a> (example: Return of the Obra Dinn). Thanks to a Dither Pattern on applied on the 3D scene (like Real time hatching) and next bayer matrix and blue noise. They add motion warping. Then use spherically-mapped dither pattern to reduce popping artifacts but it still has problem of time coherence.
Source: https://forums.tigsource.com/index.php?topic=40832.msg1363742#msg1363742

* <a href="https://sites.google.com/site/cs7490finalrealtimehatching/" style="color:red;font-weight:bold;">Real time hatch rendering</a>. Texture mapped thanks to grey level of the 3D model rendered.
Source: https://sites.google.com/site/cs7490finalrealtimehatching/
