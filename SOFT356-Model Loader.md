# __SOFT356- Model Loader__
# Jake Morgan


#IMPORTANT
To use the solution, please install the following NuGet packages into the solution:
- glm by GLM contributors (v0.9.9.600)
- nupengl.core by Jonathan Dickinson (v0.1.0.1, later versions should work too)
- pugixml by Arseny Kapoulkine (v1.10.0)


# Disclaimer:
3rd Party Dependencies:
- FreeGLUT
- GLEW
- GLM
- stb_image
- pugi XML Reader

Code has been written from scratch unless otherwise stated, using mainly the tutorials provided at https://www.opengl-tutorial.org/, a variety of Stack Overflow posts, the code available on the SOFT356 Github repo under the ‘Sample Code’ folder.
The code obtained from these sources has been reworked to fit my own need and is in no way a direct copy. Some similarities may occur in areas without much ‘wiggle room’ for customising the code (such as the shaders).
2 direct exceptions to this are __LoadShaders.cpp__ which is directly copied from the SOFT356 repo, and the __getFileName()__ method in the __ModelLoader.cpp__, which has been copied from https://www.daniweb.com/programming/software-development/threads/159753/opening-a-filedialog as this is a fairly standard c++ function. This has been credited in the code.
# Which version of Visual Studio, OpenGL?
This has been built in VisualStudio 2019 using OpenGL 4.1.
# How do I work it?
Upon launching the application, a console window will appear. The window will give an option to print a tutorial, which will list the capabilities of the model loader. After proceeding, a view window will open and will display an empty scene (keep the console window in view). The mouse will be captured to this window as it is used to rotate the camera, and the wasd/qe keys are used to move the camera around the scene. Pressing the ESCAPE button will open a menu in the console (and uncapture the mouse). The menu will list commands the application is capable of (such as load a model, retexture a model and delete a model from the scene) as well as various commands for changing the movement speed / sensitivity for comfort when navigating models of different sizes.
Each model is given an ID, which will default to the filename, or if the filename is unable to be used for whatever reason, will default to “unknown” followed by the number of models that have been loaded since the program started + 1. This ID is used for model specific commands such as removing a model from the scene, scaling or translating a model, or retexturing a model and is Case Sensitive. A list of model names can be obtained through a command shown on the menu.
# How does your code fit together and how should a programmer navigate it?
The code is split into classes based on features.
The main __ModelLoader.cpp__ file contains the main code for the model loader to function. The code is written in such a way that any code that is relied on by something else is above that class. I.e. The __main__ method creates a __pipeline__, so it is beneath the __pipeline__ class. The __pipeline__ class creates __Model__s so is beneath the __Model__ class. All code specific to models is in an __Model__ class, all code related to the initialisation, looping and OpenGL configuration is in a __pipeline__ class, and the main class and a couple of other methods are global. The tutorial is global as it needs to be called from __main__ but does not fit into any class (and is too small to be a class of it’s own) and the __key\_callback__ method is global because it needs to be for the GLFW key event system to work.
Object loading is split into a seperate __OBJLoader.cpp__ file. This only has the __LoadOBJFile__ method, which reads from an OBJ file and outputs the data to the __Model__ class from the __ModelLoader.cpp__ file.
Lastly, the __controller.cpp__ file contains code for manipulating the perspective/view matrix, and holds variables that affect various aspects of the camera, such as the field of view and perspective type. This file contains getters for the perspective/view matrices.
# Anything else which will help us to understand how your prototype works?
Physically: All information required to operate the program can be found in the tutorial or menu. Sometimes the menu doesn’t load on the first time the ESCAPE key is pressed. If this happens the cursor will still be captured by the window but pressing the escape key again will fix the issue (likewise, sometimes the menu stays open when it shouldn't, just typing anything in and pressing enter again will fix this). The selection is not automatically shifted to the console when the menu is opened, you will need to click in the console to type commands. Lastly, when loading a model, use the File Type dropdown in the OpenFileDialog to switch between __.obj__ and __.dae__ files. __Note:__ For texturing, only a single texture file is used for each model loaded (all UVs are loaded correctly however, which could be seen if a single complex object with a complex UV map that fits on a single texture is loaded). A static light source is used to calculate specular and ambience. 
Programmatically: I have tried to split everything into classes following OOP principles, and focusing on extendability. Variable names and method names should be self explanatory.
