# Don't allow our script to continue if any errors are observed
$ErrorActionPreference = "Stop"

# We will assume that we'll use Java that is bundled with Android Studio.
$env:JAVA_HOME="C:\Program Files\Android\Android Studio\jre\jre"

# We will assume that the Android SDK is in the default location that Android Studio installs it to.
$env:ANDROID_HOME="C:\Users\$env:UserName\AppData\Local\Android\Sdk"
Write-Host "Using Android SDK at: $env:ANDROID_HOME"

# We will be using a specific version of the Android NDK.
$NDK_VERSION="21.3.6528147"
$env:ANDROID_NDK="$env:ANDROID_HOME\ndk\$NDK_VERSION"
Write-Host "Using Android NDK at: $env:ANDROID_NDK"

Push-Location -Path "$env:ANDROID_HOME\tools\bin"
    Write-Host "Reviewing Android SDK licenses ..."
    
    # Create a file that can answer 'y' to each of the Android licenses automatically.
    Set-Content -Path 'yes.txt' -Value "y`r`ny`r`ny`r`ny`r`ny`r`ny`r`ny`r`ny`r`ny`r`ny`r`n"
    cmd.exe /c 'sdkmanager.bat --licenses < yes.txt'
    Remove-Item -Path 'yes.txt'

    Write-Host "Installing required Android SDK components ..."
    cmd.exe /c "sdkmanager.bat platform-tools build-tools;28.0.3 tools platforms;android-28 cmake;3.10.2.4988404 ndk;$NDK_VERSION"
Pop-Location

# Check that we have a 'third-party' folder
Push-Location -Path "..\..\"
if (!(Test-Path "third-party")) {
    New-Item -ItemType Directory -Path "third-party"
}
Pop-Location


# Check that we have the GLM third party library
if (!(Test-Path "..\..\third-party\glm")) {
    Write-Host "Downloading GLM into third party folder ..."
    $WebClient = New-Object System.Net.WebClient
    $WebClient.DownloadFile("https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip", "..\..\third-party\glm-0.9.9.8.zip")

    Push-Location -Path "..\..\third-party"
        Write-Host "Unzipping GLM into third party folder ..."
        cmd.exe /c 'tar -xf glm-0.9.9.8.zip'
        Remove-Item -Path glm-0.9.9.8.zip
    Pop-Location
}

# Check that we have the Tiny OBJ Loader third party library
if (!(Test-Path "..\..\third-party\tiny-obj-loader")) {
    Write-Host "Downloading Tiny OBJ Loader into third party folder ..."
    $WebClient = New-Object System.Net.WebClient
    $WebClient.DownloadFile("https://github.com/syoyo/tinyobjloader/archive/v1.4.1.zip", "..\..\third-party\tiny-obj-loader-v1.4.1.zip")

    Push-Location -Path "..\..\third-party"
        Write-Host "Unzipping Tiny OBJ Loader into third party folder ..."
        cmd.exe /c 'tar -xf tiny-obj-loader-v1.4.1.zip'
        Move-Item -Path tinyobjloader-1.4.1 -Destination tiny-obj-loader
        Remove-Item -Path tiny-obj-loader-v1.4.1.zip
    Pop-Location
}

# If the main Android application doesn't yet have an assets folder, create one.
Push-Location "app\src\main"
if (!(Test-Path "assets")) {
    Write-Host "Creating Android root 'assets' folder ..."
    New-Item -Path assets -ItemType Directory
}
Pop-Location

# Create a symlink to the main assets folder so they are included in the Android application.
Push-Location "app\src\main\assets"
if (!(Test-Path "assets")) {
    Write-Host "Linking assets"
    cmd.exe /c 'mklink /d assets ..\..\..\..\..\src\assets'
}
Pop-Location


# We need to add our own `vulkan-wrapper-patch.h` header into the `vulkan_wrapper.h`
# that ships with NDK version 20. The patch fixes compilation problems when using the
# `vulkan_wrapper.h` header in combination with the `vulkan.hpp` - both of which ship
# with NDK version 20, but appear to be misaligned - the `vulkan_wrapper.cpp` has
# unknown symbols in it which we will need to patch in.
Push-Location "$env:ANDROID_NDK\sources\third_party\vulkan\src\common"
    $PATCH_HEADER = Select-String -Path vulkan_wrapper.h -Pattern "#include <vulkan-wrapper-patch.h>"

    if ($null -eq $PATCH_HEADER) {
        Write-Host "Patching <android-ndk>\sources\third_party\vulkan\src\common\vulkan_wrapper.h to include patch wrapper ..."
        ((Get-Content -Path vulkan_wrapper.h -Raw) -replace('\#include <vulkan\/vulkan\.h>', "#include <vulkan/vulkan.h>`r`n#include <vulkan-wrapper-patch.h>")) | Set-Content -Path vulkan_wrapper.h
    }
Pop-Location

# Compile all Vulkan shaders
Push-Location "..\src\vulkan_shader_source"
    .\compile_shaders.ps1
Pop-Location

Write-Host "All done - import the project in this folder into Android Studio to run it!"