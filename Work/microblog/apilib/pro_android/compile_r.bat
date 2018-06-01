echo =============Compile Release Version Start

del .\obj\local\armeabi\libWBapi.a /q
rd  /s /q .\obj\local\armeabi\objs
D:\android\android-ndk-r7c\ndk-build NDK_DEBUG=0

echo =============Compile Release Version End
