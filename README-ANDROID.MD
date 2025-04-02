# 编译方法
首先，如果你是在Github Codespace下运行本项目的话，
你可以在本目录运行`sudo bash ./install-JDK-Android-SDK.sh`自动安装并设置Android SDK & JDK还有Android NDK。

否则，你需要自己手动配置这些，然后设置环境变量：
`export ANDROID_SDK_ROOT=<你的Android SDK环境> && export ANDROID_HOME=<你的Android SDK环境> && export JAVA_HOME=<你的JDK环境>`

当你配置完成后，即可运行`./start.sh`开始自动编译程序。

在start.sh中，它会自动编译两种平台的程序。**第一种是arm32位的linux(TARGET=arm-linux-gnueabihf)，第二种是arm32位的android(AD_TARGET=arm-linux-androideabi)**

要注意的是，start.sh默认编译的是arm32位安卓，如果你需要编译arm64位安卓，则需要将 **第二种平台\(带有AD\_TARGET变量\)** 的变量修改成：
```
export AD_TARGET=aarch64-linux-android
export T_AD_TARGET=aarch64-linux-android
export GOARCH=arm64
export GOARM=""
export GOOUTSUFFIX=-aarch64_android
```
反之arm32位安卓的变量是：
```
export AD_TARGET=arm-linux-androideabi
export T_AD_TARGET=armv7a-linux-androideabi
export GOARCH=arm
export GOARM=6 
export GOOUTSUFFIX=-armv6_android
```
**这一步很重要，如果你需要在Termux下运行，那么你的编译架构必须和你的手机架构一致。否则编译出来的程序只能在`adb shell`使用**

当你编译完成后，本目录下会出现src和tmp目录。src目录包含编译的二进制相关文件，而tmp包含动态库相关文件。你可以根据自己需要，自行导出需要的文件。

编译好的程序就在`./src`下，如果你没有修改过start.sh的T_AD_TARGET或T_TARGET变量的话，默认`go-librespot-armv6_android`是32位安卓的二进制程序，`go-librespot-armv6`是32位linux的二进制程序

而二进制的动态库在`./tmp/deps/<你编译的平台>/lib`下，最佳建议就是导出`libogg.so`和`libasound.so`。

出于ALSA-lib原因，如果你需要导出`libasound.so`的话，你还需要在`./tmp/deps/<你编译的平台>/share/alsa`下导出`alsa.conf`，它将会搭配环境变量`ALSA_CONFIG_PATH`使用

## 使用方法（安卓）

当你导出好你需要的文件的时候，你就可以开始接下来的工作了。

将你的程序还有动态库文件复制到你的手机，并记录好它们的位置。

如果你不打算用termux运行的话（本案例围绕`adb shell`展开），我个人推荐将程序和动态库文件以及配置文件都复制到`/data/local/tmp/`

如果你导出了上面的所需的文件，那么你应该会拥有`go-librespot-armv6_android`(程序本体)、`libogg.so`(libogg动态库，termux下可选)、`libasound.so`(alsa-lib动态库，termux下可选)、`alsa.conf`(alsa-lib配置文件，拥有alsa-lib时才需要)

之后，在你的终端里设置`LD_LIBRARY_PATH`环境变量（例如`libogg.so`在`/data/local/tmp/lib/libogg/`下，`libasound.so`在`/data/local/tmp/lib/libasound/`下。则执行`export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/data/local/tmp/lib/libogg/:/data/local/tmp/lib/libasound/`。如果`libogg.so`和`libasound.so`都在`/data/local/tmp/`下，则执行`export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/data/local/tmp/`。）

然后，设置`ALSA_CONFIG_PATH`环境变量（例如你的`alsa.conf`保存在`/data/local/tmp`目录下，则你需要执行`export ALSA_CONFIG_PATH="/data/local/tmp/alsa.conf"`）

最后，设置`XDG_CONFIG_HOME`环境变量，这个地方将会设置go-librespot的配置存放位置（例如执行`export XDG_CONFIG_HOME=/data/local/tmp/`之后，go-librespot的配置将会保存在`/data/local/tmp/go-librespot/`中）

当一切都配置好后，不出意外的话，你就可以开始使用了！

**如果你遇到了问题，你可以自己查看源代码解决。**

## 关于Pulseaudio(安卓环境下的声音输出)
由于安卓驱动以及权限原因，**ALSA-lib是无法在安卓环境下运行的**

当不配置config.yml的情况下，安卓环境下运行程序会提示：
```
!!!ALAS ONLY SUPPORT INSTALLED ALAS DRIVER AND RUNNING PROGRAM IN TERMUX OF ROOT ANDROID DEVICE!!!
!!!IF YOU DONT OR HAVEN'T,PLEASE ADD OR CHANGE "audio_backend" TO "pulseaudio" or "pipe" in $XDG_CONFIG_HOME/go-librespot/config.yml !!!
```

所以，如果我们想要在Android播放声音，我们就得借助**Termux**了。

首先，你需要下载并安装三个软件：`Simple Protocol Player`、`Termux`和`Termux:API`。其中`Termux:API`可以在`Termux`的设置中下载安装，避免出现版本不匹配的情况。

当三个软件都安装成功后，**强烈建议给这三个软件允许全部权限**。在我测试的时候我大多数都是挂在权限设置的问题上。

给予必要的权限后，打开你的Termux并执行以下命令安装软件包：`pkg install termux-x11 termux-api pulseaudio mplayer mpv`

