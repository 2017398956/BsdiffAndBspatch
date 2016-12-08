//
// Created by fuli.niu on 2016/12/6.
//
/*-
 * Copyright 2003-2005 Colin Percival
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if 0
__FBSDID("$FreeBSD: src/usr.bin/bsdiff/bspatch/bspatch.c,v 1.1 2005/08/06 01:59:06 cperciva Exp $");
#endif

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
//#include "bzip2/bzlib.c"
#include "bzip2/bzlib.h"
//#include "bzip2/bzlib_private.h"
#include <jni.h>


static const char *classPathName = "nfl/bspatch/BspatchUtil";

static off_t offtin(u_char *buf) {
    off_t y;

    y = buf[7] & 0x7F;
    y = y * 256;
    y += buf[6];
    y = y * 256;
    y += buf[5];
    y = y * 256;
    y += buf[4];
    y = y * 256;
    y += buf[3];
    y = y * 256;
    y += buf[2];
    y = y * 256;
    y += buf[1];
    y = y * 256;
    y += buf[0];

    if (buf[7] & 0x80) y = -y;

    return y;
}

extern "C"
int patchMethodByNFL(int argc, char *argv[]) {
    FILE *f, *cpf, *dpf, *epf;
    BZFILE *cpfbz2, *dpfbz2, *epfbz2;
    int cbz2err, dbz2err, ebz2err;
    int fd;
    ssize_t oldsize, newsize;
    ssize_t bzctrllen, bzdatalen;
    u_char header[32], buf[8];
    u_char *old;
    // u_char *new;
    // modified new to new File by nfl 2016-12-08
    u_char *newFile;
    off_t oldpos, newpos;
    off_t ctrl[3];
    off_t lenread;
    off_t i;

    if (argc != 4) {
        // errx(1,"usage: %s oldfile newfile patchfile\n",argv[0]);
        return -1;
    }
    /* Open patch file */
    if ((f = fopen(argv[3], "r")) == NULL) {
        // errx(1, "fopen(%s)", argv[3]);
        return -2;
    }
    /*
    File format:
        0	8	"BSDIFF40"
        8	8	X
        16	8	Y
        24	8	sizeof(newfile)
        32	X	bzip2(control block)
        32+X	Y	bzip2(diff block)
        32+X+Y	???	bzip2(extra block)
    with control block a set of triples (x,y,z) meaning "add x bytes
    from oldfile to x bytes from the diff block; copy y bytes from the
    extra block; seek forwards in oldfile by z bytes".
    */

    /* Read header */
    if (fread(header, 1, 32, f) < 32) {
        if (feof(f)) {
            // errx(1, "Corrupt patch\n");
        }
        // err(1, "fread(%s)", argv[3]);
        return -3;
    }

    /* Check for appropriate magic */
    if (memcmp(header, "BSDIFF40", 8) != 0) {
        // errx(1, "Corrupt patch\n");
        return -4;
    }
    /* Read lengths from header */
    bzctrllen = offtin(header + 8);
    bzdatalen = offtin(header + 16);
    newsize = offtin(header + 24);
    if ((bzctrllen < 0) || (bzdatalen < 0) || (newsize < 0)) {
        // errx(1,"Corrupt patch\n");
        return -5;
    }
    /* Close patch file and re-open it via libbzip2 at the right places */
    if (fclose(f)) {
        // err(1, "fclose(%s)", argv[3]);
        return -6;
    }
    if ((cpf = fopen(argv[3], "r")) == NULL) {
        // err(1, "fopen(%s)", argv[3]);
        return -7;
    }
    if (fseeko(cpf, 32, SEEK_SET)) {
        // err(1, "fseeko(%s, %lld)", argv[3], (long long)32);
        return -8;
    }
    if ((cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0)) == NULL) {
        // errx(1, "BZ2_bzReadOpen, bz2err = %d", cbz2err);
        return -9;
    }
    if ((dpf = fopen(argv[3], "r")) == NULL) {
        // err(1, "fopen(%s)", argv[3]);
        return -10;
    }
    if (fseeko(dpf, 32 + bzctrllen, SEEK_SET)) {
        // err(1, "fseeko(%s, %lld)", argv[3], (long long)(32 + bzctrllen));
        return -11;
    }
    if ((dpfbz2 = BZ2_bzReadOpen(&dbz2err, dpf, 0, 0, NULL, 0)) == NULL) {
        // errx(1, "BZ2_bzReadOpen, bz2err = %d", dbz2err);
        return -12;
    }

    if ((epf = fopen(argv[3], "r")) == NULL) {
        // err(1, "fopen(%s)", argv[3]);
        return -13;
    }

    if (fseeko(epf, 32 + bzctrllen + bzdatalen, SEEK_SET)) {
        // err(1, "fseeko(%s, %lld)", argv[3],(long long)(32 + bzctrllen + bzdatalen));
        return -14;
    }

    if ((epfbz2 = BZ2_bzReadOpen(&ebz2err, epf, 0, 0, NULL, 0)) == NULL) {
        // errx(1, "BZ2_bzReadOpen, bz2err = %d", ebz2err);
        return -15;
    }

    if (((fd = open(argv[1], O_RDONLY, 0)) < 0) ||
        ((oldsize = lseek(fd, 0, SEEK_END)) == -1) ||
        ((old = static_cast<u_char *>(malloc(oldsize + 1))) == NULL) ||
        (lseek(fd, 0, SEEK_SET) != 0) ||
        (read(fd, old, oldsize) != oldsize) ||
        (close(fd) == -1)){
        // err(1, "%s", argv[1]);
        return -16 ;
    }

    if ((newFile = static_cast<u_char *>(malloc(newsize + 1))) == NULL){
        // err(1, NULL);
        return -17 ;
    }

    oldpos = 0;
    newpos = 0;
    while (newpos < newsize) {
        /* Read control data */
        for (i = 0; i <= 2; i++) {
            lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
            if ((lenread < 8) || ((cbz2err != BZ_OK) &&
                                  (cbz2err != BZ_STREAM_END))){
                // errx(1, "Corrupt patch\n");
                return -18 ;
            }

            ctrl[i] = offtin(buf);
        };

        /* Sanity-check */
        if (newpos + ctrl[0] > newsize){
            // errx(1, "Corrupt patch\n");
            return -19 ;
        }

        /* Read diff string */
        lenread = BZ2_bzRead(&dbz2err, dpfbz2, newFile + newpos, ctrl[0]);
        if ((lenread < ctrl[0]) ||
            ((dbz2err != BZ_OK) && (dbz2err != BZ_STREAM_END))){
            // errx(1, "Corrupt patch\n");
            return -20 ;
        }

        /* Add old data to diff string */
        for (i = 0; i < ctrl[0]; i++)
            if ((oldpos + i >= 0) && (oldpos + i < oldsize))
                newFile[newpos + i] += old[oldpos + i];

        /* Adjust pointers */
        newpos += ctrl[0];
        oldpos += ctrl[0];

        /* Sanity-check */
        if (newpos + ctrl[1] > newsize){
            // errx(1, "Corrupt patch\n");
            return -21 ;
        }

        /* Read extra string */
        lenread = BZ2_bzRead(&ebz2err, epfbz2, newFile + newpos, ctrl[1]);
        if ((lenread < ctrl[1]) ||
            ((ebz2err != BZ_OK) && (ebz2err != BZ_STREAM_END))){
            // errx(1, "Corrupt patch\n");
            return -22 ;
        }

        /* Adjust pointers */
        newpos += ctrl[1];
        oldpos += ctrl[2];
    };

    /* Clean up the bzip2 reads */
    BZ2_bzReadClose(&cbz2err, cpfbz2);
    BZ2_bzReadClose(&dbz2err, dpfbz2);
    BZ2_bzReadClose(&ebz2err, epfbz2);
    if (fclose(cpf) || fclose(dpf) || fclose(epf)){
        // err(1, "fclose(%s)", argv[3]);
        return -23 ;
    }

    /* Write the new file */
    if (((fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0) ||
        (write(fd, newFile, newsize) != newsize) || (close(fd) == -1)){
        // err(1, "%s", argv[2]);
        return -24 ;
    }

    free(newFile);
    free(old);
    return 0;
}

