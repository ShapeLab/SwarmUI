#ofxLibwebsockets
* Add the power of [WebSockets](http://www.websocket.org/) to your openFrameworks project!
* openframeworks wrapper for libwebsockets (http://git.warmcat.com/cgi-bin/cgit/libwebsockets/) 
* implements both client and server functionality of libwebsockets
* includes support for sending text (strings) and binary data
* based on Paul Reimer's work on ofxWebUI (https://github.com/paulreimer/ofxWebUI) and jason vancleave's ofxLibWebSockets (https://github.com/jvcleave/ofxLibWebSockets)

##Examples: Basic
* example_client_hello_world
	* The simplest example: open up a Websocket to echo.websocket.org and say "hello"!
* example_server_echo
	* A basic Websocket server you can connect to from your browser (http://localhost:9092) to send messages back and forth
	* Includes a simple javascript example that is hosted by the app
	* Can also connect to example_client_hello_world
* shared canvas
	* example_server_sharedcanvas + example_client_sharedcanvas
	* Clients in openFrameworks and javascript can collaboratively draw on a canvas

##Examples: Advanced
* basic binary
	* example_server_binary and example_server_binaryvideo
	* demonstrates a custom binary setup, sending the raw pixels from OF to javascript
	* requires a bit of processing on the javascript side, please see the code in data/web
* optimized binary
	* example_server_blob, example_client_blob, example_server_blobvideo
	* requires my fork of [ofxTurboJpeg](https://github.com/robotconscience/ofxTurboJpeg)
	* demonstrates how to send true binary data (i.e. a file) via ofxLibwebsockets
		* check out the javascript code in example_server_blob/bin/data/web
		* example_client_blob can connect to either server!
	* thanks to @zebradog for the start of these examples!
* screen syncing via binary
	* example_particles_server and example_particles_client
		* demonstrates how to sync GPU particles (from the OF GPU particle example) via binary websockets

##A note on license
* libwebsockets is released under a LGPL v2.1, with a special rider that programs that link against a statically built version of the library do not count as derivative, and are not bound to the LGPL v2.1 license conditions. Thus this addon comes with statically built libraries to provide the flexibility of the MIT license.
* https://github.com/warmcat/libwebsockets/blob/master/LICENSE

##Adding to Projects
* OS X
	* Via OF Project Generator:
		1. Delete ofxLibwebsockets/libs/libwebsockets/include/win32port
		2. Add ofxLibwebsockets to your addons via the projectGenerator
		3. That's it!
	* Adding to a new/existing project:
		* Include ofxLibwebsockets' xcconfig file via your project's Project.xcconfig file:
			1. Define where it lives: 
			```OFX_LWS_PATH = "$(OF_PATH)/addons/ofxLibwebsockets"```
			2. Include ofxLibwebsockets xcconfig
			```#include "../../../addons/ofxLibwebsockets/ofxLibwebsockets.xcconfig"```
			2. Add to existing vars in Project.xcconfig:
			
			```
			OTHER_LDFLAGS = $(OF_CORE_LIBS) $(OFX_LWS_LIBS)
			HEADER_SEARCH_PATHS = $(OF_CORE_HEADERS) 
			USER_HEADER_SEARCH_PATHS = $(OFX_LWS_INCLUDES)
			LIBRARY_SEARCH_PATHS = $(inherited) $(OFX_LWS_SEARCH)
			```

* Windows
	* Via OF Project Generator
		1. Add ofxLibwebsockets to your addons
		2. Open your new project
	
	* Adding to new/existing project: 
		1. Add the ofxLibwebsockets source files to the C++ Linker
			* right click on project in the solution explorer, click "Properties", Go down to C++ > General
			* click the arrow at the right of "Additional include directories" and select "edit"
			* add the ofxLibwebsockets source files:
			
			```
			..\..\..\addons\ofxLibwebsockets\libs\jsoncpp
			..\..\..\addons\ofxLibwebsockets\libs\jsoncpp\json
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include\
			..\..\..\addons\ofxLibwebsockets\libs\openssl\openssl
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\include
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\include\ofxLibwebsockets
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\src
			..\..\..\addons\ofxLibwebsockets\src
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include\win32port
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include\win32port\win32helpers
			```


		2. Add the ofxLibwebsockets paths to the "Additional Library Directories":
			* right click on project in the solution explorer, click "Properties", Go down to Linker > General
			* click the arrow at the right of "Additional Library Directories" and select "edit"
			* In your debug scheme, add

			`..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\lib\win32\Debug`
			* In your release scheme, add

			`..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\lib\win32\Release`
			
			* and if you need to use OpenSSL, add:
			
			`..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\libs\openssl\lib\win32`

		3. Now go down to Linker > Input; open the editor "Additional dependencies" the same way
			* Add websockets_static.lib and ZLIB.lib
			
* Linux
	* Use openFramework's projectgenerator and voilà!

## Building libwebockets

ofxLibwebsockets uses [libwebsockets 1.4](https://github.com/warmcat/libwebsockets/releases/tag/v1.4-chrome43-firefox-36)

### OS X

* If you're using El Capitan, make sure you have OpenSSL x86_64 & i386 installed and linked properly. If not, install using [homebrew](http://brew.sh):
```
brew install openssl --universal
brew link openssl --force
```
* Clone libwebsockets
``` 
cd /path/to/libwebsockets
mkdir build
cd build
cmake "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" ..
make
cmake -DCMAKE_INSTALL_PREFIX:PATH=./install . && make install
``` 

### Raspberry Pi

* Clone libwebsockets
``` 
cd /path/to/libwebsockets
mkdir build
cd build
cmake .. -DLWS_IPV6=OFF
make
``` 

### Linux

* Clone libwebsockets
``` 
cd /path/to/libwebsockets
mkdir build
cd build
cmake ..
make
cp lib/libwebsockets.a /path/to/ofxLibwebsocket/libs/libwebsockets/lib/linux64/
```  

### iOS

* Clone libwebsockets
* Clone this repo; it contains compiling resources in ofxLibwebsockets/extras/ios_libwebsockets
	* The toolchain in this folder will allow you to create an iOS XCode project
	* NOTE: CMake is not my specialty, so this file is currently hard-coded to use iOS 8.0... change line 89 if you'd like to use a different iOS
* Create Xcode projects
``` 
cd /path/to/libwebsockets
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/ofxLibwebsockets/extras/ios_libwebsockets/toolchain/iOS.cmake -GXcode ..
``` 
* Replace lws_config.h in your build folder with the one at ofxLibwebsockets/extras/ios_libwebsockets/lws_config.h
* Open the Xcode project in your build folder
* Select the target 'websockets' (instead of ALL_BUILD) and select iOS Device as your target
	* Click on the scheme and select "Edit Scheme"
	* Set the Build Configuration to "Release"
* Build the project
* Copy the lib into place:
```
cd /path/to/your/build/folder
cp lib/Relase/libwebsockets.a /PATH/TO/OF/addons/ofxLibwebsockets/libs/libwebsockets/lib/iosarmv7/libwebsockets.a
```

### Windows (Visual Studio, x64)

* Install OpenSSL
  *  Binaries are available from [Shining Light Productions] (https://slproweb.com/products/Win32OpenSSL.html)
  * If that site is no longer around, try the [OpenSSL Community page](https://www.openssl.org/community/binaries.html)
  * Install [Win64 OpenSSL v1.0.2d](https://slproweb.com/download/Win64OpenSSL-1_0_2d.exe) (Full version, not Light)
  * Use the default location so CMake can find it easily: `C:\OpenSSL-Win64`

* Build libwebsockets
  * libwebsockets' [Build Instructions](https://github.com/warmcat/libwebsockets/blob/master/README.build.md), for reference 
  * Download [libwebsockets v1.3](https://github.com/warmcat/libwebsockets/releases/tag/v1.3-chrome37-firefox30)
  * Open the CMake GUI, `cmake-gui.exe`
  * Create a `build` folder inside the libwebsockets source folder
  * Set paths in CMake GUI to your source & build folders
  * Click Configure
  * Select `Visual Studio 14 2015 Win64` from dropdown, and leave default setting of `Use default native compilers`
  * Click Finish
  * Click Generate
  * Open `build\libwebsockets.sln` from your source folder
  * Compile the `ALL_BUILD` project in Debug and Release modes
  * Copy `websockets_static.lib` and `ZLIB.lib` from `build\lib\[Debug|Release\]` into the proper locations in ofxLibwebsockets. You don't need `websockets.lib` or `websockets.exp`
