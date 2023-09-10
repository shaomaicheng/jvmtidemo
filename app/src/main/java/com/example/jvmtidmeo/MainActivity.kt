package com.example.jvmtidmeo

import android.os.Bundle
import android.os.Debug
import android.os.Handler
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.example.jvmtidmeo.ui.theme.JvmtidmeoTheme
import com.example.mylibrary.JvmtiConfig

class MainActivity : ComponentActivity() {
    var list:ArrayList<StringWrapper>? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        JvmtiConfig().enableJvmti(this)
        setContent {
            JvmtidmeoTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    Greeting("Android")
                }
            }
        }

        list = arrayListOf()
        for (i in 0..100) {
            val str = StringWrapper(i.toString())
            list?.add(str)
        }
        Handler().postDelayed(Runnable {
            list = null
            System.gc()
            Log.e("chenglei", "gc!!! ")

            Thread.sleep(5000)
        }, 3000)
    }
}

class StringWrapper(s:String)

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello $name!",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    JvmtidmeoTheme {
        Greeting("Android")
    }
}