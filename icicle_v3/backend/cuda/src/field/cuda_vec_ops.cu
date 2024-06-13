#include <cuda.h>
#include <stdexcept>

#include "icicle/errors.h"
#include "icicle/vec_ops.h"
#include "icicle/matrix_ops.h"
#include "gpu-utils/error_handler.h"
#include "error_translation.h"

#define MAX_THREADS_PER_BLOCK 256

template <typename E>
__global__ void mul_kernel(const E* scalar_vec, const E* element_vec, int n, E* result)
{
  int tid = blockDim.x * blockIdx.x + threadIdx.x;
  if (tid < n) { result[tid] = scalar_vec[tid] * element_vec[tid]; }
}

template <typename E, typename S>
__global__ void mul_scalar_kernel(const E* element_vec, const S scalar, int n, E* result)
{
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  if (tid < n) { result[tid] = element_vec[tid] * (scalar); }
}

template <typename E>
__global__ void div_element_wise_kernel(const E* element_vec1, const E* element_vec2, int n, E* result)
{
  // TODO:implement better based on https://eprint.iacr.org/2008/199
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  if (tid < n) { result[tid] = element_vec1[tid] * E::inverse(element_vec2[tid]); }
}

template <typename E>
__global__ void add_kernel(const E* element_vec1, const E* element_vec2, int n, E* result)
{
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  if (tid < n) { result[tid] = element_vec1[tid] + element_vec2[tid]; }
}

template <typename E>
__global__ void sub_kernel(const E* element_vec1, const E* element_vec2, int n, E* result)
{
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  if (tid < n) { result[tid] = element_vec1[tid] - element_vec2[tid]; }
}

template <typename E>
__global__ void transpose_kernel(const E* in, E* out, uint32_t row_size, uint32_t column_size)
{
  int tid = blockDim.x * blockIdx.x + threadIdx.x;
  if (tid >= row_size * column_size) return;
  out[(tid % row_size) * column_size + (tid / row_size)] = in[tid];
}

template <typename E, void (*Kernel)(const E*, const E*, int, E*)>
cudaError_t vec_op(const E* vec_a, const E* vec_b, int n, const VecOpsConfig& config, E* result)
{
  CHK_INIT_IF_RETURN();

  cudaStream_t cuda_stream = reinterpret_cast<cudaStream_t>(config.stream);

  // Set the grid and block dimensions
  int num_threads = MAX_THREADS_PER_BLOCK;
  int num_blocks = (n + num_threads - 1) / num_threads;

  E *d_result, *d_alloc_vec_a, *d_alloc_vec_b;
  const E *d_vec_a, *d_vec_b;
  if (!config.is_a_on_device) {
    CHK_IF_RETURN(cudaMallocAsync(&d_alloc_vec_a, n * sizeof(E), cuda_stream));
    CHK_IF_RETURN(cudaMemcpyAsync(d_alloc_vec_a, vec_a, n * sizeof(E), cudaMemcpyHostToDevice, cuda_stream));
    d_vec_a = d_alloc_vec_a;
  } else {
    d_vec_a = vec_a;
  }

  if (!config.is_b_on_device) {
    CHK_IF_RETURN(cudaMallocAsync(&d_alloc_vec_b, n * sizeof(E), cuda_stream));
    CHK_IF_RETURN(cudaMemcpyAsync(d_alloc_vec_b, vec_b, n * sizeof(E), cudaMemcpyHostToDevice, cuda_stream));
    d_vec_b = d_alloc_vec_b;
  } else {
    d_vec_b = vec_b;
  }

  if (!config.is_result_on_device) {
    CHK_IF_RETURN(cudaMallocAsync(&d_result, n * sizeof(E), cuda_stream));
  } else {
    d_result = result;
  }

  // Call the kernel to perform element-wise operation
  Kernel<<<num_blocks, num_threads, 0, cuda_stream>>>(d_vec_a, d_vec_b, n, d_result);

  if (!config.is_a_on_device) { CHK_IF_RETURN(cudaFreeAsync(d_alloc_vec_a, cuda_stream)); }
  if (!config.is_b_on_device) { CHK_IF_RETURN(cudaFreeAsync(d_alloc_vec_b, cuda_stream)); }

  if (!config.is_result_on_device) {
    CHK_IF_RETURN(cudaMemcpyAsync(result, d_result, n * sizeof(E), cudaMemcpyDeviceToHost, cuda_stream));
    CHK_IF_RETURN(cudaFreeAsync(d_result, cuda_stream));
  }

  if (!config.is_async) return CHK_STICKY(cudaStreamSynchronize(cuda_stream));

  return CHK_LAST();
}

template <typename E>
eIcicleError mul(const E* vec_a, const E* vec_b, int n, const VecOpsConfig& config, E* result)
{
  cudaError_t err = vec_op<E, mul_kernel>(vec_a, vec_b, n, config, result);
  return translateCudaError(err);
}

