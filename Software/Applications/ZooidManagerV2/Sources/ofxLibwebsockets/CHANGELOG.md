#CHANGELOG
* Version 0.8.4 (current)
  * - No longer including OpenSSL (compiled into libwebsockets) 
  * + Added iOS library
  * + Added RPi library (thanks to @rc1)
  * + Added libwebsockets' rudimentary keep-alive
    * See [issue #38](https://github.com/labatrockwell/ofxLibwebsockets/issues/38)
  * \ Updated to latest version of libwebsockets
  * \ Bug fixes for newest libwebsockets version
  * \ testApp changed to ofApp
  * \ Better comments in general
  * \ Removed unnecessary checks for binary support (always enabled in libwebsockets)
  * \ Removed "of-protocol" protocol from examples (you don't need it!)
  * + Added adjustable image quality to binary video example for demonstration
* Version 0.8.1 
  * Overhauled all messaging, supporting sending/receiving string and binary messages of indefinite size via continuation frames
  * Added examples from @zebradog of sending Blob data over websockets: way faster, way cleaner!
  * Lots of cleanup of excessive logging, possible crashes on sending
  * Updated to latest libwebsockets on Mac and Windows
  * Cleaned up and fixed up Windows support
  * Tested on OF 0.8.0, 0.8.1 on Mac and Windows
  * Removed RPi and Linux64 until someone recompiles libwebsockets!!!
* Version 0.8.0
  * Basic support for binary data
  * Updated to latest libwebsockets
