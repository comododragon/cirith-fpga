#include "sgemm.h"

__attribute__((reqd_work_group_size(1,1,1)))
__kernel void sgemm(__global float * restrict A, __global float * restrict B, __global float * restrict C) {
<HEADER>

	<LOOP_0_1> for(int mm = 0; mm < M; mm++) {
		<LOOP_0_2> for(int nn = 0; nn < N; nn++) {
			float c = 0.0f;

			<LOOP_0_3> for(int i = 0; i < K; i += <LINCR>) {
				<LPREAMB>
				float a_<LCTR>;
				float b_<LCTR>;

				<LBEGIN>

				a_<LCTR> = <ARR_A>[mm + (i + <LITER>) * M];
				b_<LCTR> = <ARR_B>[nn + (i + <LITER>) * N];

				<LFRONT>

				c += a_<LCTR> * b_<LCTR>;

				<LEND>
			}

			<ARR_C>[mm + nn * M] = <ARR_C>[mm + nn * M] * BETA + ALPHA * c;
		}
	}

<FOOTER>
}
