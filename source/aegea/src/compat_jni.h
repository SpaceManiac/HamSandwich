#ifndef COMPAT_JNI_H
#define COMPAT_JNI_H
#ifdef ANDROID

#include <string>
#include <jni.h>

class LocalFrame
{
	JNIEnv* m_env;
public:
	LocalFrame(const LocalFrame&) = delete;
	LocalFrame(LocalFrame&&) = delete;
	LocalFrame& operator=(const LocalFrame&) = delete;
	LocalFrame& operator=(LocalFrame&&) = delete;

	~LocalFrame();
	LocalFrame() : m_env(nullptr) {}
	bool init(JNIEnv* env, int capacity = 16);

	// Test for an exception and return its detail if one occurs.
	// `error` may be null in which case the message is discarded.
	[[nodiscard]]
	bool exception_occurred(std::string* error) const;
};

#endif  // ANDROID
#endif  // COMPAT_JNI_H
