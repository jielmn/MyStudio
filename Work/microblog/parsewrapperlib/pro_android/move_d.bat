echo =============Move Debug Library Start

del ..\..\..\..\10-common\lib\debug\android\libWBParseWrapper.a /q
move /y .\obj\local\armeabi\libWBParseWrapper.a ..\..\..\..\10-common\lib\debug\android\libWBParseWrapper.a

echo =============Move Debug Library End