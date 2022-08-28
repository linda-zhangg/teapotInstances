# CGRA 251 Assignment 2

Linda Zhang zhanglind1 300570498

## Core - DONE
- Made orbital controls by transforming the view matrix by the pitch and yaw and distance.
- Added mouse controls to drag for the rotations and scroll for distance.
- Added uniform variables to the fragment shader to implement Phong shading and loaded them in for GUI controls. The light colour, specular colour and shininess can be altered on these controls.

## Completion - DONE
- made a mat4 std::vector array of matrix transformations to operate on many instances of the teapot. Loaded the vertex buffer with the transformations and made an in variable in the vertex shader. 
### A button on the GUI, "more instances" can be clicked to show this part of the completion. It can be toggled on and off.

- made a vec3 std::vector of colours to operate on many instances of the teapot. Loaded vertex buffer with the colours and altered both vertex and fragment shaders to use the random colours instead of the uniform variable.
### A button on the GUI, "use color instances" can be clicked to show this part of the completion. It can be toggled on and off.

- make a texture object and loaded in "checkerboard.jpg". COnnected this in the fragment shader. If the file does not open, the line number is 80 in cgra_mesh.cpp. I had to set the custom working directory as the work folder for the current path to work.
### A button on the GUI, "load texture" can be clicked to show this part of the completion. It can be toggled on and off.

## Challenge - DONE
- built and drew bounding boxes for each instance using help from bounding_box.hpp.
- calculated AABB bounding boxes for each instance in Application::calculateBoundingBox 
### A button on the GUI, "Draw bounding box" can be clicked to show this. It can be toggled on and off.
