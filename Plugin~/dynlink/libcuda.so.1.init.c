/*
 * Copyright 2018-2020 Yury Gribov
 *
 * The MIT License (MIT)
 *
 * Use of this source code is governed by MIT license that can be
 * found in the LICENSE.txt file.
 */

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// Sanity check for ARM to avoid puzzling runtime crashes
#ifdef __arm__
# if defined __thumb__ && ! defined __THUMB_INTERWORK__
#   error "ARM trampolines need -mthumb-interwork to work in Thumb mode"
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CHECK(cond, fmt, ...) do { \
    if(!(cond)) { \
      fprintf(stderr, "implib-gen: libcuda.so.1: " fmt "\n", ##__VA_ARGS__); \
      assert(0 && "Assertion in generated code"); \
      exit(1); \
    } \
  } while(0)

#define CALL_USER_CALLBACK 0
#define NO_DLOPEN False
#define LAZY_LOAD 1

static void *lib_handle;
static int is_lib_loading;

static void *load_library() {
  if(lib_handle)
    return lib_handle;

  is_lib_loading = 1;

  // TODO: dlopen and users callback must be protected w/ critical section (to avoid dlopening lib twice)
#if NO_DLOPEN
  CHECK(0, "internal error"); // We shouldn't get here
#elif CALL_USER_CALLBACK
  extern void *(const char *lib_name);
  lib_handle = ("libcuda.so.1");
  CHECK(lib_handle, "callback '' failed to load library");
#else
  lib_handle = dlopen("libcuda.so.1", RTLD_LAZY | RTLD_GLOBAL);
  CHECK(lib_handle, "failed to load library: %s", dlerror());
#endif

  is_lib_loading = 0;

  return lib_handle;
}

#if ! NO_DLOPEN && ! LAZY_LOAD
static void __attribute__((constructor)) load_lib() {
  load_library();
}
#endif

static void __attribute__((destructor)) unload_lib() {
  if(lib_handle)
    dlclose(lib_handle);
}

// TODO: convert to single 0-separated string
static const char *const sym_names[] = {
  "cuArray3DCreate",
  "cuArray3DCreate_v2",
  "cuArray3DGetDescriptor",
  "cuArray3DGetDescriptor_v2",
  "cuArrayCreate",
  "cuArrayCreate_v2",
  "cuArrayDestroy",
  "cuArrayGetDescriptor",
  "cuArrayGetDescriptor_v2",
  "cuCtxAttach",
  "cuCtxCreate",
  "cuCtxCreate_v2",
  "cuCtxDestroy",
  "cuCtxDestroy_v2",
  "cuCtxDetach",
  "cuCtxDisablePeerAccess",
  "cuCtxEnablePeerAccess",
  "cuCtxGetApiVersion",
  "cuCtxGetCacheConfig",
  "cuCtxGetCurrent",
  "cuCtxGetDevice",
  "cuCtxGetFlags",
  "cuCtxGetLimit",
  "cuCtxGetSharedMemConfig",
  "cuCtxGetStreamPriorityRange",
  "cuCtxPopCurrent",
  "cuCtxPopCurrent_v2",
  "cuCtxPushCurrent",
  "cuCtxPushCurrent_v2",
  "cuCtxResetPersistingL2Cache",
  "cuCtxSetCacheConfig",
  "cuCtxSetCurrent",
  "cuCtxSetLimit",
  "cuCtxSetSharedMemConfig",
  "cuCtxSynchronize",
  "cuDestroyExternalMemory",
  "cuDestroyExternalSemaphore",
  "cuDeviceCanAccessPeer",
  "cuDeviceComputeCapability",
  "cuDeviceGet",
  "cuDeviceGetAttribute",
  "cuDeviceGetByPCIBusId",
  "cuDeviceGetCount",
  "cuDeviceGetName",
  "cuDeviceGetNvSciSyncAttributes",
  "cuDeviceGetP2PAttribute",
  "cuDeviceGetPCIBusId",
  "cuDeviceGetProperties",
  "cuDeviceGetUuid",
  "cuDevicePrimaryCtxGetState",
  "cuDevicePrimaryCtxRelease",
  "cuDevicePrimaryCtxRelease_v2",
  "cuDevicePrimaryCtxReset",
  "cuDevicePrimaryCtxReset_v2",
  "cuDevicePrimaryCtxRetain",
  "cuDevicePrimaryCtxSetFlags",
  "cuDevicePrimaryCtxSetFlags_v2",
  "cuDeviceTotalMem",
  "cuDeviceTotalMem_v2",
  "cuDriverGetVersion",
  "cuEGLApiInit",
  "cuEGLStreamConsumerAcquireFrame",
  "cuEGLStreamConsumerConnect",
  "cuEGLStreamConsumerConnectWithFlags",
  "cuEGLStreamConsumerDisconnect",
  "cuEGLStreamConsumerReleaseFrame",
  "cuEGLStreamProducerConnect",
  "cuEGLStreamProducerDisconnect",
  "cuEGLStreamProducerPresentFrame",
  "cuEGLStreamProducerReturnFrame",
  "cuEventCreate",
  "cuEventDestroy",
  "cuEventDestroy_v2",
  "cuEventElapsedTime",
  "cuEventQuery",
  "cuEventRecord",
  "cuEventRecord_ptsz",
  "cuEventSynchronize",
  "cuExternalMemoryGetMappedBuffer",
  "cuExternalMemoryGetMappedMipmappedArray",
  "cuFuncGetAttribute",
  "cuFuncGetModule",
  "cuFuncSetAttribute",
  "cuFuncSetBlockShape",
  "cuFuncSetCacheConfig",
  "cuFuncSetSharedMemConfig",
  "cuFuncSetSharedSize",
  "cuGLCtxCreate",
  "cuGLCtxCreate_v2",
  "cuGLGetDevices",
  "cuGLGetDevices_v2",
  "cuGLInit",
  "cuGLMapBufferObject",
  "cuGLMapBufferObjectAsync",
  "cuGLMapBufferObjectAsync_v2",
  "cuGLMapBufferObjectAsync_v2_ptsz",
  "cuGLMapBufferObject_v2",
  "cuGLMapBufferObject_v2_ptds",
  "cuGLRegisterBufferObject",
  "cuGLSetBufferObjectMapFlags",
  "cuGLUnmapBufferObject",
  "cuGLUnmapBufferObjectAsync",
  "cuGLUnregisterBufferObject",
  "cuGetErrorName",
  "cuGetErrorString",
  "cuGetExportTable",
  "cuGraphAddChildGraphNode",
  "cuGraphAddDependencies",
  "cuGraphAddEmptyNode",
  "cuGraphAddHostNode",
  "cuGraphAddKernelNode",
  "cuGraphAddMemcpyNode",
  "cuGraphAddMemsetNode",
  "cuGraphChildGraphNodeGetGraph",
  "cuGraphClone",
  "cuGraphCreate",
  "cuGraphDestroy",
  "cuGraphDestroyNode",
  "cuGraphExecDestroy",
  "cuGraphExecHostNodeSetParams",
  "cuGraphExecKernelNodeSetParams",
  "cuGraphExecMemcpyNodeSetParams",
  "cuGraphExecMemsetNodeSetParams",
  "cuGraphExecUpdate",
  "cuGraphGetEdges",
  "cuGraphGetNodes",
  "cuGraphGetRootNodes",
  "cuGraphHostNodeGetParams",
  "cuGraphHostNodeSetParams",
  "cuGraphInstantiate",
  "cuGraphInstantiate_v2",
  "cuGraphKernelNodeCopyAttributes",
  "cuGraphKernelNodeGetAttribute",
  "cuGraphKernelNodeGetParams",
  "cuGraphKernelNodeSetAttribute",
  "cuGraphKernelNodeSetParams",
  "cuGraphLaunch",
  "cuGraphLaunch_ptsz",
  "cuGraphMemcpyNodeGetParams",
  "cuGraphMemcpyNodeSetParams",
  "cuGraphMemsetNodeGetParams",
  "cuGraphMemsetNodeSetParams",
  "cuGraphNodeFindInClone",
  "cuGraphNodeGetDependencies",
  "cuGraphNodeGetDependentNodes",
  "cuGraphNodeGetType",
  "cuGraphRemoveDependencies",
  "cuGraphicsEGLRegisterImage",
  "cuGraphicsGLRegisterBuffer",
  "cuGraphicsGLRegisterImage",
  "cuGraphicsMapResources",
  "cuGraphicsMapResources_ptsz",
  "cuGraphicsResourceGetMappedEglFrame",
  "cuGraphicsResourceGetMappedMipmappedArray",
  "cuGraphicsResourceGetMappedPointer",
  "cuGraphicsResourceGetMappedPointer_v2",
  "cuGraphicsResourceSetMapFlags",
  "cuGraphicsResourceSetMapFlags_v2",
  "cuGraphicsSubResourceGetMappedArray",
  "cuGraphicsUnmapResources",
  "cuGraphicsUnmapResources_ptsz",
  "cuGraphicsUnregisterResource",
  "cuGraphicsVDPAURegisterOutputSurface",
  "cuGraphicsVDPAURegisterVideoSurface",
  "cuImportExternalMemory",
  "cuImportExternalSemaphore",
  "cuInit",
  "cuIpcCloseMemHandle",
  "cuIpcGetEventHandle",
  "cuIpcGetMemHandle",
  "cuIpcOpenEventHandle",
  "cuIpcOpenMemHandle",
  "cuIpcOpenMemHandle_v2",
  "cuLaunch",
  "cuLaunchCooperativeKernel",
  "cuLaunchCooperativeKernelMultiDevice",
  "cuLaunchCooperativeKernel_ptsz",
  "cuLaunchGrid",
  "cuLaunchGridAsync",
  "cuLaunchHostFunc",
  "cuLaunchHostFunc_ptsz",
  "cuLaunchKernel",
  "cuLaunchKernel_ptsz",
  "cuLinkAddData",
  "cuLinkAddData_v2",
  "cuLinkAddFile",
  "cuLinkAddFile_v2",
  "cuLinkComplete",
  "cuLinkCreate",
  "cuLinkCreate_v2",
  "cuLinkDestroy",
  "cuMemAddressFree",
  "cuMemAddressReserve",
  "cuMemAdvise",
  "cuMemAlloc",
  "cuMemAllocHost",
  "cuMemAllocHost_v2",
  "cuMemAllocManaged",
  "cuMemAllocPitch",
  "cuMemAllocPitch_v2",
  "cuMemAlloc_v2",
  "cuMemCreate",
  "cuMemExportToShareableHandle",
  "cuMemFree",
  "cuMemFreeHost",
  "cuMemFree_v2",
  "cuMemGetAccess",
  "cuMemGetAddressRange",
  "cuMemGetAddressRange_v2",
  "cuMemGetAllocationGranularity",
  "cuMemGetAllocationPropertiesFromHandle",
  "cuMemGetAttribute",
  "cuMemGetAttribute_v2",
  "cuMemGetInfo",
  "cuMemGetInfo_v2",
  "cuMemHostAlloc",
  "cuMemHostGetDevicePointer",
  "cuMemHostGetDevicePointer_v2",
  "cuMemHostGetFlags",
  "cuMemHostRegister",
  "cuMemHostRegister_v2",
  "cuMemHostUnregister",
  "cuMemImportFromShareableHandle",
  "cuMemMap",
  "cuMemPrefetchAsync",
  "cuMemPrefetchAsync_ptsz",
  "cuMemRangeGetAttribute",
  "cuMemRangeGetAttributes",
  "cuMemRelease",
  "cuMemRetainAllocationHandle",
  "cuMemSetAccess",
  "cuMemUnmap",
  "cuMemcpy",
  "cuMemcpy2D",
  "cuMemcpy2DAsync",
  "cuMemcpy2DAsync_v2",
  "cuMemcpy2DAsync_v2_ptsz",
  "cuMemcpy2DUnaligned",
  "cuMemcpy2DUnaligned_v2",
  "cuMemcpy2DUnaligned_v2_ptds",
  "cuMemcpy2D_v2",
  "cuMemcpy2D_v2_ptds",
  "cuMemcpy3D",
  "cuMemcpy3DAsync",
  "cuMemcpy3DAsync_v2",
  "cuMemcpy3DAsync_v2_ptsz",
  "cuMemcpy3DPeer",
  "cuMemcpy3DPeerAsync",
  "cuMemcpy3DPeerAsync_ptsz",
  "cuMemcpy3DPeer_ptds",
  "cuMemcpy3D_v2",
  "cuMemcpy3D_v2_ptds",
  "cuMemcpyAsync",
  "cuMemcpyAsync_ptsz",
  "cuMemcpyAtoA",
  "cuMemcpyAtoA_v2",
  "cuMemcpyAtoA_v2_ptds",
  "cuMemcpyAtoD",
  "cuMemcpyAtoD_v2",
  "cuMemcpyAtoD_v2_ptds",
  "cuMemcpyAtoH",
  "cuMemcpyAtoHAsync",
  "cuMemcpyAtoHAsync_v2",
  "cuMemcpyAtoHAsync_v2_ptsz",
  "cuMemcpyAtoH_v2",
  "cuMemcpyAtoH_v2_ptds",
  "cuMemcpyDtoA",
  "cuMemcpyDtoA_v2",
  "cuMemcpyDtoA_v2_ptds",
  "cuMemcpyDtoD",
  "cuMemcpyDtoDAsync",
  "cuMemcpyDtoDAsync_v2",
  "cuMemcpyDtoDAsync_v2_ptsz",
  "cuMemcpyDtoD_v2",
  "cuMemcpyDtoD_v2_ptds",
  "cuMemcpyDtoH",
  "cuMemcpyDtoHAsync",
  "cuMemcpyDtoHAsync_v2",
  "cuMemcpyDtoHAsync_v2_ptsz",
  "cuMemcpyDtoH_v2",
  "cuMemcpyDtoH_v2_ptds",
  "cuMemcpyHtoA",
  "cuMemcpyHtoAAsync",
  "cuMemcpyHtoAAsync_v2",
  "cuMemcpyHtoAAsync_v2_ptsz",
  "cuMemcpyHtoA_v2",
  "cuMemcpyHtoA_v2_ptds",
  "cuMemcpyHtoD",
  "cuMemcpyHtoDAsync",
  "cuMemcpyHtoDAsync_v2",
  "cuMemcpyHtoDAsync_v2_ptsz",
  "cuMemcpyHtoD_v2",
  "cuMemcpyHtoD_v2_ptds",
  "cuMemcpyPeer",
  "cuMemcpyPeerAsync",
  "cuMemcpyPeerAsync_ptsz",
  "cuMemcpyPeer_ptds",
  "cuMemcpy_ptds",
  "cuMemsetD16",
  "cuMemsetD16Async",
  "cuMemsetD16Async_ptsz",
  "cuMemsetD16_v2",
  "cuMemsetD16_v2_ptds",
  "cuMemsetD2D16",
  "cuMemsetD2D16Async",
  "cuMemsetD2D16Async_ptsz",
  "cuMemsetD2D16_v2",
  "cuMemsetD2D16_v2_ptds",
  "cuMemsetD2D32",
  "cuMemsetD2D32Async",
  "cuMemsetD2D32Async_ptsz",
  "cuMemsetD2D32_v2",
  "cuMemsetD2D32_v2_ptds",
  "cuMemsetD2D8",
  "cuMemsetD2D8Async",
  "cuMemsetD2D8Async_ptsz",
  "cuMemsetD2D8_v2",
  "cuMemsetD2D8_v2_ptds",
  "cuMemsetD32",
  "cuMemsetD32Async",
  "cuMemsetD32Async_ptsz",
  "cuMemsetD32_v2",
  "cuMemsetD32_v2_ptds",
  "cuMemsetD8",
  "cuMemsetD8Async",
  "cuMemsetD8Async_ptsz",
  "cuMemsetD8_v2",
  "cuMemsetD8_v2_ptds",
  "cuMipmappedArrayCreate",
  "cuMipmappedArrayDestroy",
  "cuMipmappedArrayGetLevel",
  "cuModuleGetFunction",
  "cuModuleGetGlobal",
  "cuModuleGetGlobal_v2",
  "cuModuleGetSurfRef",
  "cuModuleGetTexRef",
  "cuModuleLoad",
  "cuModuleLoadData",
  "cuModuleLoadDataEx",
  "cuModuleLoadFatBinary",
  "cuModuleUnload",
  "cuOccupancyAvailableDynamicSMemPerBlock",
  "cuOccupancyMaxActiveBlocksPerMultiprocessor",
  "cuOccupancyMaxActiveBlocksPerMultiprocessorWithFlags",
  "cuOccupancyMaxPotentialBlockSize",
  "cuOccupancyMaxPotentialBlockSizeWithFlags",
  "cuParamSetSize",
  "cuParamSetTexRef",
  "cuParamSetf",
  "cuParamSeti",
  "cuParamSetv",
  "cuPointerGetAttribute",
  "cuPointerGetAttributes",
  "cuPointerSetAttribute",
  "cuProfilerInitialize",
  "cuProfilerStart",
  "cuProfilerStop",
  "cuSignalExternalSemaphoresAsync",
  "cuSignalExternalSemaphoresAsync_ptsz",
  "cuStreamAddCallback",
  "cuStreamAddCallback_ptsz",
  "cuStreamAttachMemAsync",
  "cuStreamAttachMemAsync_ptsz",
  "cuStreamBatchMemOp",
  "cuStreamBatchMemOp_ptsz",
  "cuStreamBeginCapture",
  "cuStreamBeginCapture_ptsz",
  "cuStreamBeginCapture_v2",
  "cuStreamBeginCapture_v2_ptsz",
  "cuStreamCopyAttributes",
  "cuStreamCopyAttributes_ptsz",
  "cuStreamCreate",
  "cuStreamCreateWithPriority",
  "cuStreamDestroy",
  "cuStreamDestroy_v2",
  "cuStreamEndCapture",
  "cuStreamEndCapture_ptsz",
  "cuStreamGetAttribute",
  "cuStreamGetAttribute_ptsz",
  "cuStreamGetCaptureInfo",
  "cuStreamGetCaptureInfo_ptsz",
  "cuStreamGetCtx",
  "cuStreamGetCtx_ptsz",
  "cuStreamGetFlags",
  "cuStreamGetFlags_ptsz",
  "cuStreamGetPriority",
  "cuStreamGetPriority_ptsz",
  "cuStreamIsCapturing",
  "cuStreamIsCapturing_ptsz",
  "cuStreamQuery",
  "cuStreamQuery_ptsz",
  "cuStreamSetAttribute",
  "cuStreamSetAttribute_ptsz",
  "cuStreamSynchronize",
  "cuStreamSynchronize_ptsz",
  "cuStreamWaitEvent",
  "cuStreamWaitEvent_ptsz",
  "cuStreamWaitValue32",
  "cuStreamWaitValue32_ptsz",
  "cuStreamWaitValue64",
  "cuStreamWaitValue64_ptsz",
  "cuStreamWriteValue32",
  "cuStreamWriteValue32_ptsz",
  "cuStreamWriteValue64",
  "cuStreamWriteValue64_ptsz",
  "cuSurfObjectCreate",
  "cuSurfObjectDestroy",
  "cuSurfObjectGetResourceDesc",
  "cuSurfRefGetArray",
  "cuSurfRefSetArray",
  "cuTexObjectCreate",
  "cuTexObjectDestroy",
  "cuTexObjectGetResourceDesc",
  "cuTexObjectGetResourceViewDesc",
  "cuTexObjectGetTextureDesc",
  "cuTexRefCreate",
  "cuTexRefDestroy",
  "cuTexRefGetAddress",
  "cuTexRefGetAddressMode",
  "cuTexRefGetAddress_v2",
  "cuTexRefGetArray",
  "cuTexRefGetBorderColor",
  "cuTexRefGetFilterMode",
  "cuTexRefGetFlags",
  "cuTexRefGetFormat",
  "cuTexRefGetMaxAnisotropy",
  "cuTexRefGetMipmapFilterMode",
  "cuTexRefGetMipmapLevelBias",
  "cuTexRefGetMipmapLevelClamp",
  "cuTexRefGetMipmappedArray",
  "cuTexRefSetAddress",
  "cuTexRefSetAddress2D",
  "cuTexRefSetAddress2D_v2",
  "cuTexRefSetAddress2D_v3",
  "cuTexRefSetAddressMode",
  "cuTexRefSetAddress_v2",
  "cuTexRefSetArray",
  "cuTexRefSetBorderColor",
  "cuTexRefSetFilterMode",
  "cuTexRefSetFlags",
  "cuTexRefSetFormat",
  "cuTexRefSetMaxAnisotropy",
  "cuTexRefSetMipmapFilterMode",
  "cuTexRefSetMipmapLevelBias",
  "cuTexRefSetMipmapLevelClamp",
  "cuTexRefSetMipmappedArray",
  "cuThreadExchangeStreamCaptureMode",
  "cuVDPAUCtxCreate",
  "cuVDPAUCtxCreate_v2",
  "cuVDPAUGetDevice",
  "cuWaitExternalSemaphoresAsync",
  "cuWaitExternalSemaphoresAsync_ptsz",
  "cudbgApiAttach",
  "cudbgApiDetach",
  "cudbgApiInit",
  "cudbgGetAPI",
  "cudbgGetAPIVersion",
  "cudbgMain",
  "cudbgReportDriverApiError",
  "cudbgReportDriverInternalError",
  0
};

extern void *_libcuda_so_1_tramp_table[];

// Can be sped up by manually parsing library symtab...
void _libcuda_so_1_tramp_resolve(int i) {
  assert((unsigned)i + 1 < sizeof(sym_names) / sizeof(sym_names[0]));

  CHECK(!is_lib_loading, "library function '%s' called during library load", sym_names[i]);

  void *h = 0;
#if NO_DLOPEN
  // FIXME: instead of RTLD_NEXT we should search for loaded lib_handle
  // as in https://github.com/jethrogb/ssltrace/blob/bf17c150a7/ssltrace.cpp#L74-L112
  h = RTLD_NEXT;
#elif LAZY_LOAD
  h = load_library();
#else
  h = lib_handle;
  CHECK(h, "failed to resolve symbol '%s', library failed to load", sym_names[i]);
#endif

  // Dlsym is thread-safe so don't need to protect it.
  _libcuda_so_1_tramp_table[i] = dlsym(h, sym_names[i]);
  CHECK(_libcuda_so_1_tramp_table[i], "failed to resolve symbol '%s'", sym_names[i]);
}

// Helper for user to resolve all symbols
void _libcuda_so_1_tramp_resolve_all(void) {
  size_t i;
  for(i = 0; i + 1 < sizeof(sym_names) / sizeof(sym_names[0]); ++i)
    _libcuda_so_1_tramp_resolve(i);
}

#ifdef __cplusplus
}  // extern "C"
#endif