// 下面是我的代码
jint bspatch(JNIEnv *env, jclass cls, jstring old, jstring now, jstring patch) {
    int argc = 4;
    char *argv[argc];
    argv[0] = "bspatch";
    argv[1] = (char *) env->GetStringUTFChars(old, 0);
    argv[2] = (char *) env->GetStringUTFChars(now, 0);
    argv[3] = (char *) env->GetStringUTFChars(patch, 0);
    int ret = patchMethodByNFL(argc, argv);
    env->ReleaseStringUTFChars(old, argv[1]);
    env->ReleaseStringUTFChars(now, argv[2]);
    env->ReleaseStringUTFChars(patch, argv[3]);
    return ret;
}

// 不知道为什么这个方法调用后会崩溃
jstring Java_nfl_bspatch_BspatchUtil_sayHello(JNIEnv *env, jclass cls, jstring str) {
    if (NULL != str) {
        return str;
    }
    return env->NewStringUTF("Hello from sayHello");
}

// 以下为动态注册JNI方法
jint add(JNIEnv *env, jobject thiz, jint x, jint y) {
    return x + y;
}

jint substraction(JNIEnv *env, jobject thiz, jint x, jint y) {
    return x - y;
}

jfloat multiplication(JNIEnv *env, jobject thiz, jint x, jint y) {
    return (float) x * (float) y;
}

