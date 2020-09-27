# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

# Check that we have a 'third-party' folder
Push-Location -Path "..\..\"
if (!(Test-Path "third-party")) {
    New-Item -ItemType Directory -Path "third-party"
}
Pop-Location

# Check that we have the SDL2 third party Windows dev library
if (!(Test-Path "..\..\third-party\sdl-windows")) {
    Write-Host "Downloading SDL Windows Dev library into third party folder sdl-windows ..."
    $WebClient = New-Object System.Net.WebClient
    $WebClient.DownloadFile("https://www.libsdl.org/release/SDL2-devel-2.0.12-VC.zip", "..\..\third-party\SDL2-devel-2.0.12-VC.zip")

    Push-Location -Path "..\..\third-party"
        Write-Host "Unzipping SDL Windows Dev library into third-party\sdl-windows ..."
        cmd.exe /c 'tar -xf SDL2-devel-2.0.12-VC.zip'
        Move-Item -Path SDL2-2.0.12 -Destination sdl-windows
        Remove-Item -Path SDL2-devel-2.0.12-VC.zip
    Pop-Location
}

# Check that we have the GLEW third party library
if (!(Test-Path "..\..\third-party\glew")) {
    Write-Host "Downloading GLEW into third party folder glew ..."
    $WebClient = New-Object System.Net.WebClient
    $WebClient.DownloadFile("https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0-win32.zip", "..\..\third-party\glew-2.1.0-win32.zip")

    Push-Location -Path "..\..\third-party"
        Write-Host "Unzipping GLEW library into third-party\glew ..."
        cmd.exe /c 'tar -xf glew-2.1.0-win32.zip'
        Move-Item -Path glew-2.1.0 -Destination glew
        Remove-Item -Path glew-2.1.0-win32.zip
    Pop-Location
}

# Check if we have the GLM third party library
if (!(Test-Path "..\..\third-party\glm")) {
	Write-Host "Downloading GLM into third party folder glm ..."
	$WebClient = New-Object System.Net.WebClient
	$WebClient.DownloadFile("https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip", "..\..\third-party\glm-0.9.9.8.zip")
	
	Push-Location -Path "..\..\third-party"
		Write-Host "Unzipping GLM library into third-party\glm ..."
		cmd.exe /c 'tar -xf glm-0.9.9.8.zip'
		Remove-Item -Path glm-0.9.9.8.zip
	Pop-Location
}

# Check if we have the Tiny Object Loader library
if (!(Test-Path "..\..\third-party\tiny-obj-loader")) {
	Write-Host "Downloading Tiny Object Loader into third party folder tiny-object-loader ..."
	$WebClient = New-Object System.Net.WebClient
	$WebClient.DownloadFile("https://github.com/syoyo/tinyobjloader/archive/v1.4.1.zip", "..\..\third-party\tiny-obj-loader-v1.4.1.zip")
	
	Push-Location -Path "..\..\third-party"
		Write-Host "Unzipping Tiny Object Loader library into third-party\tiny-object-loader ..."
		cmd.exe /c 'tar -xf tiny-obj-loader-v1.4.1.zip'
		Move-Item -Path tinyobjloader-1.4.1 -Destination tiny-obj-loader
		Remove-Item -Path tiny-obj-loader-v1.4.1.zip
	Pop-Location
}

# Check that we have the SDL2_image third party Windows dev library
if (!(Test-Path "..\..\third-party\sdl-image-windows")) {
    Write-Host "Downloading SDL Image Windows library into third party folder sdl-image-windows ..."
    $WebClient = New-Object System.Net.WebClient
    $WebClient.DownloadFile("https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.4-VC.zip", "..\..\third-party\SDL2_image-devel-2.0.4-VC.zip")

    Push-Location -Path "..\..\third-party"
        Write-Host "Unzipping SDL Image Windows library into third-party\sdl-image-windows ..."
        cmd.exe /c 'tar -xf SDL2_image-devel-2.0.4-VC.zip'
        Move-Item -Path SDL2_image-2.0.4 -Destination sdl-image-windows
        Remove-Item -Path SDL2_image-devel-2.0.4-VC.zip
    Pop-Location
}

# Compile Vulkan shaders into SPIR-V binary assets.
Push-Location -Path "../src/vulkan_shader_source"
    .\compile_shaders.ps1
Pop-Location