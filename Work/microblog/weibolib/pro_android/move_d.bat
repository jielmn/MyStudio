echo =============Move Debug Library Start

del ..\..\..\..\10-common\lib\debug\android\libweibo.a /q
move /y .\obj\local\armeabi\libweibo.a ..\..\..\..\10-common\lib\debug\android\libweibo.a

echo =============Move Debug Library End