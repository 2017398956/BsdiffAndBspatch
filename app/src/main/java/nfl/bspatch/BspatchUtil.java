package nfl.bspatch;

/**
 * Created by fuli.niu on 2016/12/6.
 * apk合并工具
 */

public class BspatchUtil {

    protected static String TAG = "BspatchUtil" ;
    static {
//        System.loadLibrary("nflbspatch-lib");
        // 验证包冲突
//        System.loadLibrary("test");
        System.loadLibrary("nflbspatch-lib");

    }
    // 注意：java代码中声明的native方法如果被调用cpp中必须有该方法，cpp代码中声明的native方法java中也必须有该方法。
    public static native int add(int x, int y);
    public static native int substraction(int x, int y);
    public static native float multiplication(int x, int y);
    public static native float division(int x, int y);
    public static native int bspatch(String oldApk, String nowApk, String patch);
    // 不知道为什么这个方法调用后会崩溃
    public static native String sayHello(String x);
    public static native int testAdd2(int x , int y ) ;
    public static native String say(String x) ;
}
