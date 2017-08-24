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
#ifndef CAMERA3_HW_INTERFACE_H
#define CAMERA3_HW_INTERFACE_H

#include "GlobalDef.h"
#include "StreamManager.h"

namespace android {

/*****************************************************************************/
/* Camera3 Hal Interface Class                                               */
/*****************************************************************************/
class Camera3HWInterface {

public:
	Camera3HWInterface(int cameraId);
	~Camera3HWInterface();

	int initialize(const camera3_callback_ops_t *callback_ops);
	int configureStreams(camera3_stream_configuration_t *stream_list);
	const camera_metadata_t *constructDefaultRequestSettings(int type);
	int processCaptureRequest(camera3_capture_request_t *request);
	int flush();

	int cameraDeviceClose();
	camera3_device_t *getCameraDevice() {
		return &mCameraDevice;
	}

private:
	int mCameraId;
	const camera3_callback_ops_t *mCallbacks;
	int mPreviewHandle;

private:
	camera3_device_t mCameraDevice;
	sp<StreamManager> mPreviewManager;
};

} // namespace android

#endif /* CAMERA3_HW_INTERFACE_H */