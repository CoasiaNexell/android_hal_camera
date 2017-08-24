/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef STREAM_MANAGER_H
#define STREAM_MANAGER_H

#include <gralloc_priv.h>

#include <utils/Condition.h>
#include <utils/Errors.h>
#include <utils/List.h>
#include <utils/Mutex.h>
#include <utils/Thread.h>

#include "GlobalDef.h"
#include "NXQueue.h"

namespace android {

class NXCamera3Buffer {
public:
	NXCamera3Buffer() {
	}

	virtual ~NXCamera3Buffer() {
	}

	void init(uint32_t frameNumber,
			  camera3_stream_t *ps, buffer_handle_t *pb,
			  camera3_stream_t *rs = NULL, buffer_handle_t *rb = NULL,
			  camera3_stream_t *cs = NULL, buffer_handle_t *cb = NULL) {
		mFrameNumber = frameNumber;
		mHaveTrigger = false;
		mStreams[PREVIEW_STREAM] = ps;
		mStreams[RECORD_STREAM] = rs;
		mStreams[CAPTURE_STREAM] = cs;
		mBuffers[PREVIEW_STREAM] = pb;
		mBuffers[RECORD_STREAM] = rb;
		mBuffers[CAPTURE_STREAM] = cb;
	}

	private_handle_t *getPrivateHandle(uint32_t type) {
		if (type < MAX_STREAM)
			return (private_handle_t *)(*mBuffers[type]);
		return NULL;
	}

	int getDmaFd(uint32_t type) {
		if (type < MAX_STREAM) {
			private_handle_t *h = (private_handle_t *)(*mBuffers[type]);
			return h->share_fd;
		}
		return -1;
	}

	camera3_stream_t *getStream(uint32_t type) {
		if (type < MAX_STREAM)
			return mStreams[type];
		return NULL;
	}

	buffer_handle_t *getBuffer(uint32_t type) {
		if (type < MAX_STREAM)
			return mBuffers[type];
		return NULL;
	}

	uint32_t getFrameNumber() {
		return mFrameNumber;
	}

	bool haveTrigger() {
		return mHaveTrigger;
	}

	void setRecord(camera3_stream_t *s, buffer_handle_t *b) {
		mStreams[RECORD_STREAM] = s;
		mBuffers[RECORD_STREAM] = b;
	}

	void setCapture(camera3_stream_t *s, buffer_handle_t *b) {
		mStreams[CAPTURE_STREAM] = s;
		mBuffers[CAPTURE_STREAM] = b;
	}

	void setFrameNumber(uint32_t number) {
		mFrameNumber = number;
	}

	void setTrigger(bool trigger) {
		mHaveTrigger = trigger;
	}

private:
	camera3_stream_t *mStreams[MAX_STREAM];
	buffer_handle_t  *mBuffers[MAX_STREAM];
	bool		mHaveTrigger;
	uint32_t	mFrameNumber;
};

class StreamManager : public Thread {
public:
	StreamManager(int fd, const camera3_callback_ops_t *callbacks,
				  uint8_t intent)
		: mFd(fd),
		  mCallbacks(callbacks), mSize(0), mQIndex(0), mIntent(intent),
		  mPrevFrameNumber(0) {
		for (uint32_t i = 0; i < MAX_BUFFER_COUNT + 2; i++)
			mFQ.queue(&mBuffers[i]);
		ALOGD("[CTOR] SteamManager");
	}
	virtual ~StreamManager() {
		ALOGD("[DTOR] SteamManager");
	}

	virtual status_t readyToRun();
	int registerRequest(camera3_capture_request_t *r);
	void stopStreaming();
	void setQIndex(int index) {
		mQIndex = index % MAX_BUFFER_COUNT;
	}
	uint8_t getIntent() {
		return mIntent;
	}

protected:
	virtual bool threadLoop();

private:
	int mFd;
	const camera3_callback_ops_t *mCallbacks;
	uint32_t mSize;
	int mQIndex;
	uint8_t mIntent;
	/* buffer manage
	 * mFQ -> mQ -> MRQ -> mFQ
	 */
	NXQueue<NXCamera3Buffer *> mFQ; /* free buffer */
	NXQueue<NXCamera3Buffer *> mQ; /* q: service --> hal */
	NXQueue<NXCamera3Buffer *> mRQ; /* return q: hal --> service */
	Mutex mLock;
	NXCamera3Buffer mBuffers[MAX_BUFFER_COUNT+2];
	uint32_t mPrevFrameNumber;

private:
	int setBufferFormat(private_handle_t *h);
	int sendResult(bool drain = false);
	void drainBuffer();
	void stopV4l2();
	int copyBuffer(private_handle_t *dst, private_handle_t *src);
	int jpegEncoding(private_handle_t *dst, private_handle_t *src);
};

}; // namespace android

#endif /* STREAM_MANAGER_H */