template <typename E>
eIcicleError add(const E* vec_a, const E* vec_b, int n, const VecOpsConfig& config, E* result)
{
  cudaError_t err = vec_op<E, add_kernel>(vec_a, vec_b, n, config, result);
  return translateCudaError(err);
}

template <typename E>
eIcicleError sub(const E* vec_a, const E* vec_b, int n, const VecOpsConfig& config, E* result)
{
  cudaError_t err = vec_op<E, sub_kernel>(vec_a, vec_b, n, config, result);
  return translateCudaError(err);
}

template <typename E>
cudaError_t transpose_matrix(
  const E* mat_in,
  E* mat_out,
  uint32_t row_size,
  uint32_t column_size,
  cudaStream_t stream,
  bool on_device,
  bool is_async)
{
  int number_of_threads = MAX_THREADS_PER_BLOCK;
  int number_of_blocks = (row_size * column_size + number_of_threads - 1) / number_of_threads;

  const E* d_mat_in;
  E* d_allocated_input = nullptr;
  E* d_mat_out;
  if (!on_device) {
    CHK_IF_RETURN(cudaMallocAsync(&d_allocated_input, row_size * column_size * sizeof(E), stream));
    CHK_IF_RETURN(
      cudaMemcpyAsync(d_allocated_input, mat_in, row_size * column_size * sizeof(E), cudaMemcpyHostToDevice, stream));

    CHK_IF_RETURN(cudaMallocAsync(&d_mat_out, row_size * column_size * sizeof(E), stream));
    d_mat_in = d_allocated_input;
  } else {
    d_mat_in = mat_in;
    d_mat_out = mat_out;
  }

  transpose_kernel<<<number_of_blocks, number_of_threads, 0, stream>>>(d_mat_in, d_mat_out, row_size, column_size);

  if (!on_device) {
    CHK_IF_RETURN(
      cudaMemcpyAsync(mat_out, d_mat_out, row_size * column_size * sizeof(E), cudaMemcpyDeviceToHost, stream));
    CHK_IF_RETURN(cudaFreeAsync(d_mat_out, stream));
    CHK_IF_RETURN(cudaFreeAsync(d_allocated_input, stream));
  }
  if (!is_async) return CHK_STICKY(cudaStreamSynchronize(stream));

  return CHK_LAST();
}

/************************************ REGISTRATION ************************************/

#include "icicle/fields/field_config.h"
using namespace field_config;

template <typename F>
eIcicleError
add_cuda(const Device& device, const F* vec_a, const F* vec_b, int n, const VecOpsConfig& config, F* result)
{
  return add<F>(vec_a, vec_b, n, config, result);
}

template <typename F>
eIcicleError
sub_cuda(const Device& device, const F* vec_a, const F* vec_b, int n, const VecOpsConfig& config, F* result)
{
  return sub<F>(vec_a, vec_b, n, config, result);
}

template <typename F>
eIcicleError
mul_cuda(const Device& device, const F* vec_a, const F* vec_b, int n, const VecOpsConfig& config, F* result)
{
  return mul<F>(vec_a, vec_b, n, config, result);
}

template <typename F>
eIcicleError matrix_transpose_cuda(
  const Device& device, const F* in, uint32_t nof_rows, uint32_t nof_cols, const MatrixOpsConfig& config, F* out)
{
  // TODO relax this limitation
  ICICLE_ASSERT(config.is_input_on_device == config.is_output_on_device)
    << "CUDA matrix transpose expects both input and output on host or on device";
  cudaStream_t cuda_stream = reinterpret_cast<cudaStream_t>(config.stream);
  auto err = transpose_matrix(in, out, nof_cols, nof_rows, cuda_stream, config.is_input_on_device, config.is_async);
  return translateCudaError(err);
}

REGISTER_VECTOR_ADD_BACKEND("CUDA", add_cuda<scalar_t>);
REGISTER_VECTOR_SUB_BACKEND("CUDA", sub_cuda<scalar_t>);
REGISTER_VECTOR_MUL_BACKEND("CUDA", mul_cuda<scalar_t>);
REGISTER_MATRIX_TRANSPOSE_BACKEND("CUDA", matrix_transpose_cuda<scalar_t>);

#ifdef EXT_FIELD
REGISTER_VECTOR_ADD_EXT_FIELD_BACKEND("CUDA", add_cuda<extension_t>);
REGISTER_VECTOR_SUB_EXT_FIELD_BACKEND("CUDA", sub_cuda<extension_t>);
REGISTER_VECTOR_MUL_EXT_FIELD_BACKEND("CUDA", mul_cuda<extension_t>);
REGISTER_MATRIX_TRANSPOSE_EXT_FIELD_BACKEND("CUDA", matrix_transpose_cuda<extension_t>);
#endif // EXT_FIELD