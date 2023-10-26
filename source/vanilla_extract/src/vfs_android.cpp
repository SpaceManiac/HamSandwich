#if defined(__ANDROID__) && __ANDROID__
#include "vfs_android.h"
#include <string.h>
#include <jni.h>
#include <SDL_system.h>
#include <SDL_assert.h>
#include <SDL_log.h>

// Android VFS implementation
class AndroidBundleVfs : public vanilla::Vfs
{
	std::string prefix;
public:
	explicit AndroidBundleVfs(const char* prefix) : prefix(prefix) {}
	owned::SDL_RWops open_sdl(const char* filename) override;
	bool list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>& output) override;
};

std::unique_ptr<vanilla::Vfs> vanilla::open_android(const char* prefix)
{
	return std::make_unique<AndroidBundleVfs>(prefix ? prefix : "");
}

owned::SDL_RWops AndroidBundleVfs::open_sdl(const char* filename)
{
	if (prefix.empty())
	{
		return owned::SDL_RWFromFile(filename, "rb");
	}
	else
	{
		std::string full = prefix;
		full.append("/");
		full.append(filename);
		return owned::SDL_RWFromFile(full.c_str(), "rb");
	}
}

// ----------------------------------------------------------------------------
// Cargo-cult copy-paste from SDL_android.c

static int s_active = 0;
struct LocalReferenceHolder
{
    JNIEnv *m_env;
    const char *m_func;
};

static struct LocalReferenceHolder LocalReferenceHolder_Setup(const char *func)
{
    struct LocalReferenceHolder refholder;
    refholder.m_env = NULL;
    refholder.m_func = func;
#ifdef DEBUG_JNI
    SDL_Log("Entering function %s", func);
#endif
    return refholder;
}

static SDL_bool LocalReferenceHolder_Init(struct LocalReferenceHolder *refholder, JNIEnv *env)
{
    const int capacity = 16;
    if (env->PushLocalFrame(capacity) < 0)
	{
        SDL_SetError("Failed to allocate enough JVM local references");
        return SDL_FALSE;
    }
    ++s_active;
    refholder->m_env = env;
    return SDL_TRUE;
}

static void LocalReferenceHolder_Cleanup(struct LocalReferenceHolder *refholder)
{
#ifdef DEBUG_JNI
    SDL_Log("Leaving function %s", refholder->m_func);
#endif
    if (refholder->m_env)
	{
        JNIEnv* env = refholder->m_env;
        env->PopLocalFrame(NULL);
        --s_active;
    }
}

static bool LocalReferenceHolder_IsActive(void)
{
    return s_active > 0;
}

/* Test for an exception and call SDL_SetError with its detail if one occurs */
/* If the parameter silent is truthy then SDL_SetError() will not be called. */
static SDL_bool Android_JNI_ExceptionOccurred(JNIEnv *mEnv, SDL_bool silent)
{
    jthrowable exception;

    SDL_assert(LocalReferenceHolder_IsActive());

    exception = mEnv->ExceptionOccurred();
    if (exception != NULL) {
        jmethodID mid;

        /* Until this happens most JNI operations have undefined behaviour */
        mEnv->ExceptionClear();

        if (!silent) {
            jclass exceptionClass = mEnv->GetObjectClass(exception);
            jclass classClass = mEnv->FindClass("java/lang/Class");
            jstring exceptionName;
            const char* exceptionNameUTF8;
            jstring exceptionMessage;

            mid = mEnv->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
            exceptionName = (jstring)mEnv->CallObjectMethod(exceptionClass, mid);
            exceptionNameUTF8 = mEnv->GetStringUTFChars(exceptionName, 0);

            mid = mEnv->GetMethodID(exceptionClass, "getMessage", "()Ljava/lang/String;");
            exceptionMessage = (jstring)mEnv->CallObjectMethod(exception, mid);

            if (exceptionMessage != NULL) {
                const char* exceptionMessageUTF8 = mEnv->GetStringUTFChars(exceptionMessage, 0);
                SDL_SetError("%s: %s", exceptionNameUTF8, exceptionMessageUTF8);
                mEnv->ReleaseStringUTFChars(exceptionMessage, exceptionMessageUTF8);
            } else {
                SDL_SetError("%s", exceptionNameUTF8);
            }

            mEnv->ReleaseStringUTFChars(exceptionName, exceptionNameUTF8);
        }

        return SDL_TRUE;
    }

    return SDL_FALSE;
}

// ----------------------------------------------------------------------------
// Implementation which calls AssetManager.list() to do some real work

bool AndroidBundleVfs::list_dir(const char* directory, std::set<std::string, vanilla::CaseInsensitive>& output)
{
    struct LocalReferenceHolder refs = LocalReferenceHolder_Setup(__FUNCTION__);

    bool result = true;

    jmethodID mid;
    jobject context;
    jobject assetManager;
    jstring fileNameJString, str;
	jobjectArray fileList;
	jobject activity;
	jclass mActivityClass;
	jsize len, i;
	const char* buffer;

    JNIEnv *mEnv = (JNIEnv*) SDL_AndroidGetJNIEnv();
    if (!LocalReferenceHolder_Init(&refs, mEnv))
	{
        goto failure;
    }

	if (prefix.empty())
	{
		fileNameJString = mEnv->NewStringUTF(directory);
	}
	else
	{
		std::string full = prefix;
		full.append("/");
		full.append(directory);
		fileNameJString = mEnv->NewStringUTF(full.c_str());
	}

	activity = (jobject) SDL_AndroidGetActivity();
	mActivityClass = mEnv->GetObjectClass(activity);

    /* context = SDLActivity.getContext(); */
    mid = mEnv->GetStaticMethodID(mActivityClass, "getContext","()Landroid/content/Context;");
    context = mEnv->CallStaticObjectMethod(mActivityClass, mid);

    /* assetManager = context.getAssets(); */
    mid = mEnv->GetMethodID(mEnv->GetObjectClass(context), "getAssets", "()Landroid/content/res/AssetManager;");
    assetManager = mEnv->CallObjectMethod(context, mid);

	/* fileList = assetManager.list(path); */
    mid = mEnv->GetMethodID(mEnv->GetObjectClass(assetManager), "list", "(Ljava/lang/String;)[Ljava/lang/String;");
	fileList = (jobjectArray) mEnv->CallObjectMethod(assetManager, mid, fileNameJString);
    if (Android_JNI_ExceptionOccurred(mEnv, SDL_FALSE))
	{
        goto failure;
    }

	len = mEnv->GetArrayLength(fileList);
	for (i = 0; i < len; ++i)
	{
		str = (jstring) mEnv->GetObjectArrayElement(fileList, i);
		buffer = mEnv->GetStringUTFChars(str, nullptr);
		output.insert(buffer);
		mEnv->ReleaseStringUTFChars(str, buffer);
	}

    if (0)
	{
failure:
        result = false;
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "AndroidBundleVfs::list_dir(%s): %s", directory, SDL_GetError());
    }

    LocalReferenceHolder_Cleanup(&refs);
    return result;
}

#endif  // __ANDROID__
