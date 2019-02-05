# procedural-stylization

# Build gratin:
- install qt5, eigen and openexr libs
- get source code: ```svn checkout https://scm.gforge.inria.fr/anonscm/svn/gratin/trunk```
```mkdir build && cd build && cmake .. && make -j8```

# Compile gratin nodes:
- edit gratin/nodes/plugins/CMakeLists.txt
- modify the path so that it links to your gratin build directory
- in plugins, do ```mkdir build && cd build && cmake .. && make -j8```
- add the path  gratin/nodes/plugins/nodes/lib to your gratin settings

