use crate::traits::FieldImpl;
use icicle_runtime::{
    config::ConfigExtension, errors::eIcicleError, memory::HostOrDeviceSlice, stream::IcicleStreamHandle,
};

pub mod tests;

#[repr(C)]
#[derive(Debug, Clone)]
pub struct VecOpsConfig {
    pub stream_handle: IcicleStreamHandle,
    pub is_a_on_device: bool,
    pub is_b_on_device: bool,
    pub is_result_on_device: bool,
    pub is_async: bool,
    pub ext: ConfigExtension,
}

impl VecOpsConfig {
    pub fn default() -> Self {
        Self {
            stream_handle: std::ptr::null_mut(),
            is_a_on_device: false,
            is_b_on_device: false,
            is_result_on_device: false,
            is_async: false,
            ext: ConfigExtension::new(),
        }
    }
}

#[doc(hidden)]
pub trait VecOps<F> {
    fn add(
        a: &(impl HostOrDeviceSlice<F> + ?Sized),
        b: &(impl HostOrDeviceSlice<F> + ?Sized),
        result: &mut (impl HostOrDeviceSlice<F> + ?Sized),
        cfg: &VecOpsConfig,
    ) -> Result<(), eIcicleError>;

    fn sub(
        a: &(impl HostOrDeviceSlice<F> + ?Sized),
        b: &(impl HostOrDeviceSlice<F> + ?Sized),
        result: &mut (impl HostOrDeviceSlice<F> + ?Sized),
        cfg: &VecOpsConfig,
    ) -> Result<(), eIcicleError>;

    fn mul(
        a: &(impl HostOrDeviceSlice<F> + ?Sized),
        b: &(impl HostOrDeviceSlice<F> + ?Sized),
        result: &mut (impl HostOrDeviceSlice<F> + ?Sized),
        cfg: &VecOpsConfig,
    ) -> Result<(), eIcicleError>;

    // fn transpose(
    //     input: &(impl HostOrDeviceSlice<F> + ?Sized),
    //     row_size: u32,
    //     column_size: u32,
    //     output: &mut (impl HostOrDeviceSlice<F> + ?Sized),
    //     ctx: &DeviceContext,
    //     on_device: bool,
    //     is_async: bool,
    // ) -> Result<(), eIcicleError>;

    // fn bit_reverse(
    //     input: &(impl HostOrDeviceSlice<F> + ?Sized),
    //     cfg: &BitReverseConfig,
    //     output: &mut (impl HostOrDeviceSlice<F> + ?Sized),
    // ) -> Result<(), eIcicleError>;

    // fn bit_reverse_inplace(
    //     input: &mut (impl HostOrDeviceSlice<F> + ?Sized),
    //     cfg: &BitReverseConfig,
    // ) -> Result<(), eIcicleError>;
}

fn check_vec_ops_args<'a, F>(
    a: &(impl HostOrDeviceSlice<F> + ?Sized),
    b: &(impl HostOrDeviceSlice<F> + ?Sized),
    result: &(impl HostOrDeviceSlice<F> + ?Sized),
    cfg: &VecOpsConfig,
) -> VecOpsConfig {
    if a.len() != b.len() || a.len() != result.len() {
        panic!(
            "left, right and output lengths {}; {}; {} do not match",
            a.len(),
            b.len(),
            result.len()
        );
    }
    // let ctx_device_id = cfg
    //     .ctx
    //     .device_id;
    // if let Some(device_id) = a.device_id() {
    //     assert_eq!(device_id, ctx_device_id, "Device ids in a and context are different");
    // }
    // if let Some(device_id) = b.device_id() {
    //     assert_eq!(device_id, ctx_device_id, "Device ids in b and context are different");
    // }
    // if let Some(device_id) = result.device_id() {
    //     assert_eq!(
    //         device_id, ctx_device_id,
    //         "Device ids in result and context are different"
    //     );
    // }
    // check_device(ctx_device_id);

    let mut res_cfg = cfg.clone();
    res_cfg.is_a_on_device = a.is_on_device();
    res_cfg.is_b_on_device = b.is_on_device();
    res_cfg.is_result_on_device = result.is_on_device();
    res_cfg
}

