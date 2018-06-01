echo =============Move Release Library Start

del ..\..\..\..\10-common\lib\release\android\libweibo.a
move /y .\obj\local\armeabi\libweibo.a ..\..\..\..\10-common\lib\release\android\libweibo.a

echo =============Move Release Library End