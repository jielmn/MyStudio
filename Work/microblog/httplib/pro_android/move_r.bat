echo =============Move Release Library Start

del ..\..\..\..\10-common\lib\release\android\libhttp.a
move /y .\obj\local\armeabi\libhttp.a ..\..\..\..\10-common\lib\release\android\libhttp.a

echo =============Move Release Library End