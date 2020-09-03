Java ヒープメモリ不足で発生する OutOfMemoryError を検知し、コンソールにその発生を報告します。

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
$ java -agentpath:/path/to/liboomehook.so --version
```

### `LD_LIBRARY_PATH` などで `liboomehook.so` にライブラリパスが通っている場合

```
$ java -agentlib:oomehook --version
```

## 動的アタッチ

jcmd を使って動的に JVMTI エージェントをロードします。

```
$ jcmd <PID> JVMTI.agent_load /path/to/liboomehook.so
```

# テスト

[oome-examples](oome-examples) を使うことで、このエージェントの動きを簡単に試すことができます。

```
$ export JAVA_HOME=/path/to/jdk
$ ./run.sh /path/to/liboomehook.so
```
