
## Image and Vidéo-Based Artistic Stylisation (Paul Rosing) - Book

* Hertzmann's curve stroke algorithm
* Huang's approach (Importance map)
* Genetic Painting process of Collomosse (Genetic algorithm)
* Decomposition of an image to color it differently => compute orientation strokes

## State-of-the-Art Report on Temporal Coherence for Stylized Animations

* Avoid artifacts(flickering, popping, sliding) in animation => <span style="color:red;font-weight:bold;">Temporal Coherence</span>
* <span style="font-weight:bold;color:blue;">marks</span> (brush strokes, hatches, watercolor pigments, ...) with <span style="color:blue;font-weight:bold;">attributes</span> (color, thickness,
opacity, ...) to draw <span style="color:blue;font-weight:bold;">2D primitives</span> (points, lines and regions)
* Temporal Coherence: <span style="color:blue;font-weight:bold;">Flatness, Motion coherence, Temporal continuity</span>

* Flatness: <span style="font-weight:bold;">size</span> and <span style="font-weight:bold;">distribution</span> of marks should be <span style="font-weight:bold;">independent</span> of the
underlying geometry of the scene (marks size do not increase during zoom)
* bad Motion coherence gives the impression to see the scene through a shower door
* <span style="font-weight:bold;">depth peeling</span> solve transparency with two Z buffers
* parametrization frame to frame can helps to temporally coherence
* <span style="color:blue;font-weight:bold;">SLAM</span> (Self-Similar ine Artmap) solve the problem of resizing an object without creating artifacts. Cost few minutes of preprocessing.


## Personal Search

* <a href="https://forums.tigsource.com/index.php?topic=40832.msg1363742#msg1363742" style="color:red;font-weight:bold;">Dithering process</a> (example: Return of the Obra Dinn). Thanks to a Dither Pattern on applied on the 3D scene (like Real time hatching) and next bayer matrix and blue noise. They add motion warping. Then use spherically-mapped dither pattern to reduce popping artifacts but it still has problem of time coherence.
Source: https://forums.tigsource.com/index.php?topic=40832.msg1363742#msg1363742

* <a href="https://sites.google.com/site/cs7490finalrealtimehatching/" style="color:red;font-weight:bold;">Real time hatch rendering</a>. Texture mapped thanks to grey level of the 3D model rendered.
Source: https://sites.google.com/site/cs7490finalrealtimehatching/
