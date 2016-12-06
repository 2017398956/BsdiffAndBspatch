package nfl.bspatch;

/**
 * Created by fuli.niu on 2016/12/6.
 * apk合并工具
 */

public class BspatchUtil {
    static {
        System.loadLibrary("nflbspatch-lib");
    }

    public static native String bspatch(String oldApk, String nowApk, String patch);
}
