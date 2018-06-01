echo =============Move Debug Library Start

del ..\..\..\..\10-common\lib\debug\android\libtinyxml.a /q
move /y .\obj\local\armeabi\libtinyxml.a ..\..\..\..\10-common\lib\debug\android\libtinyxml.a

echo =============Move Debug Library End