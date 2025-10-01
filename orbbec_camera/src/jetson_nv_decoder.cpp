/*******************************************************************************
 * Copyright (c) 2023 Orbbec 3D Technology, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/
#include "orbbec_camera/jetson_nv_decoder.h"
#include <NvJpegDecoder.h>
#include <NvV4l2Element.h>
#include <algorithm>
#include <nvbufsurface.h>
#include <nvbufsurftransform.h>
#include <NvBufSurface.h>
#include <fstream>
#include <libyuv.h>
#include <rclcpp/rclcpp.hpp>
#include "orbbec_camera/utils.h"

namespace orbbec_camera {

JetsonNvJPEGDecoder::JetsonNvJPEGDecoder(int width, int height)
    : JPEGDecoder(width, height), decoder_(nullptr), rgba_fd_(-1) {
  decoder_ = NvJPEGDecoder::createJPEGDecoder("jpegdec");
  if (!decoder_) {
    throw std::runtime_error("Failed to create JPEGDecoder");
  }

  // Pre-allocate RGBA buffer
  NvBufSurf::NvCommonAllocateParams nvbufParams;
  memset(&nvbufParams, 0, sizeof(nvbufParams));
  nvbufParams.memType = NVBUF_MEM_SURFACE_ARRAY;
  nvbufParams.width = width;
  nvbufParams.height = height;
  nvbufParams.layout = NVBUF_LAYOUT_PITCH;
  nvbufParams.colorFormat = NVBUF_COLOR_FORMAT_RGBA;

  int ret = NvBufSurf::NvAllocate(&nvbufParams, 1, &rgba_fd_);
  if (ret != 0 || rgba_fd_ == -1) {
    throw std::runtime_error("Failed to preallocate RGBA buffer");
  }

  // Setup transform params
  memset(&transform_params_, 0, sizeof(transform_params_));
  transform_params_.src_top = 0;
  transform_params_.src_left = 0;
  transform_params_.src_width = width;
  transform_params_.src_height = height;
  transform_params_.dst_top = 0;
  transform_params_.dst_left = 0;
  transform_params_.dst_width = width;
  transform_params_.dst_height = height;
  transform_params_.flag = NVBUFSURF_TRANSFORM_FILTER;
  transform_params_.flip = NvBufSurfTransform_None;
  transform_params_.filter = NvBufSurfTransformInter_Nearest;
}

JetsonNvJPEGDecoder::~JetsonNvJPEGDecoder() {
  if (rgba_fd_ != -1) {
    NvBufSurf::NvDestroy(rgba_fd_);
  }
  delete decoder_;
}

bool JetsonNvJPEGDecoder::decode(const std::shared_ptr<ob::ColorFrame> &frame, uint8_t *dest) {
  if (!isValidJPEG(frame)) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("jetson_nv_decoder"), "Invalid JPEG frame");
    return false;
  }

  auto *data = static_cast<uint8_t *>(frame->data());
  uint32_t pixfmt = 0, width = 0, height = 0;
  size_t data_size = frame->dataSize();

  // Trim trailing zeros (some frames may have padding)
  while (data_size > 4 && data[data_size - 1] == 0x00) {
    data_size--;
  }

  int fd = -1;
  decoder_->decodeToFd(fd, data, data_size, pixfmt, width, height);
  if (pixfmt != V4L2_PIX_FMT_YUV422M) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("jetson_nv_decoder"), "Unexpected pixfmt: " << pixfmt);
    if (fd != -1) close(fd);
    return false;
  }
  if (width != static_cast<uint32_t>(width_) || height != static_cast<uint32_t>(height_)) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("jetson_nv_decoder"),
                        "Unexpected width/height: " << width << "x" << height);
    if (fd != -1) close(fd);
    return false;
  }

  // Transform into pre-allocated RGBA buffer
  int ret = NvBufSurf::NvTransform(&transform_params_, fd, rgba_fd_);
  if (fd != -1) close(fd);
  if (ret != 0) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("jetson_nv_decoder"), "Failed to transform buffer");
    return false;
  }

  // Map, convert to RGB24
  NvBufSurface *nvbuf_surf = nullptr;
  NvBufSurfaceFromFd(rgba_fd_, (void **)&nvbuf_surf);
  ret = NvBufSurfaceMap(nvbuf_surf, 0, 0, NVBUF_MAP_READ_WRITE);
  if (ret < 0) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("jetson_nv_decoder"), "Failed to map buffer");
    return false;
  }
  NvBufSurfaceSyncForCpu(nvbuf_surf, 0, 0);

  uint8_t *rgba = (uint8_t *)nvbuf_surf->surfaceList[0].mappedAddr.addr[0];
  int src_stride_argb = width * 4;
  int dst_stride_rgb24 = width * 3;
  libyuv::ARGBToRGB24(rgba, src_stride_argb, dest, dst_stride_rgb24, width, height);

  NvBufSurfaceUnMap(nvbuf_surf, 0, 0);
  return true;
}

}  // namespace orbbec_camera
