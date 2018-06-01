echo =============Move Release Library Start

del ..\..\..\..\10-common\lib\release\android\libtinyxml.a
move /y .\obj\local\armeabi\libtinyxml.a ..\..\..\..\10-common\lib\release\android\libtinyxml.a

echo =============Move Release Library End