pub fn add_scalars<F>(
    a: &(impl HostOrDeviceSlice<F> + ?Sized),
    b: &(impl HostOrDeviceSlice<F> + ?Sized),
    result: &mut (impl HostOrDeviceSlice<F> + ?Sized),
    cfg: &VecOpsConfig,
) -> Result<(), eIcicleError>
where
    F: FieldImpl,
    <F as FieldImpl>::Config: VecOps<F>,
{
    let cfg = check_vec_ops_args(a, b, result, cfg);
    <<F as FieldImpl>::Config as VecOps<F>>::add(a, b, result, &cfg)
}

pub fn sub_scalars<F>(
    a: &(impl HostOrDeviceSlice<F> + ?Sized),
    b: &(impl HostOrDeviceSlice<F> + ?Sized),
    result: &mut (impl HostOrDeviceSlice<F> + ?Sized),
    cfg: &VecOpsConfig,
) -> Result<(), eIcicleError>
where
    F: FieldImpl,
    <F as FieldImpl>::Config: VecOps<F>,
{
    let cfg = check_vec_ops_args(a, b, result, cfg);
    <<F as FieldImpl>::Config as VecOps<F>>::sub(a, b, result, &cfg)
}

pub fn mul_scalars<F>(
    a: &(impl HostOrDeviceSlice<F> + ?Sized),
    b: &(impl HostOrDeviceSlice<F> + ?Sized),
    result: &mut (impl HostOrDeviceSlice<F> + ?Sized),
    cfg: &VecOpsConfig,
) -> Result<(), eIcicleError>
where
    F: FieldImpl,
    <F as FieldImpl>::Config: VecOps<F>,
{
    let cfg = check_vec_ops_args(a, b, result, cfg);
    <<F as FieldImpl>::Config as VecOps<F>>::mul(a, b, result, &cfg)
}

