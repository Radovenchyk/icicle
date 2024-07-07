#include <iostream>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string>

#include "icicle/runtime.h"
#include "icicle/device_api.h"
#include "icicle/errors.h"
#include "icicle/utils/log.h"

using namespace icicle;

extern "C" eIcicleError icicle_set_device(const Device& device) { return DeviceAPI::set_thread_local_device(device); }

extern "C" eIcicleError icicle_get_active_device(icicle::Device& device)
{
  const Device& active_device = DeviceAPI::get_thread_local_device();
  device = active_device;
  return eIcicleError::SUCCESS;
}

extern "C" eIcicleError icicle_is_host_memory(const void* ptr)
{
  auto it = DeviceAPI::get_global_memory_tracker().identify_device(ptr);
  return (it == std::nullopt) ? eIcicleError::SUCCESS : eIcicleError::INVALID_POINTER;
}

extern "C" eIcicleError icicle_is_active_device_memory(const void* ptr)
{
  auto it = DeviceAPI::get_global_memory_tracker().identify_device(ptr);
  if (it == std::nullopt) { return eIcicleError::INVALID_POINTER; }
  return (**it == DeviceAPI::get_thread_local_device()) ? eIcicleError::SUCCESS : eIcicleError::INVALID_POINTER;
}

extern "C" eIcicleError icicle_get_device_count(int& device_count /*OUT*/)
{
  return DeviceAPI::get_thread_local_deviceAPI()->get_device_count(device_count);
}

extern "C" eIcicleError icicle_malloc(void** ptr, size_t size)
{
  return DeviceAPI::get_thread_local_deviceAPI()->allocate_memory(ptr, size, DeviceAPI::get_global_memory_tracker());
}

extern "C" eIcicleError icicle_malloc_async(void** ptr, size_t size, icicleStreamHandle stream)
{
  return DeviceAPI::get_thread_local_deviceAPI()->allocate_memory_async(
    ptr, size, stream, DeviceAPI::get_global_memory_tracker());
}

extern "C" eIcicleError icicle_free(void* ptr)
{
  // If releasing memory of non-active device, switch device, release and switch back
  // Alternatively I would have to consider it an error but it means that user has to switch device before dropping
  // (when scope is closed)

  auto& tracker = DeviceAPI::get_global_memory_tracker();
  const auto& ptr_dev = tracker.identify_device(ptr);
  const auto& cur_device = DeviceAPI::get_thread_local_device();

  if (ptr_dev == std::nullopt) {
    ICICLE_LOG_ERROR << "Trying to release host memory from device " << cur_device.type << " " << cur_device.id;
    return eIcicleError::INVALID_DEVICE;
  }
  const bool is_active_device = **ptr_dev == cur_device;
  if (is_active_device) {
    return DeviceAPI::get_thread_local_deviceAPI()->free_memory(ptr, DeviceAPI::get_global_memory_tracker());
  }

  // Getting here means memory does not belong to active device
  auto err = icicle_set_device(**ptr_dev);
  if (err == eIcicleError::SUCCESS) err = icicle_free(ptr);
  err = icicle_set_device(cur_device);
  return err;
}

extern "C" eIcicleError icicle_free_async(void* ptr, icicleStreamHandle stream)
{
  return DeviceAPI::get_thread_local_deviceAPI()->free_memory_async(
    ptr, stream, DeviceAPI::get_global_memory_tracker());
}

extern "C" eIcicleError icicle_get_available_memory(size_t& total /*OUT*/, size_t& free /*OUT*/)
{
  return DeviceAPI::get_thread_local_deviceAPI()->get_available_memory(total, free);
}

extern "C" eIcicleError icicle_copy_to_host(void* dst, const void* src, size_t size)
{
  return DeviceAPI::get_thread_local_deviceAPI()->copy_to_host(dst, src, size);
}

