JVMTI エージェントのロード／アンロードでメッセージを表示するだけのサンプルです。

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
$ java -agentpath:/path/to/libhelloworld.so --version
```

### `LD_LIBRARY_PATH` で `libhelloworld.so` が通っている場合

```
$ java -agentpath:/path/to/libhelloworld.so --version
```

## 動的アタッチ

jcmd を使って動的に JVMTI エージェントをロードします。

```
$ jcmd <PID> JVMTI.agent_load /path/to/libhelloworld.so
```

## エージェント初期化エラーを発生させる

JVMTI は `Agent_OnLoad()` や `Agent_OnAttach()` でゼロ以外の値が返されるとエラーとみなし、エラーになります。本サンプルでは以下のように、エージェントのオプション文字列に `error` を与えると意図的にエラーを起こすことができます。

```
$ java -agentpath:/path/to/libhelloworld.so=error --version
Hello World from Agent_OnLoad()
  options = error
Error occurred during initialization of VM
agent library failed to init: /path/to/libhelloworld.so
``
