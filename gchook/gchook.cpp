#include <jvmti.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

static jboolean need_to_suspend = JNI_FALSE;

void JNICALL OnGarbageCollectionStart(jvmtiEnv *jvmti){
  std::cout << "from JVMTI: GC start" << std::endl;
  if(need_to_suspend){
    std::cout << "from JVMTI: Sleep 10 secs in GarbageCollectionStart()..." << std::endl;
    sleep(10);
    std::cout << "from JVMTI: Resume from sleep" << std::endl;
  }
}

void JNICALL OnGarbageCollectionFinish(jvmtiEnv *jvmti){
  std::cout << "from JVMTI: GC finish" << std::endl;
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
  if((options != NULL) && (strcmp(options, "suspend") == 0)){
    need_to_suspend = JNI_TRUE;
  }

  jint result;
  jvmtiError jvmtiResult;

  // Get JVMTI environment
  jvmtiEnv *jvmti;
  result = vm->GetEnv(reinterpret_cast<void **>(&jvmti), JVMTI_VERSION_1_1);
  if(result != JNI_OK){
    return JNI_ERR;
  }

  // Add capability for GC events
  jvmtiCapabilities capabilities = {0};
  capabilities.can_generate_garbage_collection_events = 1;
  jvmtiResult = jvmti->AddCapabilities(&capabilities);
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }

  // Register JVMTI event callback for GC events
  jvmtiEventCallbacks callbacks = {0};
  callbacks.GarbageCollectionStart = &OnGarbageCollectionStart;
  callbacks.GarbageCollectionFinish = &OnGarbageCollectionFinish;
  jvmtiResult = jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }

  // Enable GC events
  jvmtiResult = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, NULL);
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }
  jvmtiResult = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, NULL);
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }

  return JNI_OK;
}

JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options, void *reserved){
  return Agent_OnLoad(vm, options, reserved);
}

