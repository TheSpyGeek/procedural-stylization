# Procedural-stylization

## Description

This repository is about non-photorealistic rendering. Our goal was to stylize 3D scene at the step of compositing. To do so we use marks that we paste in image space on our object. But this technique implies problem during the animation of the camera. In order to solve this problem, we used anchor points to anchor the marks.

## Folders

* ```gratin```
    * ```pipeline``` folder with the pipelines to stylize
    * ```nodes``` folder with the nodes to add in gratin to make the pipelines work
* ```notes``` files with some notes about new ideas, notes on papers, ...
* ```presentation``` slides of the presentation
* ```rapport``` report files
* ```Resultats``` somes results of our technique


# Compile

## Build gratin:
* install qt5, eigen and openexr libs
* get source code: ```svn checkout https://scm.gforge.inria.fr/anonscm/svn/gratin/trunk```
* ```cd trunk && mkdir build && cd build && cmake .. && make```

## Compile procedural node
* edit ```gratin/nodes/proceduralStyle/CMakeLists.txt```
* modify the path so that it links to your gratin build directory
* edit QString  of the node contained in the file <stylization_vX.cpp> to your path to this node
    * ```gratin/nodes/proceduralStyle/stylization_vX/stylization_vX.cpp```
* in ```proceduralStyle``` folder do:
```
mkdir build
cd build
cmake .. && make
```

* add the path ```gratin/nodes/proceduralStyle/build/nodes/lib``` and add ```gratin/nodes/proceduralStyle/nodes/customs/``` (Tools >> Manage node paths... >> Add path)
* Note: the node in ```Gratin``` is in the stylization folder



# Examples

* pipeline with all function usefull: ```gratin/pipelines/romain/romainCleanPipeline01.gra```
* with the fractal noise ```gratin/pipelines/romain/romain-fractalization.gra```
* test hair rendering: ```gratin/pipelines/romain/bouleDePoils01.gra```


## Models and splats

* the splats used are in the folder: ```gratin/data/imgs/```
* the models used are in the folder: ```gratin/data/models/```

**but you can use your owns**


# Presentation use


1. install npm (4.0 or later)

2. go to presentation folder

```cd presentation/```

3. Install dependencies

```npm install```

4. Serve the presentation and monitor source files for changes

```npm start```


5. Open http://localhost:8000 to view your presentation

You can change the port by using ```npm start --port=8001.```

6. In the slides press 's' to display presentator view


# Common issues

* [The pipeline does not open. The program segmentation fault.](The-pipeline-does-not-open-The-program-segmentation-fault)

## The pipeline does not open. The program segmentation fault.

Check if you have added the path to the compiled nodes (Tools >> Manage node paths...) See [section procedural nodes](#Compile-procedural-node)

## There is nothing displayed in all the nodes of the pipeline.

You have to load in the first node 'AdvancedGBuffers' of models. You have some models in the folder  ```gratin/data/models/```

## The last node of the pipeline 'stylization_vX' does not display anything.

Check if you have changed the path of your shader in the ```stylization_vX..cpp``` to your path (as in the [section Compile procedural node](#Compile-procedural-node))
