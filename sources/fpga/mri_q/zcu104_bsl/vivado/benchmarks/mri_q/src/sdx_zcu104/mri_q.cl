#include "consts.h"
#include "sincoslt.h"

#define PI   3.1415926535897932384626433832795029f
#define PIx2 6.2831853071795864769252867665590058f

__attribute__((reqd_work_group_size(1,1,1)))
__kernel void mri_q(
	__global float * restrict kx, __global float * restrict ky, __global float * restrict kz, __global float * restrict phiMag,
	__global float * restrict x, __global float * restrict y, __global float * restrict z,
	__global float * restrict Qr, __global float * restrict Qi
) {
<HEADER>

	float expArg;
	float cosArg;
	float sinArg;

	__attribute__((xcl_pipeline_loop)) for(unsigned int i = 0; i < NUMX; i++) {
		<ARR_Qr>[i] = 0;
		<ARR_Qi>[i] = 0;
	}

	<LOOP_0_1> for(int indexK = 0; indexK < NUMK; indexK++) {
		float _kx = <ARR_kx>[indexK];
		float _ky = <ARR_ky>[indexK];
		float _kz = <ARR_kz>[indexK];
		float _phiMag = <ARR_phiMag>[indexK];

		<LOOP_0_2> for(int indexX = 0; indexX < NUMX; indexX++) {
			expArg = PIx2 * (_kx * <ARR_x>[indexX] + _ky * <ARR_y>[indexX] + _kz * <ARR_z>[indexX]);

			COS(cosArg, expArg);
			SIN(sinArg, expArg);

			<ARR_Qr>[indexX] += _phiMag * cosArg;
			<ARR_Qi>[indexX] += _phiMag * sinArg;
		}
	}

<FOOTER>
}
