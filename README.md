# questart

Steps to clone, build and run:

Windows:
	1) Enable developer mode on your windows pc
	2) Run questart/project/windows/setup.ps1 script using windows powershell
	3) Third-party folder should have glew, glm, sdl-image-windows, sdl-windows & tiny-obj-loader sub folders
	4) Download latest vulkan-sdk from https://vulkan.lunarg.com/sdk/home#windows
	5) During installation process of the vulkan-sdk, choose destination folder as questart/third-party
	6) Replace the version name with vulkan-windows before proceeding with installation
		- C:\xxx\questart\third-party\vulkan-windows
	7) Repeat step 2
	8) Download ktx.zip from the google drive third-party folders. Unzip and add 'ktx' folder to third-party folders
	9) Open visual studio, click on open a local folder and select questart/project/windows/ directory. 
	10) On the solution explorer window, click on switch views icon and select 'CMake Targets View' to see the files.
	11) Open main.cpp file under project/platform/windows folder and run the code (ctrl + f5)