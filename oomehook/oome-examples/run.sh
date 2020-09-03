#!/bin/bash

AGENT=$1

if [ "x$AGENT" = "x" ]; then
  echo "usage: ./run.sh /path/to/agent"
  exit 1
fi

$JAVA_HOME/bin/javac OOME.java
$JAVA_HOME/bin/java -Xmx100m -agentpath:$AGENT OOME
