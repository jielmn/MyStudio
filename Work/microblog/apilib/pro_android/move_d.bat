echo =============Move Debug Library Start

del ..\..\..\..\10-common\lib\debug\android\libWBapi.a /q
move /y .\obj\local\armeabi\libWBapi.a ..\..\..\..\10-common\lib\debug\android\libWBapi.a

echo =============Move Debug Library End