安装成功后**重启`Termux`**,之后执行：`export PULSE_SERVER=tcp:127.0.0.1:4713`。这将会设置默认的Pulseaudio连接地址，随后执行`pactl list sources short`。

**如果你在这一步或者任意步骤**出现了`Connection failure: Connection refused`，则需要先执行`pulseaudio --start --disallow-exit --exit-idle-time=-1`，之后再执行那条命令。

如果又出现了`Daemon startup failed`，那么执行`pulseaudio -k`，然后再试一次。

如果还是不行的话，终极方案就是将本项目中的`./hook/pulse/default.pa`复制到手机的`/data/data/com.termux/files/usr/etc/pulse/`，然后重启Termux。但是这样做会覆盖你原先的Pulseaudio的配置，不过可以解决你在本篇文章可能无法遇到的奇怪问题。如果你不想这么做，你可以自行执行`pulseaudio -v &`或`pulseaudio -vvvv &`查看日志（一般这种情况只发生在第一次安装使用的时候，详情请见： https://askubuntu.com/questions/225444/how-to-make-pulseaudio-work-again 和 https://github.com/CypherpunkArmory/UserLAnd/issues/135 ）

当你执行完`pactl list sources short`之后，你能看到一个字符串被输出，不出意外的话你的终端大概长这样：
```
~ $ pactl list sources short
0   auto_null.monitor   module-null-sink.c  s16le   2ch 44100Hz IDLE
~ $ ▮
```
**记住上面的内容，然后按照下面的例子对比上面的信息，自行修改成适合你设备的命令。每个人所适用的参数配置都是不同的！**
```
export PULSE_SERVER=tcp:127.0.0.1:4713 && pactl load-module module-simple-protocol-tcp rate=44100 format=s16le channels=2 source=auto_null.monitor record=true port=6951 listen=127.0.0.1
```
不懂的话，上面的命令大概对应的内容是这样的：

`
export PULSE_SERVER=tcp:<Pulseaudio服务器地址><:端口,默认4713> && pactl load-module module-simple-protocol-tcp rate=<44100Hz> format=<s16le> channels=<2ch> source=<auto_null.monitor> record=true port=6951 listen=127.0.0.1
`

修改完成后并执行，如果没有任何异常并且输出数字的话就是成功了的意思：
```
~ $ pactl list sources short
0   auto_null.monitor   module-null-sink.c  s16le   2ch 44100Hz IDLE
~ $ export PULSE_SERVER=tcp:127.0.0.1:4713 && pactl load-module module-simple-protocol-tcp rate=44100 format=s16le channels=2 source=auto_null.monitor record=true port=6951 listen=127.0.0.1
16
~ $ ▮
```

之后打开`Simple Protocol Player`软件，在`IP Address / Hostname`这一边填入`127.0.0.1`，接着在`Audio Port`这边填入`6951`，然后在`Sample Rate`这边你可以设置成跟上面命令一样的参数(对应本案例则是`44100`)，之后在`Buffer size (in ms)`这边设置成`200`，最后勾选`Performance mode`，点击启动按钮。**如果出现了`Unable to stream`，请检查Termux是不是被杀后台了，如果没有的话请在Termux执行`exit`退出Termux然后重新走一遍该小节（没必要在继续下去，这个时候的Pulseaudio百分百是乱套的，信我你不会想因为这个浪费几个小时的😭😭😭）**

**之后记得锁定`Simple Protocol Player`的后台**，如果你的手机有音乐文件（除了网易云QQ音乐之类的会员加密格式）的话，可以通过执行`mplayer <音乐文件路径>`进行测试，能听到声音就是成功。**不能的话，在Termux执行`exit`退出Termux然后重新走一遍该小节吧。**

当然没有mplayer的话，配置`$XDG_CONFIG_HOME/go-librespot/config.yml`，将`audio_backend`设置为`pulseaudio`，然后运行程序，直接调用程序的播放接口进行测试也行。

参考资料：
```
https://github.com/termux/termux-app/issues/874
https://www.bilibili.com/opus/635423781408473091
https://www.reddit.com/r/termux/comments/162w750/please_help_me_set_up_pulseaudio/
https://www.freedesktop.org/wiki/Software/PulseAudio/Documentation/User/PerfectSetup/
https://askubuntu.com/questions/225444/how-to-make-pulseaudio-work-again
https://github.com/CypherpunkArmory/UserLAnd/issues/135
https://bugs.launchpad.net/ubuntu/+source/pulseaudio/+bug/663019
https://www.freedesktop.org/wiki/Software/PulseAudio/Documentation/User/Modules/
https://askubuntu.com/questions/201780/how-do-i-debug-issues-with-pulse-audio
https://android.stackexchange.com/questions/205576/how-to-play-sound-from-termux-when-using-linux
https://stackoverflow.com/questions/51128790/linux-pipe-audio-to-virtual-microphone-using-pactl
https://stackoverflow.com/questions/43480867/linux-pipe-audio-file-to-microphone-input
https://bkhome.org/news/202111/disable-pulseaudio-idle-exit-and-support-multiple-users.html
https://bbs.archlinux.org/viewtopic.php?id=92851
https://github.com/termux/termux-packages/issues/12289
https://github.com/meefik/linuxdeploy/issues/223#issuecomment-114639940
https://github.com/pelya/pulseaudio-android
```