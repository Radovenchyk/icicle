#include "icicle/backend/hash/merkle_tree_backend.h"
#include "icicle/errors.h"

namespace icicle {

  class CPUMerkleTreeBackend : public MerkleTreeBackend
  {
  public:
    CPUMerkleTreeBackend(
      const std::vector<Hash>& layer_hashes, uint64_t leaf_element_size_in_limbs, uint64_t output_store_min_layer = 0)
        : MerkleTreeBackend(layer_hashes, leaf_element_size_in_limbs, output_store_min_layer)
    {
    }

    ~CPUMerkleTreeBackend() = default;

    eIcicleError
    build(const limb_t* leaves, const MerkleTreeConfig& config, const limb_t* secondary_leaves = nullptr) override
    {
      ICICLE_LOG_INFO << "in CPU CPUMerkleTreeBackend::build()";
      // TODO implement
      return eIcicleError::SUCCESS;
    }

    eIcicleError get_root(const limb_t*& root) const override { return eIcicleError::SUCCESS; }

    eIcicleError
    get_path(const limb_t* leaves, uint64_t element_idx, limb_t* path, const MerkleTreeConfig& config) const override
    {
      ICICLE_LOG_INFO << "in CPU CPUMerkleTreeBackend::get_path()";
      // TODO implement
      return eIcicleError::SUCCESS;
    }

    eIcicleError
    verify(const limb_t* path, uint64_t element_idx, bool& verification_valid, const MerkleTreeConfig& config) override
    {
      ICICLE_LOG_INFO << "in CPU CPUMerkleTreeBackend::verify()";
      // TODO implement
      return eIcicleError::SUCCESS;
    };
  };

  eIcicleError create_merkle_tree_backend(
    const Device& device,
    const std::vector<Hash>& layer_hashes,
    uint64_t leaf_element_size_in_limbs,
    uint64_t output_store_min_layer,
    std::shared_ptr<MerkleTreeBackend>& backend /*OUT*/)
  {
    backend = std::make_shared<CPUMerkleTreeBackend>(layer_hashes, leaf_element_size_in_limbs, output_store_min_layer);
    return eIcicleError::SUCCESS;
  }

  REGISTER_MERKLE_TREE_FACTORY_BACKEND("CPU", create_merkle_tree_backend);

} // namespace icicle