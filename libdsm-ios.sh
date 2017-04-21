#!/bin/bash

# Global build settings
export SDKPATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
export SIMSDKPATH=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk
export MIN_IOS_VERSION=8.0
export HOST=arm-apple-darwin
export LDFLAGS_NATIVE="-isysroot $SDKPATH"
export LDFLAGS_SIMULATOR="-isysroot $SIMSDKPATH"
export TASN1_CFLAGS="-Ilibtasn1/include"
export TASN1_LIBS="-Llibtasn1 -ltasn1"
export ARCHES=(armv7 armv7s arm64 i386 x86_64)


######################################################################
#Build libdsm

rm -rf build

build_files=""
for i in "${ARCHES[@]}"
do
	build_files="$build_files build/$i/lib/libdsm.a"
	export ARCH=$i
	if [[ $i == *"arm"* ]]
	then
		export LDFLAGS=$LDFLAGS_NATIVE
	else
		export LDFLAGS=$LDFLAGS_SIMULATOR
	fi
	export CFLAGS="-arch $ARCH $LDFLAGS -miphoneos-version-min=$MIN_IOS_VERSION -fembed-bitcode -DNDEBUG"
	./bootstrap
	./configure --host=$HOST --prefix=$PWD/build/$ARCH && make && make install
	make clean
done

#Merge the compiled binaries into a single universal one
mkdir -p build/universal
lipo -create $build_files -output build/universal/libdsm.a

#Copy headers across
mkdir build/universal/include
cp -R build/armv7/include build/universal

#Move final product to parent directory
cp -R build/universal ../libdsm
