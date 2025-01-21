#ifdef ANDROID
#include "compat_jni.h"
#include <stdio.h>
#include <jni.h>

// ----------------------------------------------------------------------------
// Based in part on SDL_android.c

LocalFrame::~LocalFrame()
{
	if (m_env)
	{
		m_env->PopLocalFrame(nullptr);
	}
}

bool LocalFrame::init(JNIEnv* env, int capacity)
{
	if (env->PushLocalFrame(capacity) < 0)
	{
		return false;
	}
	m_env = env;
	return true;
}

[[nodiscard]]
bool LocalFrame::exception_occurred(std::string* error) const
{
	jthrowable exception = m_env->ExceptionOccurred();
	if (exception)
	{
		// Until this happens most JNI operations have undefined behaviour.
		m_env->ExceptionClear();

		if (error)
		{
			jclass exceptionClass = m_env->GetObjectClass(exception);
			jclass classClass = m_env->FindClass("java/lang/Class");
			auto exceptionName = (jstring)m_env->CallObjectMethod(exceptionClass, m_env->GetMethodID(classClass, "getName", "()Ljava/lang/String;"));
			const char* exceptionNameUTF8 = m_env->GetStringUTFChars(exceptionName, nullptr);
			auto exceptionMessage = (jstring)m_env->CallObjectMethod(exception, m_env->GetMethodID(exceptionClass, "getMessage", "()Ljava/lang/String;"));

			if (exceptionMessage)
			{
				const char* exceptionMessageUTF8 = m_env->GetStringUTFChars(exceptionMessage, nullptr);
				*error = exceptionNameUTF8;
				*error += ": ";
				*error += exceptionMessageUTF8;
				m_env->ReleaseStringUTFChars(exceptionMessage, exceptionMessageUTF8);
			}
			else
			{
				*error = exceptionNameUTF8;
			}

			m_env->ReleaseStringUTFChars(exceptionName, exceptionNameUTF8);
		}

		return true;
	}

	return false;
}
#endif
