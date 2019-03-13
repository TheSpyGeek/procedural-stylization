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
* add the path ```gratin/nodes/proceduralStyle/build/nodes/lib``` and add ```gratin/nodes/proceduralStyle/nodes/customs/``` (Tools >> Manage node paths... >> Add path)
* Note: the node in ```Gratin``` is in the stylization folder

# Examples
* ```gratin/pipelines/simplified_pipeline/simplified01.gra``` basic example
* ```gratin/pipelines/simplified_pipeline/simplified02.gra``` example with a character and its color in texture
* the splats used are in the folder: ```gratin/data/imgs/```
* the models used are in the folder: ```gratin/data/mdoels/``` but you can use your owns


# Compile tests nodes (optional):
* edit ```gratin/nodes/plugins/CMakeLists.txt```
* modify the path so that it links to your gratin build directory
* edit QString  of the node contained in the file <nodeName.cpp> to your path to this node
* in plugins, do ```mkdir build && cd build && cmake .. && make -j8```
* add the path ```gratin/nodes/testNodes/build/nodes/lib``` to your gratin settings
