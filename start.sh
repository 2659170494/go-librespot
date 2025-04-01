echo "此脚本为Codespace专用！！！"
echo "即将开始编译安卓动态库go-librespot"
echo "在开始之前如果没有装过android sdk，请先运行：sudo bash install-JDK-Android-SDK.sh"
echo "如果装过，则运行：export ANDROID_SDK_ROOT=/workspaces/go-librespot/build-tools/android-sdk && export ANDROID_HOME=/workspaces/go-librespot/build-tools/android-sdk && export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64"
echo "这将会删除本目录下的SRC和TMP文件夹！！！"
echo "编译成功后，建议将src和tmp/deps文件夹复制到您的安卓设备上以备份编译结果"
echo "若要运行程序，请将src的go-librespot-armv6_android"
echo "和tmp/deps/arm-linux-androideabi/lib下的libogg.so以及libasound.so"
echo "还有tmp/deps/arm-linux-androideabi/share/alsa下的alsa.conf复制到/data/local/tmp中。"
echo '并在shell设置: export ALSA_CONFIG_PATH="/data/local/tmp/alsa.conf" && export LD_LIBRARY_PATH=/data/local/tmp/ && export XDG_CONFIG_HOME=/data/local/tmp/ '
echo "关于pulseaudio，你可以下载Simple Protocol Player然后设置ip和端口并打开网络连接进行对接，"
echo "或者下载termux，并在termux执行apt install pulseaudio"
#https://www.reddit.com/r/termux/comments/162w750/please_help_me_set_up_pulseaudio/
echo "之后执行：pulseaudio --start --exit-idle-time=-1 && pacmd load-module module-native-protocol-tcp auth-ip-acl=127.0.0.1 auth-anonymous=1"
echo "然后重启termux，再执行一遍上面的命令。回到shell里执行后即可食用：export PULSE_SERVER=127.0.0.1"
echo "安装成功后，如果执行pulseaudio提示libogg.so not found"
echo "则将本脚本的arm-linux-androideabi和armv7a-linux-androideabi改成aarch64-linux-android"
echo "之后运行脚本，将编译后的./tmp/deps/aarch64-linux-android/lib/libogg.so复制到手机的/sdcard/下"
echo "之后在termux里执行mv /sdcard/libogg.so ./ && export LD_LIBRARY_PATH=/data/data/com.termux/files/home/"
echo "请等待3秒！"
sleep 3;
echo "按下回车后继续。。。";
read -n 1 -s ;
export WORK_DIR=/workspaces/go-librespot
export HOOK_DIR=${WORK_DIR}/hook
export TMEP_DIR=${WORK_DIR}/tmp
export SRC_DIR=${WORK_DIR}/src
export ANDROID_SDK_ROOT=/workspaces/go-librespot/build-tools/android-sdk
export ANDROID_HOME=/workspaces/go-librespot/build-tools/android-sdk 
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
export NDK=${ANDROID_SDK_ROOT}/ndk/26.3.11579264
#export NDK=${ANDROID_SDK_ROOT}/ndk-bundle/toolchains
export COMMIT=dev
export VERSION="0.0.0"
# export GOAMD64=v1
export GOCACHE=${SRC_DIR}/.gocache/go-build
export GOMODCACHE=${SRC_DIR}/.gocache/mod
export CGO_ENABLED=1

#export PATH=${PATH}:${ANDROID_SDK_ROOT}/ndk/21.3.6528147/toolchains/arm-linux-androideabi-4.9/bin/ #arm32
#export PATH=${PATH}:${ANDROID_SDK_ROOT}/ndk/21.3.6528147/toolchains/aarch64-linux-android-4.9/bin/ #arm64
#export PATH=${PATH}:${ANDROID_SDK_ROOT}/ndk/21.3.6528147/toolchains/llvm/prebuilt/linux-x86_64/bin/ #通用

sudo rm -rf ${TMEP_DIR}
sudo rm -rf ${SRC_DIR}
cd ${WORK_DIR} && mkdir tmp
cd ${WORK_DIR} && mkdir src 
sudo apt-get install libogg-dev libvorbis-dev libasound2-dev
sudo apt-get install autopoint makeinfo texinfo
# sudo dpkg -S libogg-dev 
# sudo dpkg -L libogg-dev 
# sudo apt-cache show libogg-dev 
# echo "按下回车后继续。。。";
# read -n 1 -s ;
# sudo dpkg -S libvorbis-dev 
# sudo dpkg -L libvorbis-dev 
# sudo apt-cache show libvorbis-dev 
# echo "按下回车后继续。。。";
# read -n 1 -s ;
# sudo dpkg -S libasound2-dev
# sudo dpkg -L libasound2-dev
# sudo apt-cache show libasound2-dev
# echo "按下回车后继续。。。";
# read -n 1 -s ;