#[macro_export]
macro_rules! impl_vec_ops_field {
    (
        $field_prefix:literal,
        $field_prefix_ident:ident,
        $field:ident,
        $field_config:ident
    ) => {
        mod $field_prefix_ident {

            use crate::vec_ops::{$field, HostOrDeviceSlice};
            // use icicle_core::vec_ops::BitReverseConfig;
            use icicle_core::vec_ops::VecOpsConfig;
            use icicle_runtime::errors::eIcicleError;

            extern "C" {
                #[link_name = concat!($field_prefix, "_vector_add")]
                pub(crate) fn add_scalars_cuda(
                    a: *const $field,
                    b: *const $field,
                    size: u32,
                    cfg: *const VecOpsConfig,
                    result: *mut $field,
                ) -> eIcicleError;

                #[link_name = concat!($field_prefix, "_vector_sub")]
                pub(crate) fn sub_scalars_cuda(
                    a: *const $field,
                    b: *const $field,
                    size: u32,
                    cfg: *const VecOpsConfig,
                    result: *mut $field,
                ) -> eIcicleError;

                #[link_name = concat!($field_prefix, "_vector_mul")]
                pub(crate) fn mul_scalars_cuda(
                    a: *const $field,
                    b: *const $field,
                    size: u32,
                    cfg: *const VecOpsConfig,
                    result: *mut $field,
                ) -> eIcicleError;

                // #[link_name = concat!($field_prefix, "_transpose_matrix_cuda")]
                // pub(crate) fn transpose_cuda(
                //     input: *const $field,
                //     row_size: u32,
                //     column_size: u32,
                //     output: *mut $field,
                //     ctx: *const DeviceContext,
                //     on_device: bool,
                //     is_async: bool,
                // ) -> CudaError;

                // #[link_name = concat!($field_prefix, "_bit_reverse_cuda")]
                // pub(crate) fn bit_reverse_cuda(
                //     input: *const $field,
                //     size: u64,
                //     config: *const BitReverseConfig,
                //     output: *mut $field,
                // ) -> CudaError;
            }
        }

        impl VecOps<$field> for $field_config {
            fn add(
                a: &(impl HostOrDeviceSlice<$field> + ?Sized),
                b: &(impl HostOrDeviceSlice<$field> + ?Sized),
                result: &mut (impl HostOrDeviceSlice<$field> + ?Sized),
                cfg: &VecOpsConfig,
            ) -> Result<(), eIcicleError> {
                unsafe {
                    $field_prefix_ident::add_scalars_cuda(
                        a.as_ptr(),
                        b.as_ptr(),
                        a.len() as u32,
                        cfg as *const VecOpsConfig,
                        result.as_mut_ptr(),
                    )
                    .wrap()
                }
            }

            fn sub(
                a: &(impl HostOrDeviceSlice<$field> + ?Sized),
                b: &(impl HostOrDeviceSlice<$field> + ?Sized),
                result: &mut (impl HostOrDeviceSlice<$field> + ?Sized),
                cfg: &VecOpsConfig,
            ) -> Result<(), eIcicleError> {
                unsafe {
                    $field_prefix_ident::sub_scalars_cuda(
                        a.as_ptr(),
                        b.as_ptr(),
                        a.len() as u32,
                        cfg as *const VecOpsConfig,
                        result.as_mut_ptr(),
                    )
                    .wrap()
                }
            }

            fn mul(
                a: &(impl HostOrDeviceSlice<$field> + ?Sized),
                b: &(impl HostOrDeviceSlice<$field> + ?Sized),
                result: &mut (impl HostOrDeviceSlice<$field> + ?Sized),
                cfg: &VecOpsConfig,
            ) -> Result<(), eIcicleError> {
                unsafe {
                    $field_prefix_ident::mul_scalars_cuda(
                        a.as_ptr(),
                        b.as_ptr(),
                        a.len() as u32,
                        cfg as *const VecOpsConfig,
                        result.as_mut_ptr(),
                    )
                    .wrap()
                }
            }

            // fn transpose(
            //     input: &(impl HostOrDeviceSlice<$field> + ?Sized),
            //     row_size: u32,
            //     column_size: u32,
            //     output: &mut (impl HostOrDeviceSlice<$field> + ?Sized),
            //     ctx: &DeviceContext,
            //     on_device: bool,
            //     is_async: bool,
            // ) -> Result<(), eIcicleError> {
            //     unsafe {
            //         $field_prefix_ident::transpose_cuda(
            //             input.as_ptr(),
            //             row_size,
            //             column_size,
            //             output.as_mut_ptr(),
            //             ctx as *const _ as *const DeviceContext,
            //             on_device,
            //             is_async,
            //         )
            //         .wrap()
            //     }
            // }

            //     fn bit_reverse(
            //         input: &(impl HostOrDeviceSlice<$field> + ?Sized),
            //         cfg: &BitReverseConfig,
            //         output: &mut (impl HostOrDeviceSlice<$field> + ?Sized),
            //     ) -> Result<(), eIcicleError> {
            //         unsafe {
            //             $field_prefix_ident::bit_reverse_cuda(
            //                 input.as_ptr(),
            //                 input.len() as u64,
            //                 cfg as *const BitReverseConfig,
            //                 output.as_mut_ptr(),
            //             )
            //             .wrap()
            //         }
            //     }

            //     fn bit_reverse_inplace(
            //         input: &mut (impl HostOrDeviceSlice<$field> + ?Sized),
            //         cfg: &BitReverseConfig,
            //     ) -> Result<(), eIcicleError> {
            //         unsafe {
            //             $field_prefix_ident::bit_reverse_cuda(
            //                 input.as_ptr(),
            //                 input.len() as u64,
            //                 cfg as *const BitReverseConfig,
            //                 input.as_mut_ptr(),
            //             )
            //             .wrap()
            //         }
            //     }
        }
    };
}

#[macro_export]
macro_rules! impl_vec_ops_tests {
    (
      $field:ident
    ) => {
        use icicle_runtime::{device::Device, runtime};
        use std::sync::Once;

        static INIT: Once = Once::new();

        fn initialize() {
            INIT.call_once(|| {
                // load backends to process
                runtime::load_backend(&env!("DEFAULT_BACKEND_INSTALL_DIR"), true).unwrap();
                let _ = runtime::get_registered_devices().unwrap();
                let cuda_device = Device::new("CUDA", 0);
                let cpu_device = Device::new("CPU", 0);
                if runtime::is_device_available(&cuda_device) {
                    runtime::set_device(&cuda_device).unwrap();
                } else {
                    runtime::set_device(&cpu_device).unwrap();
                }
            });
        }

        #[test]
        pub fn test_vec_ops_scalars() {
            initialize();
            check_vec_ops_scalars::<$field>()
        }

        // #[test]
        // pub fn test_bit_reverse() {
        //     check_bit_reverse::<$field>()
        // }
        // #[test]
        // pub fn test_bit_reverse_inplace() {
        //     check_bit_reverse_inplace::<$field>()
        // }
    };
}