jfloat division(JNIEnv *env, jobject thiz, jint x, jint y) {
    return (float) x / (float) y;
}

jstring say(JNIEnv *env, jclass thiz, jstring str) {
    if (NULL != str) {
        return str;
    }
    return env->NewStringUTF("say");
}

jint testAdd2(JNIEnv *env, jobject thiz, jint x, jint y) {
    return x + y;
}

static JNINativeMethod methods[] = {
        {"add",            "(II)I",                                                     (void *) add},
        {"substraction",   "(II)I",                                                     (void *) substraction},
        {"multiplication", "(II)F",                                                     (void *) multiplication},
        {"division",       "(II)F",                                                     (void *) division},
        {"testAdd2",       "(II)I",                                                     (void *) testAdd2},
        {"say",            "(Ljava/lang/String;)Ljava/lang/String;",                    (void *) say},
        {"bspatch",        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", (void *) bspatch},
};

// 存储JNI环境
typedef union {
    JNIEnv *env;
    void *venv;
} UnionJNIEnvToVoid;

// 静态注册JNI方法
static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL)
        return JNI_FALSE;
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
        return JNI_FALSE;
    return JNI_TRUE;
}

static int registerNatives(JNIEnv *env) {
    if (!registerNativeMethods(env, classPathName,
                               methods, sizeof(methods) / sizeof(methods[0]))) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

// 当VM执行到System.loadLibrary()时，首先执行C组件里的JNI_OnLoad()。它的用途有二：
// 1. 告诉VM此C组件使用那一个JNI版本。如果你的*.so文件没有提供JNI_OnLoad()，VM会默认该*.so檔是使用最老的JNI 1.1版本。由于新版的JNI做了许多扩充，如果需要使用JNI的新版功能，例如JNI 1.4的java.nio.ByteBuffer, 就必须藉由JNI_OnLoad()来告知VM。
// 2. 由于VM执行到System.loadLibrary()时，就会立即先呼叫JNI_OnLoad()，所以C组件的开发者可以藉由JNI_OnLoad()来进行C组件内的初期值之设定(Initialization)。
// 初始化并返回jni版本，无法获取jni环境时返回-1
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv *env = NULL;
    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_6) != JNI_OK) {
        goto bail;// 无法获得jni环境，返回-1
    }
    env = uenv.env;
    if (registerNatives(env) != JNI_TRUE) {
        goto bail;
    }
    result = JNI_VERSION_1_6;
    bail:
    return result;
}

// onUnLoad方法，在JNI组件被释放时调用
void JNI_OnUnload(JavaVM *vm, void *reserved) {
}
