#summary Guide to the ios-cmake project



== Introduction ==



This project provides a simple iOS toolchain file that may be used with CMake to build libraries.





== Contents ==



  * [#Toolchain_file]

  * [#Sample_library]

  * [#Sample_application]





==Toolchain_file==



The toolchain is located in at toolchain/iOS.cmake location. Using the toolchain is quite straightforward. It only requires you to call CMake with:



{{{

cmake -DCMAKE_TOOLCHAIN_FILE=path/to/iOS.cmake -GXcode

}}}



*NOTE:* Since this toolchain by default builds both arm6 and arm7 fat libraries and applications, you need to use the Xcode generator. Makefiles currently do not work.



The iOS.cmake file only has a few different options, all of which are documented in the file itself.





===IOS_PLATFORM=== 



*Values:* OS (default) or SIMULATOR



This decides if SDKS will be selected from the iPhoneOS.platform or iPhoneSimulator.platform folders

  * OS - the default, used to build for iPhone and iPad physical devices, which have an arm arch.

  * SIMULATOR - used to build for the Simulator platforms, which have an x86 arch.



===CMAKE_IOS_DEVELOPER_ROOT===



*Values:* automatic(default) or /path/to/platform/ eveloper folder



By default this location is automatically chosen based on the IOS_PLATFORM value above. If set manually, it will override the default location and force the user of a particular Developer Platform



===CMAKE_IOS_SDK_ROOT===



*Values:* automatic(default) or /path/to/platform/Developer/SDKs/SDK folder



By default this location is automatically chosen based on the CMAKE_IOS_DEVELOPER_ROOT value. In this case it will always be the most up-to-date SDK found in the CMAKE_IOS_DEVELOPER_ROOT path. If set manually, this will force the use of a specific SDK version





==Sample_library==



The iOS.cmake toolchain is great for building libraries.



A sample library is located in the samples/hello-lib location. It creates a small static sample library. You can build the library in the standard CMake way using a build subdirectory:



{{{

1. mkdir build.ios

2. cd build.ios

3. cmake -DCMAKE_TOOLCHAIN_FILE=../../../toolchains/iOS.cmake -GXcode ..

4a. Open XCode and build the install target or

4b. Simply type: xcodebuild -target install -configuration Debug

}}}



This will place the library and header file into the samples/hello-app folder.





==Sample_application==



The sample application is located in samples/hello-app. This application uses the library and headers from the hello-lib project which are automatically installed if you built the install target.



Simply open the included Xcode project and build the application for whichever platform you created the build-lib target for (OS or SIMULATOR). Note that you will need to sign the application to deploy it to a device.



*NOTE:* Building an iOS executable application strictly using cmake is not a great idea. There are a large number of options that need to be setup within XCode before a project can simply be deployed onto a device. The whole signing process is a good example. For this reason, we do not attempt to build the hello-app sample using cmake. 
