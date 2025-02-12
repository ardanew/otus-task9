#ifndef ASYNC_H
#define ASYNC_H

#include <cstddef>

#ifdef _MSC_VER
  #ifdef BUILDING_LIBRARY // building async_lib
    #define ASYNC_LIB_EXPORT __declspec(dllexport)
  #else
    #ifdef UNIT_TESTS // building test_async
      #define ASYNC_LIB_EXPORT
    #else // building demo_app
      #define ASYNC_LIB_EXPORT __declspec(dllimport)
    #endif
  #endif
#else // not windows 
  #define ASYNC_LIB_EXPORT
#endif


namespace async
{

using handle_t = void*;

/**
 * @brief connect Create a new command bulks handler
 * @param bulk Bulk size
 * @return Handle to pass to other functions
 */
ASYNC_LIB_EXPORT handle_t connect(std::size_t bulk);

/**
 * @brief receive Pass input to command bulks handler
 * @param handle Handle received from connect
 * @param data String
 * @param size String size
 */
ASYNC_LIB_EXPORT void receive(handle_t handle, const char* data, std::size_t size);

/**
 * @brief Delete command bulks handler
 * @param handle Handle received from connect
 */
ASYNC_LIB_EXPORT void disconnect(handle_t handle);

} // namespace async

#endif // ASYNC_H
