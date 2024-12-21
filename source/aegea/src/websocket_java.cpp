#ifdef AEGEA_WEBSOCKET_JAVA
#include "websocket.h"
#include <stdio.h>
#include <stdint.h>
#include <queue>
#include <jni.h>

namespace
{

JavaVM* javaVM = nullptr;

struct JavaWebSocket : public WebSocket
{
	std::queue<Message> queue;
	Message buffer;
	std::string error;
	bool opened = false;

	jclass class_WebSocket;
	jobject client;

	~JavaWebSocket()
	{

	}

	JavaWebSocket(const JavaWebSocket&) = delete;
	JavaWebSocket(JavaWebSocket&&) = delete;
	void operator=(const JavaWebSocket&) = delete;
	void operator=(JavaWebSocket&&) = delete;

	JavaWebSocket(const char* url)
	{

	}

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

	if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		fprintf(stderr, "Failed to get JNI Env\n");
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
