========================================
README
CS 550, Assignment #4
Partners: Michael Lam, Behrooz Mahasseni
E-mails: lamm@eecs, mahasseb@eecs
========================================

----------------------
Command Line Arguments
----------------------

./SimpleProgram (O left right bottom top near far)|(P fovy near far) light_x light_y light_z
	The first argument takes either "P" or "O", perspective and orthographic projections respectively.
	The next arguments specify the projection parameters, depending on perspective or orthographic:
		Perspective:
			fovy = field of view of Y, near/far are z coordinates of clipping planes
		Orthographic:
			left, right, bottom, top, near, far specify the rectangle of the viewing box
	The final three arguments specify the initial light position in (x, y, z) world coordinates.

-------------
Program Menus
-------------

Upon program start, the current mode is set to nothing. Set the current mode to something 
by selecting something under the Object Transformation or Scene Transformation submenu.

Right click anywhere in the window to load the following menu:

	Load .obj File - prompts path to .obj file in the command prompt, the loads into the program
	
	Object Transformation
		Translation - model translation
		Scale - model scaling
		Rotation - model rotation
	
	Scene Transformation
		Rotation - camera rotation
		Translation - camera panning
		Dolly - camera dolly
	
	Set Shader
    Old - switch to old (Gonoud) shader
    Phong - switch to Phong shader
    Toon - switch to toon shader
    Set Num Shades - prompts integer to set number of shades for toon shader
	
	Reset Model Transformations - reset all model transformations from all objects
	
	Reset View Transformations - reset all view transformations to default view
	
	Set Light - prompts new light position in the command prompt in x,y,z format, then updates light
	
	Exit - quit the program

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