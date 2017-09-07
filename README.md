# VRPN-OpenVR
OpenVR support for VRPN. Builds as a server that exposes OpenVR HMDs and controllers as as openvr/[hmd|controller]/[trackedDeviceId]. 
Each of which having the relevant vrpn devices that it supports: vrpn_Tracker for HMDs and vrpn_Tracker, crpn_Analog and vrpn_Button
for the controllers.

# Installing dependencies
Follow the [README](https://github.com/ValveSoftware/SteamVR-for-Linux) from SteamVR for Linux to install Steam, SteamVR and Nvidia drivers.

## On Linux
Install the downloaded package (change the path if it is different for you)

    sudo dpkg -i ~/Downloads/steam_latest.deb
    
You will need to execute some preliminary steps to be able to run SteamVR on Linux.
* First you need the latest nvidia drivers (at least 381.22):

      sudo add-apt-repository ppa:graphics-drivers/ppa
      sudo apt update
      sudo apt install nvidia-381  # or higher if available
        
* Then you will have to modify the steam runtime script used to run an executable in the steam runtime:

      vim ~/.steam/steam/ubuntu12_32/steam-runtime/run.sh
    
* Add the system library folder first in LD_LIBRARY_PATH:

      export LD_LIBRARY_PATH="/usr/lib/x86_64-linux-gnu/:${TOP}/amd64/lib/x86_64-linux-gnu:${TOP}/amd64/lib:${TOP }/amd64/usr/

## On Windows
Run the downloaded executable to install Steam.

## Post installation

Install SteamVR directly from Steam and run the room setup application.


# Building
This server builds in Visual Studio (2015 was used) and Linux.
We ran into issues trying to build it with CMake under Cygwin. A CMake file generating a VS project was considered 
but was found to introduce too many steps for the end result still requiring switching to VS for the build.

* Initialize the repository with submodules in order to retrieve OpenVR and VRPN

      git submodule update --init --recursive

## On Linux:
* Create a build directory at the root of the project and set the current directory to it:

      mkdir build && cd build

* Generate cmake files and build it:
    
      cmake .. && make -j$(nproc)

## On Windows:
Open the `VRNP-OpenVR.sln` solution and build from the IDE
       
        
# Running

## On Linux

Run your vrpnOpenVR binary in the Steam runtime, from the build directory (if you are elsewhere just change the path 
of the binary):

    ~/.steam/steam/ubuntu12_32/steam-runtime/run.sh ./vrpnOpenVR

## On Windows
Just launch the program from the VS solution (F5 to simple run or Ctrl+F5 for debug). Otherwise just click the play 
button at the top of the window.


# Troubleshooting

## Headset is not detected properly

A few ideas to fix this:

* activate / deactivate the direct mode
* unplug / replug the lighthouses
* reboot the computer
* check that your graphic driver is up-to-date: SteamVR needs the very latest drivers which are not always packaged in the default repositories, nor in the Graphic Drivers PPA.

# Acknowledgements
This is still very much a work in progress and is by no means a complete and stable solution for using OpenVR with VRPN.
The primary goal of this server is to provide controller data to a Blender instance running on a separate machine 
(but can also be used locally).