sudo apt-get install -y gcc-x86-64-linux-gnu gcc-arm-linux-gnueabihf gcc-aarch64-linux-gnu
cd ${TMEP_DIR} && wget http://www.alsa-project.org/files/pub/lib/alsa-lib-1.2.10.tar.bz2 
tar -xvf alsa-lib-1.2.10.tar.bz2 && cd ${TMEP_DIR}
cd ${TMEP_DIR} && wget https://downloads.xiph.org/releases/ogg/libogg-1.3.5.tar.xz 
tar -xvf libogg-1.3.5.tar.xz && cd ${TMEP_DIR}
cd ${TMEP_DIR} && wget https://downloads.xiph.org/releases/vorbis/libvorbis-1.3.7.tar.xz 
tar -xvf libvorbis-1.3.7.tar.xz && cd ${TMEP_DIR}

export TARGET=arm-linux-gnueabihf
export T_TARGET=gcc-arm-linux-gnueabihf
export API=0
export GOOS=linux
export PKG_CONFIG_PATH=${TMEP_DIR}/deps/${TARGET}/lib/pkgconfig/
export TOOLCHAIN=/usr/$TARGET
export AR=$TOOLCHAIN/bin/ar
export CC="/usr/bin/${TARGET}-gcc"
export AS=$CC
export CXX="/usr/bin/${TARGET}-gcc"
export CPP="/usr/bin/${TARGET}-gcc"
export LD=$TOOLCHAIN/bin/ld
export RANLIB=$TOOLCHAIN/bin/ranlib
export STRIP=$TOOLCHAIN/bin/strip
export GOARCH=arm
export GOARM=6 
export GOOUTSUFFIX=-armv6
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${TMEP_DIR}/deps/${TARGET}/lib/
export LIBRARY_PATH=${LIBRARY_PATH}:${TMEP_DIR}/deps/${TARGET}/lib/

echo "动态库编译环境："
echo TARGET=${TARGET}
echo T_TARGET=${T_TARGET}
echo API=${API}
echo GOOS=${GOOS}
echo PKG_CONFIG_PATH=${PKG_CONFIG_PATH}
echo TOOLCHAIN=${TOOLCHAIN}
echo AR=${AR}
echo CC=${CC}
echo AS=${AS}
echo CXX=${CXX}
echo CPP=${CXX}
echo LD=${LD}
echo RANLIB=${RANLIB}
echo STRIP=${STRIP}
echo GOARCH=${GOARCH}
echo GOARM=${GOARM}
echo GOOUTSUFFIX=${GOOUTSUFFIX}
echo LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
echo LIBRARY_PATH=${LIBRARY_PATH}
# echo "按下回车后继续。。。";
# read -n 1 -s ;

cd ${TMEP_DIR}/alsa-lib-1.2.10 
sudo make clean
sudo ./configure --enable-shared=yes --enable-static=no --with-pic --without-versioned --host=${TARGET} --prefix=${TMEP_DIR}/deps/${TARGET}
sudo make && sudo make install && cd ${TMEP_DIR} 
# echo "按下回车后继续。。。";
# read -n 1 -s ;
cd ${TMEP_DIR}/libogg-1.3.5 
sudo make clean
sudo ./configure --enable-shared=yes --enable-static=no --host=${TARGET} --prefix=${TMEP_DIR}/deps/${TARGET} CFLAGS="-shared -lgcc -Wimplicit-function-declaration -L${TMEP_DIR}/deps/${TARGET}/lib/ " #-nostartfiles 
sudo make && sudo make install && cd ${TMEP_DIR} 
# echo "按下回车后继续。。。";
# read -n 1 -s ;
cd ${TMEP_DIR}/libvorbis-1.3.7 
sudo make clean
sudo ./configure --enable-shared=no --enable-static=yes --host=${TARGET} --prefix=${TMEP_DIR}/deps/${TARGET} CFLAGS="-static -lgcc -Wimplicit-function-declaration -L${TMEP_DIR}/deps/${TARGET}/lib/ "  #-nostartfiles 
sudo make && sudo make install && cd ${TMEP_DIR} 
# echo "按下回车后继续。。。";
# read -n 1 -s ;

cd ${WORK_DIR} && cd ${SRC_DIR}
go clean -r && CGO_LDFLAGS="-v -lm -logg -L${TMEP_DIR}/deps/${TARGET}/lib/ " CC="/usr/bin/${TARGET}-gcc" go build -buildvcs=false -ldflags="-X github.com/devgianlu/go-librespot.commit=${COMMIT} -X github.com/devgianlu/go-librespot.version=${VERSION} " -o ./go-librespot${GOOUTSUFFIX} -a ${WORK_DIR}/cmd/daemon
# echo "按下回车后继续。。。";
# read -n 1 -s ;

