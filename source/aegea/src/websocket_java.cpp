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
#define logf(fmt, ...) __android_log_print(ANDROID_LOG_INFO, "Aegea", fmt, __VA_ARGS__)
#else
#define log(msg) fprintf(stderr, "[Aegea] %s\n", msg)
#define logf(fmt, ...) fprintf(stderr, "[Aegea] " fmt "\n", __VA_ARGS__)
#endif

static_assert(sizeof(jbyte) == sizeof(char));
static_assert(sizeof(uintptr_t) <= sizeof(jlong));

namespace
{

JavaVM* javaVM = nullptr;

struct JavaWebSocket : public WebSocket
{
	std::queue<Message> queue;
	Message buffer;
	std::string error;
	bool opened = false;

	JNIEnv* env;
	jmethodID jm_dispose;
	jmethodID jm_recv;
	jmethodID jm_send;
	jobject client = nullptr;

	~JavaWebSocket()
	{
		if (client)
		{
			env->CallVoidMethod(client, jm_dispose);
			env->ExceptionClear();
			env->DeleteGlobalRef(client);
		}
	}

	JavaWebSocket(const char* url)
	{
		// TODO: We don't do any manual attaching to threads, so probably this
		// requires that JavaWebSocket stay on the main thread.
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

		jclass class_WebSocket = env->FindClass("com/platymuus/aegea/WebSocket");
		if (!class_WebSocket)
		{
			error = "Failed to find WebSocket class";
			return;
		}

		jmethodID constructor = env->GetMethodID(class_WebSocket, "<init>", "(JLjava/lang/String;)V");
		jm_dispose = env->GetMethodID(class_WebSocket, "dispose", "()V");
		jm_recv = env->GetMethodID(class_WebSocket, "recv", "()V");
		jm_send = env->GetMethodID(class_WebSocket, "send", "(Z[B)V");
		if (!constructor || !jm_dispose || !jm_recv || !jm_send)
		{
			error = "WebSocket class missing a method";
			return;
		}

		jlong handle = static_cast<jlong>(reinterpret_cast<uintptr_t>(this));

		jstring jniUrl = env->NewStringUTF(url);
		if (refs.exception_occurred(&error))
		{
			logf("%s", error.c_str());
			return;
		}

		jobject clientLocal = env->NewObject(class_WebSocket, constructor, handle, jniUrl);
		if (refs.exception_occurred(&error))
		{
			logf("%s", error.c_str());
			return;
		}

		client = env->NewGlobalRef(clientLocal);
		if (!client)
		{
			error = "NewGlobalRef failed";
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
		if (queue.empty() && error.empty())
		{
			LocalFrame refs;
			if (!refs.init(env))
			{
				error = "JNI stack is full";
				return nullptr;
			}

			// Re-entrantly calls onX below which fill the queue.
			env->CallVoidMethod(client, jm_recv);
			if (refs.exception_occurred(&error))
			{
				logf("%s", error.c_str());
				return nullptr;
			}
		}
		if (queue.empty() || !error.empty())
		{
			return nullptr;
		}
		buffer = std::move(queue.front());
		queue.pop();
		return &buffer;
	}

	void send_text(const char* text)
	{
		LocalFrame refs;
		if (!refs.init(env))
		{
			error = "JNI stack is full";
			return;
		}


		size_t len = strlen(text);
		jbyteArray array = env->NewByteArray(len);
		if (!array)
		{
			error = "Failed to allocate byte[]";
			return;
		}

		env->SetByteArrayRegion(array, 0, len, reinterpret_cast<const jbyte*>(text));
		if (refs.exception_occurred(&error))
		{
			logf("%s", error.c_str());
			return;
		}

		jboolean is_text = true;
		env->CallVoidMethod(client, jm_send, is_text, array);
		if (refs.exception_occurred(&error))
		{
			logf("%s", error.c_str());
			return;
		}
	}
};

void onConnect(JNIEnv* env, jclass class_, jlong handle)
{
	(void)env;
	(void)class_;

	logf("onConnect(%lx)", (long)handle);
	if (handle == 0)
	{
		return;
	}
	auto self = reinterpret_cast<JavaWebSocket*>(static_cast<uintptr_t>(handle));

	self->opened = true;
}

void onMessage(JNIEnv* env, jclass class_, jlong handle, jboolean text, jbyteArray bytes, jint offset, jint len)
{
	(void)class_;

	logf("onMessage(%lx, %s, %d)", (long)handle, text ? "true" : "false", len);
	if (handle == 0)
	{
		return;
	}
	auto self = reinterpret_cast<JavaWebSocket*>(static_cast<uintptr_t>(handle));

	LocalFrame refs;
	if (!refs.init(env))
	{
		self->error = "JNI stack is full";
		return;
	}

	WebSocket::Message& message = self->queue.emplace();
	message.type = text ? WebSocket::Text : WebSocket::Binary;
	message.data.resize(len);

	env->GetByteArrayRegion(bytes, offset, len, reinterpret_cast<jbyte*>(message.data.data()));
	if (refs.exception_occurred(&self->error))
	{
		logf("%s", self->error.c_str());
		return;
	}
}

void onError(JNIEnv* env, jclass class_, jlong handle, jstring error)
{
	(void)class_;

	logf("onError(%lx)", (long)handle);
	if (handle == 0)
	{
		return;
	}
	auto self = reinterpret_cast<JavaWebSocket*>(static_cast<uintptr_t>(handle));

	const char* error_mutf8 = env->GetStringUTFChars(error, nullptr);
	logf("  %s", error_mutf8);
	self->error = error_mutf8 ? error_mutf8 : "Error getting error message in onError";
	env->ReleaseStringUTFChars(error, error_mutf8);
}

void onClose(JNIEnv* env, jclass class_, jlong handle, jint reason)
{
	(void)env;
	(void)class_;

	logf("onClose(%lx, %d)", (long)handle, reason);
	if (handle == 0)
	{
		return;
	}
	auto self = reinterpret_cast<JavaWebSocket*>(static_cast<uintptr_t>(handle));

	self->error = "WebSocket closed (";
	self->error += std::to_string(reason);
	self->error += ")";
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
	(void)reserved;

	javaVM = vm;
	JNIEnv *env = NULL;

	if (javaVM->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		log("Failed to get JNI Env");
		return JNI_VERSION_1_4;
	}

	jclass class_WebSocket = env->FindClass("com/platymuus/aegea/WebSocket");
	if (!class_WebSocket)
	{
		log("Failed to find WebSocket class");
		return JNI_ERR;
	}

	static const JNINativeMethod methods[] =
	{
		{"onConnect", "(J)V", reinterpret_cast<void*>(onConnect)},
		{"onMessage", "(JZ[BII)V", reinterpret_cast<void*>(onMessage)},
		{"onError", "(JLjava/lang/String;)V", reinterpret_cast<void*>(onError)},
		{"onClose", "(JI)V", reinterpret_cast<void*>(onClose)},
	};
	int rc = env->RegisterNatives(class_WebSocket, methods, sizeof(methods) / sizeof(JNINativeMethod));
	if (rc != JNI_OK)
	{
		log("RegisterNatives failed");
		return rc;
	}

	return JNI_VERSION_1_4;
}

#endif
