#!/bin/bash

AGENT=$1
OPTS=$2

if [ "x$AGENT" = "x" ]; then
  echo "usage: ./run.sh /path/to/agent"
  exit 1
fi

AGENT_PATH=-agentpath:$AGENT

if [ "x$OPTS" != "x" ]; then
  AGENT_PATH="$AGENT_PATH=$OPTS"
fi

$JAVA_HOME/bin/javac GC.java
$JAVA_HOME/bin/java -Xlog:gc=info,safepoint=info $AGENT_PATH GC
