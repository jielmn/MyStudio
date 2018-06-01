echo =============Move Release Library Start

del ..\..\..\..\10-common\lib\release\android\libWBapi.a
move /y .\obj\local\armeabi\libWBapi.a ..\..\..\..\10-common\lib\release\android\libWBapi.a

echo =============Move Release Library End