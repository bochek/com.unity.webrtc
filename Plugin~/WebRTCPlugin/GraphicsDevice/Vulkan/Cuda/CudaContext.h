#pragma once

#include <vulkan/vulkan.h>
#include <cuda.h>

namespace unity
{
namespace webrtc
{

class CudaContext {
public:
    CudaContext();
    ~CudaContext() = default;

    CUresult Init(const VkInstance instance, VkPhysicalDevice physicalDevice);
    void Shutdown();
    inline CUcontext GetContext() const;
private:
    CUcontext m_context;

};

inline CUcontext CudaContext::GetContext() const { return m_context; }

} // end namespace webrtc
} // end namespace unity
