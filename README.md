# procedural-stylization

# Build gratin:
* install qt5, eigen and openexr libs
* get source code: ```svn checkout https://scm.gforge.inria.fr/anonscm/svn/gratin/trunk```
* ```cd trunk && mkdir build && cd build && cmake .. && make```

# Compile procedural node
* edit ```gratin/nodes/proceduralStyle/CMakeLists.txt```
* modify the path so that it links to your gratin build directory
* edit QString  of the node contained in the file <stylization_v1.cpp> to your path to this node
* in ```proceduralStyle``` folder do:
```
mkdir build
cd build
cmake .. && make
```
* add the path ```gratin/nodes/proceduralStyle/nodes/lib``` (Tools >> Manage node paths... >> Add path)

# Pipeline example
* ```gratin/pipelines/simplified_pipeline/simplified01.gra```


# Compile tests nodes (optional):
* edit ```gratin/nodes/plugins/CMakeLists.txt```
* modify the path so that it links to your gratin build directory
* edit QString  of the node contained in the file <nodeName.cpp> to your path to this node
* in plugins, do ```mkdir build && cd build && cmake .. && make -j8```
* add the path  gratin/nodes/plugins/nodes/lib to your gratin settings