extern "C" eIcicleError icicle_copy_to_host_async(void* dst, const void* src, size_t size, icicleStreamHandle stream)
{
  return DeviceAPI::get_thread_local_deviceAPI()->copy_to_host_async(dst, src, size, stream);
}

extern "C" eIcicleError icicle_copy_to_device(void* dst, const void* src, size_t size)
{
  return DeviceAPI::get_thread_local_deviceAPI()->copy_to_device(dst, src, size);
}

extern "C" eIcicleError icicle_copy_to_device_async(void* dst, const void* src, size_t size, icicleStreamHandle stream)
{
  return DeviceAPI::get_thread_local_deviceAPI()->copy_to_device_async(dst, src, size, stream);
}

extern "C" eIcicleError icicle_stream_synchronize(icicleStreamHandle stream)
{
  return DeviceAPI::get_thread_local_deviceAPI()->synchronize(stream);
}

extern "C" eIcicleError icicle_device_synchronize()
{
  return DeviceAPI::get_thread_local_deviceAPI()->synchronize(nullptr);
}

extern "C" eIcicleError icicle_get_device_properties(DeviceProperties& properties)
{
  return DeviceAPI::get_thread_local_deviceAPI()->get_device_properties(properties);
}

extern "C" eIcicleError icicle_is_device_avialable(const Device& dev)
{
  return is_device_registered(dev.type) ? eIcicleError::SUCCESS : eIcicleError::INVALID_DEVICE;
}

extern "C" eIcicleError icicle_get_registered_devices(char* output, size_t output_size)
{
  return get_registered_devices(output, output_size);
}

extern "C" eIcicleError icicle_create_stream(icicleStreamHandle* stream)
{
  return DeviceAPI::get_thread_local_deviceAPI()->create_stream(stream);
}

extern "C" eIcicleError icicle_destroy_stream(icicleStreamHandle stream)
{
  return DeviceAPI::get_thread_local_deviceAPI()->destroy_stream(stream);
}

// Determine the shared library extension based on the operating system
#ifdef __linux__
const std::string SHARED_LIB_EXTENSION = ".so";
#elif __APPLE__
const std::string SHARED_LIB_EXTENSION = ".dylib";
#else
#error "Unsupported operating system"
#endif

extern "C" eIcicleError icicle_load_backend(const char* path, bool is_recursive)
{
  auto is_directory = [](const char* path) {
    struct stat pathStat;
    if (stat(path, &pathStat) != 0) { return false; }
    return S_ISDIR(pathStat.st_mode);
  };

  auto is_shared_library = [](const std::string& filename) {
    return filename.size() >= SHARED_LIB_EXTENSION.size() &&
           filename.compare(
             filename.size() - SHARED_LIB_EXTENSION.size(), SHARED_LIB_EXTENSION.size(), SHARED_LIB_EXTENSION) == 0;
  };

  auto load_library = [](const char* filePath) {
    ICICLE_LOG_DEBUG << "Attempting load: " << filePath;
    void* handle = dlopen(filePath, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) { ICICLE_LOG_ERROR << "Failed to load " << filePath << ": " << dlerror(); }
  };

  if (is_directory(path)) {
    // Path is a directory, recursively search for libraries
    DIR* dir = opendir(path);
    if (!dir) {
      ICICLE_LOG_ERROR << "Cannot open directory: " << path;
      return eIcicleError::INVALID_ARGUMENT;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
      const std::string& entryPath = std::string(path) + "/" + entry->d_name;

      // Skip "." and ".." entries
      if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") { continue; }

      // Recurse into subdirectories and load libraries in files
      const bool is_nested_dir = is_directory(entryPath.c_str());
      if (is_recursive || !is_nested_dir) { icicle_load_backend(entryPath.c_str(), is_recursive); }
    }

    closedir(dir);
  } else if (is_shared_library(path)) {
    load_library(path);
  }

  return eIcicleError::SUCCESS;
}