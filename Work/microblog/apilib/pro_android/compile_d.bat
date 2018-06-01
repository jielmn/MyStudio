echo =============Compile Debug Version Start

del .\obj\local\armeabi\libWBapi.a /q
rd  /s /q .\obj\local\armeabi\objs-debug
D:\android\android-ndk-r7c\ndk-build NDK_DEBUG=1

echo =============Compile Debug Version End