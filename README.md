# Inverse Kinematics Editor (working title)
This is an editor containing an inverse kinematics implementation. The aim is to be able to easily open existing character models and modify their pose. 

On the longer term, it would be made possible to make animations by interpolating between poses.

## How to build

Language standard: c++17

Dependencies:
* Ogre 1.12.1 (OgreMain, OgreBites and OgreHLMS)
* SDL2 (strictly speaking optional for building, but input won't work otherwise)

The suggested way of setting up the project for development is by using vcpkg. Make sure to first install SDL2 and afterwards install Ogre because SDL2 must be present for detection by the Ogre build.

After installing the dependencies in vcpkg, run the following cmake command within the desired build folder:

`$>cmake -DCMAKE_TOOLCHAIN_FILE=<vcpkg_folder>/scripts/buildsystem/vcpkg.cmake <source_folder>`
