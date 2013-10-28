========================================
README
CS 550, Assignment #2
Partners: Michael Lam, Behrooz Mahasseni
E-mails: lamm@eecs, mahasseb@eecs
========================================

----------------------
Command Line Arguments
----------------------

./SimpleProgram p|o fx fy fz ax ay az ux uy uz [(fovy aspect zNear zFar)|(left right bottom top zNear zFar)]
	The first argument takes either "p" or "o", perspective and orthographic projections respectively.
	The next six arguments specify the camera paramters:
		(f_x, f_y, f_z) is the camera position
		(a_x, a_y, a_z) is the look at position
		(u_x, u_y, u_z) is the up direction of camera
	The final arguments specify the projection parameters, depending on perspective or orthographic:
		Perspective:
			fovy = field of view of Y, aspect = aspect ratio, zNear/zFar are z coordinates of clipping planes
		Orthographic:
			left, right, bottom, top, zNear, zFar specify the rectangle of the viewing box

------------------
Running on Windows
------------------

Using a fresh copy of the Windows MSVC 2012 files from the Resources page, copy the attached directories into vs2012-example-3.2\SimpleProgram\. There should be folders named headers, shaders and src. Overwrite the src folder.

(Note that the shader files have been unaltered from the original assignment #2 shader files.)

Then compile using Visual Studio and run.

----------------
Running in Linux
----------------

Similar to Windows instructions. Must comment out the line #define USE_WINDOWS in all .cpp files in the src/ directory.

Then compile using make and run.