export AD_TARGET=arm-linux-androideabi
# export AD_TARGET=aarch64-linux-android
export T_AD_TARGET=armv7a-linux-androideabi
# export T_AD_TARGET=aarch64-linux-android
export API=21
export GOOS=android
export PKG_CONFIG_PATH=${TMEP_DIR}/deps/${AD_TARGET}/lib/pkgconfig/
export TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/linux-x86_64
export AR=$TOOLCHAIN/bin/llvm-ar
export CC="$TOOLCHAIN/bin/clang --target=$T_AD_TARGET$API "
export AS=$CC
export CXX="$TOOLCHAIN/bin/clang++ --target=$T_AD_TARGET$API "
export CPP="$TOOLCHAIN/bin/clang -E --target=$T_AD_TARGET$API "
export LD=$TOOLCHAIN/bin/ld
export RANLIB=$TOOLCHAIN/bin/llvm-ranlib
export STRIP=$TOOLCHAIN/bin/llvm-strip
export GOARCH=arm
# export GOARCH=arm64
export GOARM=6 
# export GOARM=""
export GOOUTSUFFIX=-armv6_android
# export GOOUTSUFFIX=-aarch64_android
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${TMEP_DIR}/deps/${AD_TARGET}/lib/
export LIBRARY_PATH=${LIBRARY_PATH}:${TMEP_DIR}/deps/${AD_TARGET}/lib/

echo "安卓编译环境："
echo AD_TARGET=${AD_TARGET}
echo T_AD_TARGET=${T_AD_TARGET}
echo API=${API}
echo GOOS=${GOOS}
echo PKG_CONFIG_PATH=${PKG_CONFIG_PATH}
echo TOOLCHAIN=${TOOLCHAIN}
echo AR=${AR}
echo CC=${CC}
echo AS=${AS}
echo CXX=${CXX}
echo CPP=${CXX}
echo LD=${LD}
echo RANLIB=${RANLIB}
echo STRIP=${STRIP}
echo GOARCH=${GOARCH}
echo GOARM=${GOARM}
echo GOOUTSUFFIX=${GOOUTSUFFIX}
echo LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
echo LIBRARY_PATH=${LIBRARY_PATH}
# echo "按下回车后继续。。。";
# read -n 1 -s ;
cp -r ${HOOK_DIR}/* ${TMEP_DIR}/

#https://blog.csdn.net/qq_17256689/article/details/131773176
#https://developer.android.com/ndk/guides/symbol-visibility?hl=zh-cn
#https://blog.csdn.net/k663514387/article/details/107107350
cd ${TMEP_DIR}/alsa-lib-1.2.10 
sudo make clean
sudo ./configure --enable-shared=yes --enable-static=no --with-pic --without-versioned --host=${AD_TARGET} --prefix=${TMEP_DIR}/deps/${AD_TARGET} LDFLAGS="-v " CC="$TOOLCHAIN/bin/clang --target=$T_AD_TARGET$API " CFLAGS="-Wint-conversion "
sudo make && sudo make install && cd ${TMEP_DIR} 
# echo "按下回车后继续。。。";
# read -n 1 -s ;
cd ${TMEP_DIR}/libogg-1.3.5 
sudo make clean
sudo ./configure --enable-shared=yes --enable-static=no --host=${AD_TARGET} --prefix=${TMEP_DIR}/deps/${AD_TARGET} LDFLAGS="-v " CC="$TOOLCHAIN/bin/clang --target=$T_AD_TARGET$API " CFLAGS="-shared " # -lgcc -Wimplicit-function-declaration -nostartfiles -Wl,--unresolved-symbols=ignore-all
sudo make && sudo make install && cd ${TMEP_DIR} 
# echo "按下回车后继续。。。";
# read -n 1 -s ;
cd ${TMEP_DIR}/libvorbis-1.3.7 
sudo make clean
sudo ./configure --enable-shared=no --enable-static=yes --host=${AD_TARGET} --prefix=${TMEP_DIR}/deps/${AD_TARGET} LDFLAGS="-v -L${TMEP_DIR}/deps/${AD_TARGET}/lib/ " CC="$TOOLCHAIN/bin/clang --target=$T_AD_TARGET$API " CFLAGS="-static "  # -lgcc -Wimplicit-function-declaration -nostartfiles -Wl,--unresolved-symbols=ignore-all
sudo make && sudo make install && cd ${TMEP_DIR} 
# echo "按下回车后继续。。。";
# read -n 1 -s ;

cd ${WORK_DIR} && cd ${SRC_DIR}
go clean -r && CGO_LDFLAGS="-lm -L${TMEP_DIR}/deps/${AD_TARGET}/lib/ " CC="$TOOLCHAIN/bin/clang --target=$T_AD_TARGET$API " go build -buildvcs=false -ldflags="-X github.com/devgianlu/go-librespot.commit=${COMMIT} -X github.com/devgianlu/go-librespot.version=${VERSION} " -o ./go-librespot${GOOUTSUFFIX} -a ${WORK_DIR}/cmd/daemon #-Wl,--unresolved-symbols=ignore-all

#go run ${WORK_DIR}/cmd/daemon/main.go

#buf generate
#go generate ./...

