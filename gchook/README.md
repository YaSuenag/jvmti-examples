完全停止（Stop-The-World: STW）を伴う GC の発生をロギングします

# ビルド

## Linuxの場合

### 必要なもの

* GCC
* CMake
* JDK
    * `JAVA_HOME` 環境変数が設定済みであること

### ビルド方法

```
$ cd out/build
$ cmake ../..
$ make
```

## Windowsの場合

### 必要なもの

* Visual Studio
* JDK
    * `JAVA_HOME` 環境変数が設定済みであること

### ビルド方法

1. Visual Studio でこのディレクトリを開きます
2. [CMakeLists.txt](CMakeLists.txt) を右クリックし、ビルドします

# 実行

Linux を例に示します。Windows ではパスや DLL 名に置き換えてください。

## プロセスと同期したエージェントのロード

### ライブラリのフルパス指定

```
$ java -agentpath:/path/to/libgchook.so <メインクラス>
```

### `LD_LIBRARY_PATH` などで `libgchook.so` にライブラリパスが通っている場合

```
$ java -agentlib:gchook <メインクラス>
```

## 動的アタッチ

jcmd を使って動的に JVMTI エージェントをロードします。

```
$ jcmd <PID> JVMTI.agent_load /path/to/libgchook.so
```

# テスト

[gc-examples](gc-examples) を使うことで、このエージェントの動きを簡単に試すことができます。 `JAVA_HOME` 環境変数に Java 9 以降を指定して以下のように [run.sh](gc-examples/run.sh) を実行するとエージェントの出力するメッセージが確認できます。

```
$ export JAVA_HOME=/path/to/jdk
$ ./run.sh /path/to/libgchook.so
```

suspend オプションを与える（エージェントオプション文字列に `-agentlib:gchook=suspend` のように `suspend` を与える）と、 `GarbageCollectionStart` イベントの内部で 10 秒スリープします。GC 時間は短いのに STW（Safepoint）が 10 秒以上あることを確認できます。

```
$ ./run.sh /path/to/libgchook.so suspend
      :
[0.026s][info][safepoint] Entering safepoint region: G1CollectFull
from JVMTI: GC start
from JVMTI: Sleep 10 secs in GarbageCollectionStart()...
from JVMTI: Resume from sleep
[10.029s][info][gc       ] GC(0) Pause Full (System.gc()) 1M->0M(10M) 1.820ms
from JVMTI: GC finish
[10.029s][info][safepoint] Leaving safepoint region
[10.029s][info][safepoint] Total time for which application threads were stopped: 10.0031141 seconds, Stopping threads took: 0.0000172 seconds
      :
```

`GC(0) Pause Full` のエントリでは GC 時間が 1.820ms と報告されているにもかかわらず、 `Total time for which application threads were stopped` のエントリで報告されているアプリケーション停止時間は 10 秒になっています。
