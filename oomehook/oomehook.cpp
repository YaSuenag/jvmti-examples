#include <jvmti.h>

#include <iostream>


void JNICALL OnOutOfMemoryError(jvmtiEnv *jvmti, JNIEnv *env, jint flags, const void *reserved, const char *description){
  std::cerr << "from JVMTI: OutOfMemoryError occurred: " << description << std::endl;
}

static void ShowJVMTIError(jvmtiEnv *jvmti, jvmtiError value){
  jvmtiError jvmtiResult;
  char *err_string;

  jvmtiResult = jvmti->GetErrorName(value, &err_string);
  if(jvmtiResult == JVMTI_ERROR_NONE){
    std::cerr << err_string << std::endl;
    jvmti->Deallocate(reinterpret_cast<unsigned char *>(&err_string));
  }
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved){
  jint result;
  jvmtiError jvmtiResult;

  // Get JVMTI environment
  jvmtiEnv *jvmti;
  result = vm->GetEnv(reinterpret_cast<void **>(&jvmti), JVMTI_VERSION_1_1);
  if(result != JNI_OK){
    return JNI_ERR;
  }

  // Add capability for ResourceExhausted event due to Java heap space
  jvmtiCapabilities capabilities = {0};
  capabilities.can_generate_resource_exhaustion_heap_events = 1;
  jvmtiResult = jvmti->AddCapabilities(&capabilities);
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }

  // Register JVMTI event callback for ResourceExhausted
  jvmtiEventCallbacks callbacks = {0};
  callbacks.ResourceExhausted = &OnOutOfMemoryError;
  jvmtiResult = jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }

  // Enable ResourceExhausted event
  jvmtiResult = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_RESOURCE_EXHAUSTED, NULL);
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }

  return JNI_OK;
}

JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options, void *reserved){
  return Agent_OnLoad(vm, options, reserved);
}

