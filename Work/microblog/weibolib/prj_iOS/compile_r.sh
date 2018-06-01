#!/bin/sh

#  build.Script.sh
#  xxxx
#
#  Created by michael on 12-1-16.
#  Copyright (c) 2012年 xxxx. All rights reserved.

libsDir="../../../../10-common/lib/release/ios"

#rm -f "${libsDir}/libmtservice.a"
projectDir="./"
projectName="WeiBo"
productName="weibo"
targetName="lib${productName}"
echo "***开始build .a***"
xcodebuild clean -project "${projectDir}${projectName}.xcodeproj" -configuration Release -sdk iphoneos4.2
xcodebuild -project "${projectDir}${projectName}.xcodeproj" -target "${projectName}" -configuration Release -sdk iphoneos4.2
appfile="${projectDir}build/Release-iphoneos/${targetName}.a"
cp ${appfile} ${libsDir}

