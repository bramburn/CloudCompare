// C++ implementation of the CXX bridge for `ICPRegistrationTools::Register`.
//
// Wraps the C++ ICP call behind a flat-array + params-struct ABI
// so the Rust side can drive it without exposing CCCoreLib's
// virtual cloud types (`GenericIndexedCloudPersist*`,
// `GenericIndexedMesh*`).

#include "src/cpp/icp_shim.h"   // this file's header (cxx-build's include path includes the project root)
#include "cc_rust/src/ffi.rs.h"  // CXX auto-generated shared types (declares IcpParamsCpp / IcpResultCpp in global namespace)

// CCCoreLib headers. Note: `CCVector3`, `CCVector3d`, and
// `CCCoreLib::SquareMatrixd` are all in the global namespace
// (CCGeom.h's `using` declarations are not wrapped in
// `namespace CCCoreLib`), so we can refer to them without
// the `CCCoreLib::` prefix in some places.
#include <CCCoreLib.h>
#include <CCGeom.h>          // CCVector3 (float[3]), CCVector3d (double[3])
#include <PointCloud.h>      // CCCoreLib::PointCloud, transitively brings CCGeom.h via SquareMatrix.h
#include <GenericProgressCallback.h>
#include <RegistrationTools.h>

#include <cstddef>
#include <cstdint>

IcpResultCpp run_icp_cpp(
    const float* model_points,
    std::size_t model_count,
    const float* data_points,
    std::size_t data_count,
    const IcpParamsCpp& params)
{
    // Build model cloud (reference; won't move). PointCloud
    // has only a default constructor — the cloud name is a
    // qCC_db concept (ccPointCloud) that CCCoreLib doesn't carry.
    CCCoreLib::PointCloud modelCloud;
    if (model_count % 3 != 0) {
        // Malformed input — return an error result with rms = -1.
        IcpResultCpp err{};
        err.result_code = 105;  // ICP_ERROR_INVALID_INPUT
        err.rms = -1.0;
        return err;
    }
    const unsigned nModel = static_cast<unsigned>(model_count / 3);
    modelCloud.reserve(nModel);
    for (unsigned i = 0; i < nModel; ++i) {
        const float* p = &model_points[i * 3];
        // `CCVector3` is `Vector3Tpl<float>` from CCGeom.h
        // (defined at the global namespace).
        modelCloud.addPoint(CCVector3(p[0], p[1], p[2]));
    }

    // Build data cloud (to be aligned; will move).
    CCCoreLib::PointCloud dataCloud;
    if (data_count % 3 != 0) {
        IcpResultCpp err{};
        err.result_code = 105;
        err.rms = -1.0;
        return err;
    }
    const unsigned nData = static_cast<unsigned>(data_count / 3);
    dataCloud.reserve(nData);
    for (unsigned i = 0; i < nData; ++i) {
        const float* p = &data_points[i * 3];
        dataCloud.addPoint(CCVector3(p[0], p[1], p[2]));
    }

    // Map Rust params to C++ params. All other fields use the
    // C++ default constructor (no weights, no sampling limit
    // trimming, etc.).
    CCCoreLib::ICPRegistrationTools::Parameters icpParams;
    icpParams.minRMSDecrease     = params.min_rms_decrease;
    icpParams.nbMaxIterations   = params.nb_max_iterations;
    icpParams.adjustScale        = params.adjust_scale;
    icpParams.samplingLimit     = params.sampling_limit;
    icpParams.finalOverlapRatio = params.final_overlap_ratio;

    // Call ICP. Pass nullptr for the optional mesh (cloud-vs-cloud)
    // and the optional progress callback (parity tests don't need it).
    //
    // Important: the C++ `Transformation` default constructor
    //   `Transformation() : s(1.0) {}`
    // leaves R and T **uninitialized**. If ICP returns
    // ICP_NOTHING_TO_DO, R and T may not be written at all (the
    // library exits early in `FindBestRegistration` and never fills
    // them in). Reading uninitialized members is UB; on Windows/MSVC
    // it can STATUS_ACCESS_VIOLATION. We pre-initialize R to the
    // 3×3 identity and T to (0, 0, 0) so callers can always
    // inspect the returned `Transformation` safely — even on
    // early-out returns.
    CCCoreLib::PointProjectionTools::Transformation totalTrans;
    totalTrans.R = CCCoreLib::SquareMatrixd(3);  // zero-initialized
    // SquareMatrixTpl(3) is zero-init; explicitly set the diagonal
    // to 1 so the "no transform applied" case is well-defined.
    totalTrans.R.setValue(0, 0, 1.0);
    totalTrans.R.setValue(1, 1, 1.0);
    totalTrans.R.setValue(2, 2, 1.0);
    totalTrans.T = CCVector3d(0.0, 0.0, 0.0);
    // s is already 1.0 from the default ctor.
    double finalRMS = -1.0;
    unsigned finalPointCount = 0;
    auto resultCode = CCCoreLib::ICPRegistrationTools::Register(
        &modelCloud,
        nullptr,    // modelMesh
        &dataCloud,
        icpParams,
        totalTrans,
        finalRMS,
        finalPointCount,
        nullptr);   // progressCb

    // Pack result. The C++ `Transformation` is:
    //   P' = s * R * P + T
    // where R is row-major 3x3 (`CCCoreLib::SquareMatrixd`, a
    // `SquareMatrixTpl<double>`), T is `CCVector3d` (double[3]),
    // and s is double. We pass each matrix element individually
    // (CXX 1.0 doesn't support fixed-size array struct fields
    // directly). SquareMatrixTpl exposes `getValue(row, col)`,
    // not `operator()(row, col)`.
    const CCCoreLib::SquareMatrixd& R = totalTrans.R;
    IcpResultCpp result;
    result.result_code      = static_cast<std::int32_t>(resultCode);
    result.rms              = finalRMS;
    result.final_point_count = finalPointCount;
    result.scale            = totalTrans.s;
    result.r00 = R.getValue(0, 0); result.r01 = R.getValue(0, 1); result.r02 = R.getValue(0, 2);
    result.r10 = R.getValue(1, 0); result.r11 = R.getValue(1, 1); result.r12 = R.getValue(1, 2);
    result.r20 = R.getValue(2, 0); result.r21 = R.getValue(2, 1); result.r22 = R.getValue(2, 2);
    result.tx  = totalTrans.T.x;
    result.ty  = totalTrans.T.y;
    result.tz  = totalTrans.T.z;
    return result;
}
