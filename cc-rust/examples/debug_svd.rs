// examples/debug_svd.rs — step through the actual ICP iterations.
use nalgebra::{Matrix3, Vector3};
use cc_rust::registration::{self, IcprParamsRust};

fn cube_corners() -> Vec<f32> {
    vec![
        0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0, 1.0, 0.0,  0.0, 0.0, 1.0,
        1.0, 1.0, 0.0,  1.0, 0.0, 1.0,  0.0, 1.0, 1.0,  1.0, 1.0, 1.0,
    ]
}

fn apply(p: &mut [f32], rot: &Matrix3<f64>, t: &Vector3<f64>) {
    for i in 0..p.len() / 3 {
        let v = Vector3::new(p[i*3] as f64, p[i*3+1] as f64, p[i*3+2] as f64);
        let r = rot * v + t;
        p[i*3]   = r[0] as f32;
        p[i*3+1] = r[1] as f32;
        p[i*3+2] = r[2] as f32;
    }
}

fn main() {
    let model = cube_corners();
    let params = IcprParamsRust { max_iterations: 5, min_rms_decrease: 1e-6, ..Default::default() };

    // === translation_only ===
    let mut data = model.clone();
    apply(&mut data, &Matrix3::identity(), &Vector3::new(1.0, 0.0, 0.0));
    println!("=== translation_only ===");
    println!("Initial data[:9] = {:?}", &data[..9]);
    let r = registration::icp_iterate(&mut data, &model, &params).expect("icp failed");
    println!("Final data[:9] = {:?}", &data[..9]);
    println!("Final model[:9] = {:?}", &model[..9]);
    println!("Diff data-model[:9] = {:?}", {
        let mut d = vec![0.0; 9];
        for i in 0..9 { d[i] = data[i] - model[i]; }
        d
    });
    println!("transform = {:?}", r.transform);
    println!("translation = ({}, {}, {})", r.transform[12], r.transform[13], r.transform[14]);
    println!("rms = {}, converged = {}, iter = {}", r.rms, r.converged, r.iterations);
    println!();

    // === rotation_only ===
    let r30 = Matrix3::new(
        30.0_f64.to_radians().cos(), -30.0_f64.to_radians().sin(), 0.0,
        30.0_f64.to_radians().sin(),  30.0_f64.to_radians().cos(), 0.0,
        0.0, 0.0, 1.0,
    );
    let mut data = model.clone();
    apply(&mut data, &r30, &Vector3::zeros());
    println!("=== rotation_only ===");
    println!("Initial data[:9] = {:?}", &data[..9]);
    let r = registration::icp_iterate(&mut data, &model, &params).expect("icp failed");
    println!("Final data[:9] = {:?}", &data[..9]);
    println!("Final model[:9] = {:?}", &model[..9]);
    println!("Diff data-model[:9] = {:?}", {
        let mut d = vec![0.0; 9];
        for i in 0..9 { d[i] = data[i] - model[i]; }
        d
    });
    println!("transform = {:?}", r.transform);
    println!("translation = ({}, {}, {})", r.transform[12], r.transform[13], r.transform[14]);
    println!("rms = {}, converged = {}, iter = {}", r.rms, r.converged, r.iterations);
}
