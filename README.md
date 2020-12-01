![](/webassets/title2.jpg)

# LUDUS
### By RoManS
	Mansoor Patan
	Rongyi Zhao
	Surya Ajayakumar


## What is LUDUS?

Ludus is a VR scenario creation environment for educators who would like to use VR tools to create interactive experiences to aid their curriculums. Create mini tutorials to challenge the boundaries between fun and learning.

## Devlog

### Latest features:
	- Load/Save feature and shareable tutorials
	- Zoom lens for precise inspection
	- Laser pointer for ranged selection
	
### Upcoming Features:
	- Gizmo mode for precision translation
	- Media Embedding (text/audio/video)
	- Tutorial and Asset server

## Resources and Links

### Demo Video
	*Coming Soon*
	
### Latest stable build
	*Coming soon*

## Steps to clone, build and run the app in Quest 2 (from Unity):
	1. Download the code from GitHub
	2. Download Unity version 2019.4.4f1
		- Install with Android SDK, NDK and OpenJDK checked
	4. Connect Oculus Quest 2 device to PC with developer mode enabled
	3. Open the project and go to File -> Build Settings
	4. Select platform as Android
	5. Set Texture Compression to ASTC
	6. Select device under Run Deivice
	7. Click on Build and Run

## Steps to clone, build and run the engine :

### Windows:
	1. Enable developer mode on your windows pc
	2. Run questart/project/windows/setup.ps1 script using windows powershell
	3. Third-party folder should have glew, glm, sdl-image-windows, sdl-windows & tiny-obj-loader sub folders
	4. Download latest vulkan-sdk from https://vulkan.lunarg.com/sdk/home#windows
	5. During installation process of the vulkan-sdk, choose destination folder as questart/third-party
	6. Replace the version name with vulkan-windows before proceeding with installation
		* C:\xxx\questart\third-party\vulkan-windows
	7. Repeat step 2
	8. Download ktx.zip from the google drive third-party folders. Unzip and add 'ktx' folder to third-party folders
	9. Open visual studio, click on open a local folder and select questart/project/windows/ directory. 
	10. On the solution explorer window, click on switch views icon and select 'CMake Targets View' to see the files.
	11. Open main.cpp file under project/platform/windows folder and run the code (ctrl + f5)

### Android (QUEST):
	1. Enable developer mode on your windows pc
	2. Run questart/project/quest/setup.ps1 script using windows powershell
	3. Download latest oculus mobile sdk form https://developer.oculus.com/downloads/package/oculus-mobile-sdk/. Select version 1.36
	4. Extract it in third-party folder and rename it as ovr-mobile
	5. Install latest android-studio if you haven't already
	6. Open android-studio. Select File->Open and choose quest folder under questart/project/
	7. In the project window, choose the Project from the drop-down. Source code is seen under Quest Start
	8. Connect quest to the pc. When the device name is seen in android studio, press run app button to build and push to quest
### Contact Us: https://twitter.com/RongyiZhao
