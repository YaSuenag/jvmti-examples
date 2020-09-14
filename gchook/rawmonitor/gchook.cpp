#include <jvmti.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

static jboolean need_to_suspend = JNI_FALSE;
static volatile bool is_terminated = false;
jrawMonitorID monitor;

static void JNICALL monitor_thread_entry(jvmtiEnv *jvmti, JNIEnv *env, void *arg){
  jvmti->RawMonitorEnter(monitor);

  while(true){
    jvmti->RawMonitorWait(monitor, 0L);
    if(is_terminated){
      break;
    }
    std::cout << "from JVMTI agent thread" << std::endl;
    //env->NewStringUTF("Access Java heap");
    std::cout << "from JVMTI agent thread: continue" << std::endl;
  }

  jvmti->RawMonitorExit(monitor);
  std::cout << "JVMTI agent thread finished" << std::endl;
}

void JNICALL OnVMInit(jvmtiEnv *jvmti, JNIEnv *env, jthread thread){
  jclass threadClass = env->FindClass("Ljava/lang/Thread;");
  jmethodID constructorID = env->GetMethodID(threadClass, "<init>", "(Ljava/lang/String;)V");
  jstring jThreadName = env->NewStringUTF("GarbageCollectionStart monitor thread");
  jthread agent_thread = env->NewObject(threadClass, constructorID, jThreadName);
  jvmti->RunAgentThread(agent_thread, &monitor_thread_entry, NULL, JVMTI_THREAD_NORM_PRIORITY);
}

void JNICALL OnVMDeath(jvmtiEnv *jvmti, JNIEnv *env){
  is_terminated = true;
  jvmti->RawMonitorEnter(monitor);
  jvmti->RawMonitorNotify(monitor);
  jvmti->RawMonitorExit(monitor);
}

void JNICALL OnGarbageCollectionStart(jvmtiEnv *jvmti){
  std::cout << "from JVMTI: GC start" << std::endl;

  jvmti->RawMonitorEnter(monitor);
  std::cout << "from JVMTI: notify" << std::endl;
  jvmti->RawMonitorNotify(monitor);
  jvmti->RawMonitorExit(monitor);

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

  // Create monitor for GarbageCollectionStart event
  jvmtiResult = jvmti->CreateRawMonitor("GarbageCollectionStart monitor", &monitor);
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
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

  // Register JVMTI event callbacks
  jvmtiEventCallbacks callbacks = {0};
  callbacks.VMInit = &OnVMInit;
  callbacks.VMDeath = &OnVMDeath;
  callbacks.GarbageCollectionStart = &OnGarbageCollectionStart;
  callbacks.GarbageCollectionFinish = &OnGarbageCollectionFinish;
  jvmtiResult = jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }

  // Enable JVMTI events
  jvmtiResult = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL);
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }
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
  jvmtiResult = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, NULL);
  if(jvmtiResult != JVMTI_ERROR_NONE){
    ShowJVMTIError(jvmti, jvmtiResult);
    return JNI_ERR;
  }

  return JNI_OK;
}
