#!/bin/sh

#  build.Script.sh
#  xxxx
#
#  Created by michael on 12-1-16.
#  Copyright (c) 2012年 xxxx. All rights reserved.

libsDir="../../../../10-common/lib/debug/ios"

#rm -f "${libsDir}/libmtservice.a"
projectDir="./"
projectName="WBParseWrapper"
productName="parsewrapper"
targetName="lib${productName}"
echo "***开始build .a***"
xcodebuild clean -project "${projectDir}${projectName}.xcodeproj" -configuration Debug -sdk iphoneos4.2
xcodebuild -project "${projectDir}${projectName}.xcodeproj" -target "${projectName}" -configuration Debug -sdk iphoneos4.2
appfile="${projectDir}build/Debug-iphoneos/${targetName}.a"
cp ${appfile} ${libsDir}

