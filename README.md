# SHVR

## What is it?

SHVR (Segmentation using Haptics and Volumetric Rendering) is a program designed to segment volumetric dataset by using the haptic device to interact with structures. Automated segmentation processes rely heavily on assumptions about the dataset to set parameters for segmentation. With 1-to-1 mapping of the controller to the 3D virtual space, and force feedback to provide assistance along a region of interest, we aim to encourage interactivity to the segmentation process. This project is developed by Philmo Gu, with the assistance of Andrew Owens, Sonny Chan, and supervision of Dr. Przemyslaw Prusinkiewicz.

## How to compile with Microsoft Visual Studio

(1) Install Qt 5.15.2 for Open Source Development via the official Qt installer: https://www.qt.io/download-open-source.
You should select the "component" that corresponds to your version of Microsoft Visual Studio, e.g., MSVC 2019 64-bit, for Visual Studio 2019.
All other components are not needed and may be deselected.

(2) Set Visual Studio to recognize where Qt is installed.
Install Qt VS Tools as an "Extension" in Visual Studio and set the Qt Project Settings to your Qt installation from Step (1).
Then, add Qt to your PATH environment variable, e.g., add the entry `C:\Qt\5.15.2\msvc2019_64\bin` to the list of entries for PATH.

(3) Compile chai3d Version 3.2.
See SHVR/external/chai3d-3.2.0 or download it from https://www.chai3d.org/download/releases
After compiling, copy the library file, `chai3d.lib`, to `SHVR/external/chai3d-3.2.0/lib/{Debug|Release}/{Win32|x64}`

(4) Compile SHVR.
Open SHVR.sln, select Release, x64. Build.

## How to run SHVR

(1) Ensure the library files for Qt version 5.15.2 can be found by your system, e.g., add Qt's `bin` directory to your PATH environment variable.

(2) If you downloaded a release version of SHVR, the .dll files for loading tiff images and using a haptic device are included as part of the package.
You can skip the rest of this. Otherwise, ensure tiff.dll and the .dll for you haptic device (e.g., hdPhantom64.dll for the Geomagic Touch) can be found by your system,
or copy these files to the same folder as SHVR.exe. Then, copy the `settings` and `shaders` folders from `SHVR\SHVR` to the folder with SHVR.exe.

(3) Set the config file, `settings\default.ini`, and run SHVR.exe to load the image stack.
Alternatively, run SHVR.exe and drag and drop an image stack (and other SHVR files) into the open SHVR window.

## Developers

* Philmo Gu (Main)
* Andrew Owens
* Dr. Sonny Chan
* Dr. Przemyslaw Prusinkiewicz (Supervisor)
