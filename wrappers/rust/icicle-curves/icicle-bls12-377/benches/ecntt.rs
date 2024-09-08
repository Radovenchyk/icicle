#[cfg(not(feature = "no_ecntt"))]
use icicle_bls12_377::curve::{CurveCfg, ScalarField};

#[cfg(not(feature = "no_ecntt"))]
use icicle_core::impl_ecntt_bench;
#[cfg(not(feature = "no_ecntt"))]
impl_ecntt_bench!("bls12_377", ScalarField, CurveCfg);

#[cfg(feature = "no_ecntt")]
fn main() {}