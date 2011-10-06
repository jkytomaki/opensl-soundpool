BUILDING
--------
- requirements: Android SDK, Android NDK r6b
- If using CDT, correct the project and ndk-sdk locations in .cproject and .project files
- On my Win7 x64 box, the first build fails because of a permission error with obj/local/armeabi/libstlport_static.a. Chmod 777:ing tihe file with cygwin seems to fix the problem.

