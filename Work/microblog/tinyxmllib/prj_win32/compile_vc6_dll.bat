echo build clean debug
del /F /Q debug\*.*
@msdev tinyxmldll.dsp /MAKE "tinyxmldll - Win32 Debug" /REBUILD /NORECURSE /OUT ..\..\..\..\10-common\version\compileInfo\tinyxml_vc6dll_d.txt

echo build clean release
del /F /Q release\*.*
@msdev tinyxmldll.dsp /MAKE "tinyxmldll - Win32 Release" /REBUILD /NORECURSE /OUT ..\..\..\..\10-common\version\compileInfo\tinyxml_vc6dll_r.txt

echo tinyxmldll build over