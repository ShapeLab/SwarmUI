meta:
	ADDON_NAME = ofxLibwebsockets
	ADDON_DESCRIPTION = openframeworks wrapper for libwebsockets (http://git.warmcat.com/cgi-bin/cgit/libwebsockets/), a C library for creating WebSocket servers and clients
	ADDON_AUTHOR = Brett Renfer
	ADDON_TAGS = "websocket" "libwebsocket" "networking"
	ADDON_URL = https://github.com/labatrockwell/ofxLibwebsockets

common:
	# dependencies with other addons, a list of them separated by spaces 
	# or use += in several lines
	ADDON_DEPENDENCIES =
	
	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	ADDON_INCLUDES = libs/jsoncpp
	ADDON_INCLUDES += libs/jsoncpp/json
	ADDON_INCLUDES += libs/libwebsockets/include/
	ADDON_INCLUDES += libs/ofxLibwebsockets/include
	ADDON_INCLUDES += libs/ofxLibwebsockets/include/ofxLibwebsockets
	ADDON_INCLUDES += libs/ofxLibwebsockets/src
	ADDON_INCLUDES += src

	# any special flag that should be passed to the compiler when using this
	# addon
	ADDON_CFLAGS = 
	
	# any special flag that should be passed to the linker when using this
	# addon, also used for system libraries with -lname
	ADDON_LDFLAGS = 
	
	# linux only, any library that should be included in the project using
	# pkg-config
	ADDON_PKG_CONFIG_LIBRARIES =
	
	# osx/iOS only, any framework that should be included in the project
	ADDON_FRAMEWORKS =
	
	# source files, these will be usually parsed from the file system looking
	# in the src folders in libs and the root of the addon. if your addon needs
	# to include files in different places or a different set of files per platform
	# they can be specified here
	# ADDON_SOURCES = libs/jsoncpp/jsoncpp.cpp
	# ADDON_SOURCES += libs/ofxLibwebsockets/src/ofxLibwebsockets.h
	# ADDON_SOURCES += libs/ofxLibwebsockets/include/ofxLibwebsockets/Server.h
	# ADDON_SOURCES += libs/ofxLibwebsockets/src/Server.cpp
	# ADDON_SOURCES += libs/ofxLibwebsockets/include/ofxLibwebsockets/Reactor.h
	# ADDON_SOURCES += libs/ofxLibwebsockets/src/Reactor.cpp
	# ADDON_SOURCES += libs/ofxLibwebsockets/include/ofxLibwebsockets/Protocol.h
	# ADDON_SOURCES += libs/ofxLibwebsockets/src/Protocol.cpp
	# ADDON_SOURCES += libs/ofxLibwebsockets/include/ofxLibwebsockets/Events.h
	# ADDON_SOURCES += libs/ofxLibwebsockets/src/Events.cpp
	# ADDON_SOURCES += libs/ofxLibwebsockets/include/ofxLibwebsockets/Connection.h
	# ADDON_SOURCES += libs/ofxLibwebsockets/src/Connection.cpp
	# ADDON_SOURCES += libs/ofxLibwebsockets/include/ofxLibwebsockets/Client.h
	# ADDON_SOURCES += libs/ofxLibwebsockets/src/Client.cpp
	# ADDON_SOURCES += libs/ofxLibwebsockets/include/ofxLibwebsockets/Util.h
	# ADDON_SOURCES += libs/ofxLibwebsockets/src/Util.cpp

	# some addons need resources to be copied to the bin/data folder of the project
	# specify here any files that need to be copied, you can use wildcards like * and ?
	ADDON_DATA = 
	
	# when parsing the file system looking for libraries exclude this for all or
	# a specific platform
	ADDON_LIBS_EXCLUDE =
	
linux64:
	# binary libraries, these will be usually parsed from the file system but some 
	# libraries need to passed to the linker in a specific order 
	#nothing yet
	
linux:
	#nothing yet
	
win_cb:
	#nothing yet

vs:
	# source files, these will be usually parsed from the file system looking
	# in the src folders in libs and the root of the addon. if your addon needs
	# to include files in different places or a different set of files per platform
	# they can be specified here
	ADDON_SOURCES += 

	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=

	# when parsing the file system looking for include paths exclude this for all or
	# a specific platform
	ADDON_INCLUDES_EXCLUDE
	
	# Win32/Debug
	# ADDON_LIBS  = libs/libwebsockets/lib/vs/Win32/Debug/websockets.lib
	# ADDON_LIBS += libs/libwebsockets/lib/vs/Win32/Debug/zlib_internal.lib
	# Win32/Release
	# ADDON_LIBS  = libs/libwebsockets/lib/vs/Win32/Release/websockets.lib
	# ADDON_LIBS += libs/libwebsockets/lib/vs/Win32/Release/zlib_internal.lib
	# x64/Debug 
	# ADDON_LIBS  = libs/libwebsockets/lib/vs/x64/Debug/websockets_static.lib
	# ADDON_LIBS += libs/libwebsockets/lib/vs/x64/Debug/ZLIB.lib
	# x64/Release 
	# ADDON_LIBS  = libs/libwebsockets/lib/vs/x64/Release/websockets_static.lib
	# ADDON_LIBS += libs/libwebsockets/lib/vs/x64/Release/ZLIB.lib


linuxarmv6l:
	ADDON_LDFLAGS = -lssl
	ADDON_SOURCES_EXCLUDE = libs/libwebsockets/include/win32port/%
	ADDON_LIBS = libs/libwebsockets/lib/linuxarmv6l/libwebsockets.a
linuxarmv7l:
	ADDON_LDFLAGS = -lssl
	ADDON_SOURCES_EXCLUDE = libs/libwebsockets/include/win32port/%
	ADDON_LIBS = libs/libwebsockets/lib/linuxarmv7l/libwebsockets.a
	
android/armeabi:	
	#nothing yet
	
android/armeabi-v7a:	
	#nothing yet

osx:
	ADDON_INCLUDES_EXCLUDE = libs/libwebsockets/include/win32port
	ADDON_INCLUDES_EXCLUDE += libs/libwebsockets/include/win32port/win32helpers
	# OpenSSL support for OSX 10.11+
	ADDON_INCLUDES += ../../../libs/openssl/include
	ADDON_LIBS += ../../../libs/openssl/lib/osx/libcrypto.a
    ADDON_LIBS += ../../../libs/openssl/lib/osx/libssl.a

ios:
	ADDON_INCLUDES_EXCLUDE = libs/libwebsockets/include/win32port/.*
	ADDON_INCLUDES_EXCLUDE += libs/libwebsockets/include/win32port/win32helpers/.*
	ADDON_SOURCES_EXCLUDE = libs/libwebsockets/include/win32port/.*
	ADDON_LIBS = libs/libwebsockets/lib/iosarmv7/libwebsockets.a
