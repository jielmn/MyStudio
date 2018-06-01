echo =============Move Debug Library Start

del ..\..\..\..\10-common\lib\debug\android\libcurl.a /q
move /y .\obj\local\armeabi\libcurl.a ..\..\..\..\10-common\lib\debug\android\libcurl.a

echo =============Move Debug Library End