package com.example.mylibrary

import android.content.Context
import android.os.Build
import android.os.Debug
import android.util.Log
import androidx.annotation.RequiresApi
import java.io.File
import java.io.FileOutputStream
import java.nio.file.Files
import java.nio.file.Paths
import java.time.ZoneId
import java.util.zip.ZipFile

/**
 * @author chenglei01
 * @date 2023/9/6
 * @time 21:47
 */
class JvmtiConfig {
    init {

    }

    fun enableJvmti(context: Context){
        System.loadLibrary("jvmtidemo")
        demo()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            try {
                enableInner(context)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun enableInner(context: Context){
        val path = File(context.cacheDir, JVMTI_LIB_NAME)
        val copiedApkDir = File(context.cacheDir, "mineapk")
        System.loadLibrary("jvmtidemo")
        if (path.exists()) {
            path.delete()
        }
        if (!path.exists()) {
            val findLibrary =
                ClassLoader::class.java.getDeclaredMethod("findLibrary", String::class.java)
            val jvmtiSoLibPath = findLibrary.invoke(context.classLoader, "jvmtidemo") as? String?:return
            val zipSplite = jvmtiSoLibPath.split("!")
            if (zipSplite.size != 2)  return
            val apkFile = zipSplite[0].replace("!", "") // apk路径
            val soPath = zipSplite[1]
            Log.e("chenglei", "apkFile:$apkFile, soPath:$soPath")
            Log.e("chenglei", "libjvmtidemo.so's path is $jvmtiSoLibPath, copied path:${path}")

            copiedApkDir.mkdirs()

            val copiedApkPath = File(copiedApkDir, "mine.apk")
            if (copiedApkPath.exists()) {
                copiedApkPath.delete()
            }
            Files.copy(Paths.get(apkFile), Paths.get(copiedApkPath.absolutePath))

            Log.e("chenglei", "copiedApkPath:$copiedApkPath")

            val packageManager = context.packageManager
            val packageInfo = packageManager.getPackageArchiveInfo(copiedApkPath.absolutePath, 0)
            val applicationInfo = packageInfo?.applicationInfo
            applicationInfo?.sourceDir = copiedApkPath.absolutePath

            applicationInfo?:return
            ZipFile(applicationInfo.sourceDir)
                .use {zip->
                    val entry = zip.getEntry(soPath.replaceFirst(File.separator,""))
                    val inputStream = zip.getInputStream(entry)
                    FileOutputStream(path)
                        .use { output->
                            inputStream.copyTo(output)
                        }
                    zip.close()
                }
            Log.e("chenglei", "so copy成功:${path.absolutePath}")
        } else {
            Log.e("chenglei", "${path}已经存在，跳过")
        }


        attachJvmtiAgent(path, context)

    }

    private fun attachJvmtiAgent(path:File, context: Context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            Debug.attachJvmtiAgent(path.absolutePath, null, context.classLoader)
        } else {
            val debugClazz = Class.forName("dalvik.system.VMDebug")
            val attachAgentMethod = debugClazz.getMethod("attachAgent",String::class.java)
            attachAgentMethod.isAccessible = true
            attachAgentMethod.invoke(null, path)
        }
    }

    companion object {
        @JvmStatic
        external fun demo()


        private const val JVMTI_LIB_NAME = "libjvmtidemo.so"
    }
}