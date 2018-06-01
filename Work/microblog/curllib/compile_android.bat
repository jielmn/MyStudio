echo off
echo =============Compile curl Begin====================

setlocal
PATH=D:\android\android-ndk-r7c
cd .\pro_android
call compile_r.bat
call move_r.bat

call compile_d.bat
call move_d.bat
cd ..\
endlocal

echo =============Compile curl End====================
echo on
