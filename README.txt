Implemented Features:
- Text Renderer (prints to terminal/console without issue); our visitor is GLScenegraphTextRenderer.h

- Scenegraph model; we have created a simple castle. We have met the criteria of 10 minimum structures; we have the ground,
  four walls, and the four turrets (each of which are composed of a cylinder, a box, and four smaller boxes). The castle scenegraph
  is located in castle.txt, and we have created smaller scenegraphs for the turret, turret roof, wall, and ground respectively. We
  have two humanoids in creative poses; the scenegraphs are akash.txt and jaemin.txt

- File Input; the program will take a command line argument as input and use the file name to open the corresponding scenegraph.

- Camera Position; the camera has a good view of the entire scene; we are also using glm::perspective

- Window: The model will not stretch or squeeze after resizing the window.

Incomplete Features:
- Trackball; the model will rotate in response to the player pressing and dragging the cursor, however the rotations are incomplete.
             The model rotates along its relative axes, regardless of its orientation with respect to the camera. These
             axes rotate along with the model, causing issues with the trackball.
             However, we have implemented the reset functionality where pressing 'R' or 'r' will reset the trackball to the initial
             orientation.

Work Split:
- Everything was done on call together; we collaborated on every aspect of the program that we added or created (the only exceptions
  are the scenegraph models akash.txt and jaemin.txt, which were created by Akash and Jae-Min respectively).