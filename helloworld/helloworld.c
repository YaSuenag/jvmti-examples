#include <jvmti.h>
#include <stdio.h>
#include <string.h>

static int is_error(const char *options){
  int result = JNI_FALSE;
  if(options != NULL){
    printf("  options = %s\n", options);
    result = (strcmp(options, "error") == 0);
  }
  return result;
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved){
  printf("Hello World from Agent_OnLoad()\n");
  return is_error(options) ? JNI_ERR : JNI_OK;
}

JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options, void *reserved){
  printf("Hello World from Agent_OnAttach()\n");
  return is_error(options) ? JNI_ERR : JNI_OK;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm){
  printf("Goodbye World from Agent_OnUnload()\n");
}
