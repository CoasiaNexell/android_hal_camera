LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE := camera.$(TARGET_BOOTLOADER_BOARD_NAME)
LOCAL_CFLAGS :=
ifneq ($(BOARD_CAMERA_BACK_DEVICE),)
	LOCAL_CFLAGS += -DBOARD_CAMERA_BACK_DEVICE='$(BOARD_CAMERA_BACK_DEVICE)'
endif
ifneq ($(BOARD_CAMERA_FRONT_DEVICE),)
	LOCAL_CFLAGS += -DBOARD_CAMERA_FRONT_DEVICE='$(BOARD_CAMERA_FRONT_DEVICE)'
endif
ifneq ($(BOARD_CAMERA_NUM),)
	LOCAL_CFLAGS += -DBOARD_CAMERA_NUM=$(BOARD_CAMERA_NUM)
endif
ifeq ($(BOARD_CAMERA_USE_ZOOM), true)
	LOCAL_CFLAGS += -DCAMERA_USE_ZOOM
endif
ifeq ($(BOARD_CAMERA_INTERLACED), true)
	LOCAL_CFLAGS += -DCAMERA_INTERLACED
endif
ifneq ($(BOARD_CAMERA_SKIP_FRAME),)
	LOCAL_CFLAGS += -DBOARD_NUM_OF_SKIP_FRAMES=$(BOARD_CAMERA_SKIP_FRAME)
endif

LOCAL_SRC_FILES := \
	Camera3HWInterface.cpp \
	StreamManager.cpp \
	Stream.cpp \
	Exif.cpp \
	v4l2.cpp \
	ExifProcessor.cpp
LOCAL_SHARED_LIBRARIES := \
	liblog \
	libcutils \
	libutils \
	libhardware \
	libcamera_metadata \
	libcamera_client \
	libsync \
	libnxjpeg \
	libnx_scaler
LOCAL_C_INCLUDES += \
	system/media/camera/include \
	system/media/core/include \
	system/core/libsync/include \
	system/core/include/utils \
	frameworks/native/include \
	frameworks/av/include \
	device/nexell/library/nx-scaler \
	$(LOCAL_PATH)/../gralloc \
	$(LOCAL_PATH)/../libnxjpeg \
	external/libjpeg-turbo \
	$(call include-path-for)

include $(BUILD_SHARED_LIBRARY)
