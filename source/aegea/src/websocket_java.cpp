#ifdef AEGEA_WEBSOCKET_JAVA
#include "websocket.h"
#include <stdio.h>
#include <stdint.h>
#include <queue>
#include <jni.h>
#include "compat_jni.h"

#ifdef ANDROID
#include <android/log.h>
#define log(fmt) __android_log_print(ANDROID_LOG_INFO, "Aegea", "%s", fmt)
#define logf(fmt, ...) __android_log_print(ANDROID_LOG_INFO, "Aegea", fmt, ...)
#else
#define log(msg) fprintf(stderr, "[Aegea] %s\n", msg)
#define logf(fmt, ...) fprintf(stderr, "[Aegea] " fmt "\n", __VA_ARGS__)
#endif

namespace
{

JavaVM* javaVM = nullptr;

struct JavaWebSocket : public WebSocket
{
	std::queue<Message> queue;
	Message buffer;
	std::string error;
	bool opened = false;

	jclass class_WebSocket = nullptr;
	jobject client = nullptr;

	~JavaWebSocket()
	{
	}

	JavaWebSocket(const char* url)
	{
		// TODO: We don't do any manual attaching to threads, so probably this
		// requires that JavaWebSocket stay on the main thread.
		JNIEnv* env;
		if (javaVM->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
		{
			error = "Failed to get JNI environment";
			return;
		}

		LocalFrame refs;
		if (!refs.init(env))
		{
			error = "JNI stack is full";
			return;
		}

		class_WebSocket = env->FindClass("com/platymuus/aegea/WebSocket");
		if (!class_WebSocket)
		{
			error = "Failed to find WebSocket class";
			return;
		}

		jlong handle = static_cast<jlong>(reinterpret_cast<uintptr_t>(this));

		jstring jniUrl = env->NewStringUTF(url);
		if (refs.exception_occurred(&error))
		{
			return;
		}

		jmethodID constructor = env->GetMethodID(class_WebSocket, "<init>", "(JLjava/lang/String;)V");
		client = env->NewObject(class_WebSocket, constructor, handle, jniUrl);
		if (refs.exception_occurred(&error))
		{
			client = nullptr;
			return;
		}
	}

	JavaWebSocket(const JavaWebSocket&) = delete;
	JavaWebSocket(JavaWebSocket&&) = delete;
	void operator=(const JavaWebSocket&) = delete;
	void operator=(JavaWebSocket&&) = delete;

	bool is_connected() const
	{
		return opened;
	}

	std::string_view error_message() const
	{
		return error;
	}

	const Message* recv()
	{
		if (queue.empty())
		{
			return nullptr;
		}
		buffer = std::move(queue.front());
		queue.pop();
		return &buffer;
	}

	void send_text(const char* text)
	{
	}
};

void onConnect(JNIEnv* env, jclass _class, jlong handle)
{
	auto self = reinterpret_cast<JavaWebSocket*>(static_cast<uintptr_t>(handle));
	log("onConnect received");
}

void onMessage(JNIEnv* env, jclass _class, jlong handle, jboolean text, jbyteArray bytes, jint offset, jint len)
{
}

void onError(JNIEnv* env, jclass _class, jlong handle, jstring error)
{
}

void onClose(JNIEnv* env, jclass _class, jlong handle, jint reason)
{
}

}

bool WebSocket::is_supported()
{
	return javaVM != nullptr;
}

std::unique_ptr<WebSocket> WebSocket::connect(const char* url)
{
	return std::make_unique<JavaWebSocket>(url);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	javaVM = vm;
	JNIEnv *env = NULL;

	if (javaVM->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		log("Failed to get JNI Env");
		return JNI_VERSION_1_4;
	}

	jclass class_WebSocket = env->FindClass("com/platymuus/aegea/WebSocket");
	if (!class_WebSocket)
		return JNI_ERR;

	static const JNINativeMethod methods[] =
	{
		{"onConnect", "(J)V", reinterpret_cast<void*>(onConnect)},
		{"onMessage", "(JZ[BII)V", reinterpret_cast<void*>(onMessage)},
		{"onError", "(JLjava/lang/String;)V", reinterpret_cast<void*>(onError)},
		{"onClose", "(JI)V", reinterpret_cast<void*>(onClose)},
	};
	int rc = env->RegisterNatives(class_WebSocket, methods, sizeof(methods) / sizeof(JNINativeMethod));
	if (rc != JNI_OK)
		return rc;

	return JNI_VERSION_1_4;
}

#endif
