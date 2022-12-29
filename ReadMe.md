# JVMTI实现内存监控
- 初始化内存监控环境
- 开启jvmti事件监听
- 对象创建与方法调用回调处理
- 日志分析定位问题

# BUG
## Android Studio生成的apk找不到so问题
正常来说，jar包放在app/libs下，so文件放在app/src/main/jniLibs文件夹下，这样编译运行不用再加其他配置就可以用了，但实际上还是报了找不到so文件的错误

注意，这时候你要检查一下你的minSdkVersion是不是23及以上了，因为在Android API 23即 6.0增加extractNativeLibs标签，它会将应用的nativelibs文件解压到Application.nativeLibraryDir目录下，一般为/data/app/<packagename>/lib目录，而默认情况下extractNativeLibs为false，是不会解压nativelibs即so文件的。

所以你要在应用AndroidManifest文件application标签下增加android:extractNativeLibs="true"，即可解决问题。