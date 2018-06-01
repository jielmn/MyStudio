echo =============Move Debug Library Start

del ..\..\..\..\10-common\lib\debug\android\libhttp.a /q
move /y .\obj\local\armeabi\libhttp.a ..\..\..\..\10-common\lib\debug\android\libhttp.a

echo =============Move Debug Library End