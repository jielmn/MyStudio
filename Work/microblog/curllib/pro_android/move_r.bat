echo =============Move Release Library Start

del ..\..\..\..\10-common\lib\release\android\libcurl.a
move /y .\obj\local\armeabi\libcurl.a ..\..\..\..\10-common\lib\release\android\libcurl.a

echo =============Move Release Library End