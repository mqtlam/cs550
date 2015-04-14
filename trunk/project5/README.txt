========================================
README
CS 550, Final Project
Partners: Michael Lam, Behrooz Mahasseni
E-mails: lamm@eecs, mahasseb@eecs
========================================

------------------
Running on Windows
------------------

Using a fresh copy of the Windows MSVC 2012 files from the Resources page with the updated header files, copy the attached directories into vs2012-example-3.2\SimpleProgram\. There should be folders named headers, shaders and src. Overwrite the src folder.

Must uncomment the line #define USE_WINDOWS in all .cpp files in the src/ directory.

Then compile using Visual Studio and run.

----------------
Running in Linux
----------------

Similar to Windows instructions. Must comment the line #define USE_WINDOWS in all .cpp files in the src/ directory.

Then compile using make and run.

-------------
Program Menus
-------------

Upon program start, the current transformation mode is set to nothing. Set the current mode to something by selecting something under the Scene Transformation submenu. Load an image using the "Load RGB Image" or "Load RGBD Images" options.

Right click anywhere in the window to load the following menu:

	Load RGB Image: the command prompt will ask for the file name of the RGB image.
	
	Load RGBD Images: the command prompt will ask for two file names, the first for the RGB image and the second for the Depth image.
	
	Switch to Red Channel: visualizes the red channel as the depth.
	
	Switch to Green Channel: visualizes the green channel as the depth.
	
	Switch to Blue Channel: visualizes the blue channel as the depth.
	
	Switch to Grayscale: visualizes the gray scale as the depth.
	
	Switch to Depth Channel: visualizes the depth data (when RGBD image is loaded).
	
	Scene Transformation
		Rotation
		Translation
		Dolly
	
	Reset All Transformations: reset transformation