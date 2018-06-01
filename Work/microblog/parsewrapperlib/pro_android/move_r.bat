echo =============Move Release Library Start

del ..\..\..\..\10-common\lib\release\android\libWBParseWrapper.a
move /y .\obj\local\armeabi\libWBParseWrapper.a ..\..\..\..\10-common\lib\release\android\libWBParseWrapper.a

echo =============Move Release Library End