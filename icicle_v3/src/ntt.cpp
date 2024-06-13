#include "icicle/ntt.h"
#include "icicle/dispatcher.h"

namespace icicle {

  /*************************** NTT ***************************/
  ICICLE_DISPATCHER_INST(NttDispatcher, ntt, NttImpl);

  extern "C" eIcicleError
  CONCAT_EXPAND(FIELD, ntt)(const scalar_t* input, int size, NTTDir dir, NTTConfig<scalar_t>& config, scalar_t* output)
  {
    return NttDispatcher::execute(input, size, dir, config, output);
  }

  template <>
  eIcicleError ntt(const scalar_t* input, int size, NTTDir dir, NTTConfig<scalar_t>& config, scalar_t* output)
  {
    return CONCAT_EXPAND(FIELD, ntt)(input, size, dir, config, output);
  }

#ifdef EXT_FIELD
  ICICLE_DISPATCHER_INST(NttExtFieldDispatcher, ntt_ext_field, NttExtFieldImpl);

  extern "C" eIcicleError CONCAT_EXPAND(FIELD, ntt_ext_field)(
    const extension_t* input, int size, NTTDir dir, NTTConfig<scalar_t>& config, extension_t* output)
  {
    return NttExtFieldDispatcher::execute(input, size, dir, config, output);
  }

  template <>
  eIcicleError ntt(const extension_t* input, int size, NTTDir dir, NTTConfig<scalar_t>& config, extension_t* output)
  {
    return CONCAT_EXPAND(FIELD, ntt_ext_field)(input, size, dir, config, output);
  }
#endif // EXT_FIELD

  /*************************** INIT DOMAIN ***************************/
  ICICLE_DISPATCHER_INST(NttInitDomainDispatcher, ntt_init_domain, NttInitDomainImpl);

  extern "C" eIcicleError
  CONCAT_EXPAND(FIELD, ntt_init_domain)(const scalar_t& primitive_root, const NTTInitDomainConfig& config)
  {
    return NttInitDomainDispatcher::execute(primitive_root, config);
  }

  template <>
  eIcicleError ntt_init_domain(const scalar_t& primitive_root, const NTTInitDomainConfig& config)
  {
    return CONCAT_EXPAND(FIELD, ntt_init_domain)(primitive_root, config);
  }

  /*************************** RELEASE DOMAIN ***************************/
  ICICLE_DISPATCHER_INST(NttReleaseDomainDispatcher, ntt_release_domain, NttReleaseDomainImpl);

  extern "C" eIcicleError CONCAT_EXPAND(FIELD, ntt_release_domain)() { return NttReleaseDomainDispatcher::execute(); }

  template <>
  eIcicleError ntt_release_domain<scalar_t>()
  {
    return CONCAT_EXPAND(FIELD, ntt_release_domain)();
  }

  /*************************** GET ROOT OF UNITY ***************************/
  extern "C" scalar_t CONCAT_EXPAND(FIELD, get_root_of_unity)(uint64_t max_size)
  {
    const auto log_max_size = static_cast<uint32_t>(std::ceil(std::log2(max_size)));
    return scalar_t::omega(log_max_size);
  }

  template <>
  scalar_t get_root_of_unity(uint64_t max_size)
  {
    return CONCAT_EXPAND(FIELD, get_root_of_unity)(max_size);
  }
} // namespace icicle