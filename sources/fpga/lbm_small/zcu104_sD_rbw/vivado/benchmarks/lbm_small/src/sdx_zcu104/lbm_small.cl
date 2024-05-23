#include "config.h"
#include "lbm_1d_array.h"

#define DFL1 (1.0/ 3.0)
#define DFL2 (1.0/18.0)
#define DFL3 (1.0/36.0)

#define CL_MAGIC_CAST(v) ((<ARRQ_src> unsigned int *) ((<ARRQ_src> void *) (&(v))))
#define CL_FLAG_VAR(v) unsigned int* const _aux_ = CL_MAGIC_CAST(v)

#define CL_TEST_FLAG_SWEEP(g,f,o)     ((*CL_MAGIC_CAST(LOCAL_2(g, FLAGS, o))) & (f))
#define CL_SET_FLAG_SWEEP(g,f,o)      {CL_FLAG_VAR(LOCAL_2(g, FLAGS, o)); (*_aux_) |=  (f);}
#define CL_CLEAR_FLAG_SWEEP(g,f,o)    {CL_FLAG_VAR(LOCAL_2(g, FLAGS, o)); (*_aux_) &= ~(f);}
#define CL_CLEAR_ALL_FLAGS_SWEEP(g,o) {CL_FLAG_VAR(LOCAL_2(g, FLAGS, o)); (*_aux_)  =    0;}

#define CL_TEST_FLAG(g,x,y,z,f)     ((*CL_MAGIC_CAST(GRID_ENTRY(g, x, y, z, FLAGS))) & (f))
#define CL_SET_FLAG(g,x,y,z,f)      {CL_FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_) |=  (f);}
#define CL_CLEAR_FLAG(g,x,y,z,f)    {CL_FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_) &= ~(f);}
#define CL_CLEAR_ALL_FLAGS(g,x,y,z) {CL_FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_)  =    0;}

#define N_DISTR_FUNCS FLAGS

typedef enum {OBSTACLE    = 1 << 0,
              ACCEL       = 1 << 1,
              IN_OUT_FLOW = 1 << 2} CELL_FLAGS;

__attribute__((reqd_work_group_size(1,1,1)))
__kernel void lbm_small(__global float * restrict src, __global float * restrict dst) {
<HEADER>

	size_t margin = 2 * SIZE_X * SIZE_Y * N_CELL_ENTRIES;
	<ARRQ_src> float *src2 = <ARR_src> + margin;
	<ARRQ_dst> float *dst2 = <ARR_dst> + margin;

	SWEEP_VAR

	<LOOP_0_1> SWEEP_START_2( 0, 0, 0, 0, 0, SIZE_Z, <LINCR> )
		<LPREAMB>

		float ux_<LCTR>, uy_<LCTR>, uz_<LCTR>, u2_<LCTR>, rho_<LCTR>;
		bool __tmp_0_<LCTR>;
		float __tmp_1_<LCTR>, __tmp_2_<LCTR>, __tmp_3_<LCTR>, __tmp_4_<LCTR>, __tmp_5_<LCTR>, __tmp_6_<LCTR>, __tmp_7_<LCTR>, __tmp_8_<LCTR>;
		float __tmp_9_<LCTR>, __tmp_10_<LCTR>, __tmp_11_<LCTR>, __tmp_12_<LCTR>, __tmp_13_<LCTR>, __tmp_14_<LCTR>, __tmp_15_<LCTR>, __tmp_16_<LCTR>;
		float __tmp_17_<LCTR>, __tmp_18_<LCTR>, __tmp_19_<LCTR>, __tmp_20_<LCTR>, __tmp_21_<LCTR>, __tmp_22_<LCTR>, __tmp_23_<LCTR>, __tmp_24_<LCTR>;
		float __tmp_25_<LCTR>, __tmp_26_<LCTR>, __tmp_27_<LCTR>, __tmp_28_<LCTR>, __tmp_29_<LCTR>, __tmp_30_<LCTR>, __tmp_31_<LCTR>, __tmp_32_<LCTR>;
		float __tmp_33_<LCTR>, __tmp_34_<LCTR>, __tmp_35_<LCTR>, __tmp_36_<LCTR>, __tmp_37_<LCTR>, __tmp_38_<LCTR>;

		<LBEGIN>

		__tmp_0_<LCTR> = CL_TEST_FLAG_SWEEP( src2, OBSTACLE, <LITER> );
		if(__tmp_0_<LCTR>) {
			__tmp_1_<LCTR> = SRC_C_2( src2, <LITER> );
			__tmp_2_<LCTR> = SRC_N_2( src2, <LITER> );
			__tmp_3_<LCTR> = SRC_S_2( src2, <LITER> );
			__tmp_4_<LCTR> = SRC_E_2( src2, <LITER> );
			__tmp_5_<LCTR> = SRC_W_2( src2, <LITER> );
			__tmp_6_<LCTR> = SRC_T_2( src2, <LITER> );
			__tmp_7_<LCTR> = SRC_B_2( src2, <LITER> );
			__tmp_8_<LCTR> = SRC_NE_2( src2, <LITER> );
			__tmp_9_<LCTR> = SRC_NW_2( src2, <LITER> );
			__tmp_10_<LCTR> = SRC_SE_2( src2, <LITER> );
			__tmp_11_<LCTR> = SRC_SW_2( src2, <LITER> );
			__tmp_12_<LCTR> = SRC_NT_2( src2, <LITER> );
			__tmp_13_<LCTR> = SRC_NB_2( src2, <LITER> );
			__tmp_14_<LCTR> = SRC_ST_2( src2, <LITER> );
			__tmp_15_<LCTR> = SRC_SB_2( src2, <LITER> );
			__tmp_16_<LCTR> = SRC_ET_2( src2, <LITER> );
			__tmp_17_<LCTR> = SRC_EB_2( src2, <LITER> );
			__tmp_18_<LCTR> = SRC_WT_2( src2, <LITER> );
			__tmp_19_<LCTR> = SRC_WB_2( src2, <LITER> );
		}
		else {
			rho_<LCTR> = + SRC_C_2 ( src2, <LITER> ) + SRC_N_2 ( src2, <LITER> )
			      + SRC_S_2 ( src2, <LITER> ) + SRC_E_2 ( src2, <LITER> )
			      + SRC_W_2 ( src2, <LITER> ) + SRC_T_2 ( src2, <LITER> )
			      + SRC_B_2 ( src2, <LITER> ) + SRC_NE_2( src2, <LITER> )
			      + SRC_NW_2( src2, <LITER> ) + SRC_SE_2( src2, <LITER> )
			      + SRC_SW_2( src2, <LITER> ) + SRC_NT_2( src2, <LITER> )
			      + SRC_NB_2( src2, <LITER> ) + SRC_ST_2( src2, <LITER> )
			      + SRC_SB_2( src2, <LITER> ) + SRC_ET_2( src2, <LITER> )
			      + SRC_EB_2( src2, <LITER> ) + SRC_WT_2( src2, <LITER> )
			      + SRC_WB_2( src2, <LITER> );

			ux_<LCTR> = + SRC_E_2 ( src2, <LITER> ) - SRC_W_2 ( src2, <LITER> )
			     + SRC_NE_2( src2, <LITER> ) - SRC_NW_2( src2, <LITER> )
			     + SRC_SE_2( src2, <LITER> ) - SRC_SW_2( src2, <LITER> )
			     + SRC_ET_2( src2, <LITER> ) + SRC_EB_2( src2, <LITER> )
			     - SRC_WT_2( src2, <LITER> ) - SRC_WB_2( src2, <LITER> );
			uy_<LCTR> = + SRC_N_2 ( src2, <LITER> ) - SRC_S_2 ( src2, <LITER> )
			     + SRC_NE_2( src2, <LITER> ) + SRC_NW_2( src2, <LITER> )
			     - SRC_SE_2( src2, <LITER> ) - SRC_SW_2( src2, <LITER> )
			     + SRC_NT_2( src2, <LITER> ) + SRC_NB_2( src2, <LITER> )
			     - SRC_ST_2( src2, <LITER> ) - SRC_SB_2( src2, <LITER> );
			uz_<LCTR> = + SRC_T_2 ( src2, <LITER> ) - SRC_B_2 ( src2, <LITER> )
			     + SRC_NT_2( src2, <LITER> ) - SRC_NB_2( src2, <LITER> )
			     + SRC_ST_2( src2, <LITER> ) - SRC_SB_2( src2, <LITER> )
			     + SRC_ET_2( src2, <LITER> ) - SRC_EB_2( src2, <LITER> )
			     + SRC_WT_2( src2, <LITER> ) - SRC_WB_2( src2, <LITER> );

			ux_<LCTR> /= rho_<LCTR>;
			uy_<LCTR> /= rho_<LCTR>;
			uz_<LCTR> /= rho_<LCTR>;

			if( CL_TEST_FLAG_SWEEP( src2, ACCEL, <LITER> )) {
				ux_<LCTR> = 0.005f;
				uy_<LCTR> = 0.002f;
				uz_<LCTR> = 0.000f;
			}

			u2_<LCTR> = 1.5f * (ux_<LCTR>*ux_<LCTR> + uy_<LCTR>*uy_<LCTR> + uz_<LCTR>*uz_<LCTR>);
			__tmp_20_<LCTR> = (1.0f-OMEGA)*SRC_C_2( src2, <LITER> ) + DFL1*OMEGA*rho_<LCTR>*(1.0f                                 - u2_<LCTR>);

			__tmp_21_<LCTR> = (1.0f-OMEGA)*SRC_N_2( src2, <LITER> ) + DFL2*OMEGA*rho_<LCTR>*(1.0f +       uy_<LCTR>*(4.5f*uy_<LCTR>       + 3.0f) - u2_<LCTR>);
			__tmp_22_<LCTR> = (1.0f-OMEGA)*SRC_S_2( src2, <LITER> ) + DFL2*OMEGA*rho_<LCTR>*(1.0f +       uy_<LCTR>*(4.5f*uy_<LCTR>       - 3.0f) - u2_<LCTR>);
			__tmp_23_<LCTR> = (1.0f-OMEGA)*SRC_E_2( src2, <LITER> ) + DFL2*OMEGA*rho_<LCTR>*(1.0f +       ux_<LCTR>*(4.5f*ux_<LCTR>       + 3.0f) - u2_<LCTR>);
			__tmp_24_<LCTR> = (1.0f-OMEGA)*SRC_W_2( src2, <LITER> ) + DFL2*OMEGA*rho_<LCTR>*(1.0f +       ux_<LCTR>*(4.5f*ux_<LCTR>       - 3.0f) - u2_<LCTR>);
			__tmp_25_<LCTR> = (1.0f-OMEGA)*SRC_T_2( src2, <LITER> ) + DFL2*OMEGA*rho_<LCTR>*(1.0f +       uz_<LCTR>*(4.5f*uz_<LCTR>       + 3.0f) - u2_<LCTR>);
			__tmp_26_<LCTR> = (1.0f-OMEGA)*SRC_B_2( src2, <LITER> ) + DFL2*OMEGA*rho_<LCTR>*(1.0f +       uz_<LCTR>*(4.5f*uz_<LCTR>       - 3.0f) - u2_<LCTR>);

			__tmp_27_<LCTR> = (1.0f-OMEGA)*SRC_NE_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (+ux_<LCTR>+uy_<LCTR>)*(4.5f*(+ux_<LCTR>+uy_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_28_<LCTR> = (1.0f-OMEGA)*SRC_NW_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (-ux_<LCTR>+uy_<LCTR>)*(4.5f*(-ux_<LCTR>+uy_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_29_<LCTR> = (1.0f-OMEGA)*SRC_SE_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (+ux_<LCTR>-uy_<LCTR>)*(4.5f*(+ux_<LCTR>-uy_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_30_<LCTR> = (1.0f-OMEGA)*SRC_SW_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (-ux_<LCTR>-uy_<LCTR>)*(4.5f*(-ux_<LCTR>-uy_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_31_<LCTR> = (1.0f-OMEGA)*SRC_NT_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (+uy_<LCTR>+uz_<LCTR>)*(4.5f*(+uy_<LCTR>+uz_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_32_<LCTR> = (1.0f-OMEGA)*SRC_NB_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (+uy_<LCTR>-uz_<LCTR>)*(4.5f*(+uy_<LCTR>-uz_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_33_<LCTR> = (1.0f-OMEGA)*SRC_ST_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (-uy_<LCTR>+uz_<LCTR>)*(4.5f*(-uy_<LCTR>+uz_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_34_<LCTR> = (1.0f-OMEGA)*SRC_SB_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (-uy_<LCTR>-uz_<LCTR>)*(4.5f*(-uy_<LCTR>-uz_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_35_<LCTR> = (1.0f-OMEGA)*SRC_ET_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (+ux_<LCTR>+uz_<LCTR>)*(4.5f*(+ux_<LCTR>+uz_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_36_<LCTR> = (1.0f-OMEGA)*SRC_EB_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (+ux_<LCTR>-uz_<LCTR>)*(4.5f*(+ux_<LCTR>-uz_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_37_<LCTR> = (1.0f-OMEGA)*SRC_WT_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (-ux_<LCTR>+uz_<LCTR>)*(4.5f*(-ux_<LCTR>+uz_<LCTR>) + 3.0f) - u2_<LCTR>);
			__tmp_38_<LCTR> = (1.0f-OMEGA)*SRC_WB_2( src2, <LITER> ) + DFL3*OMEGA*rho_<LCTR>*(1.0f + (-ux_<LCTR>-uz_<LCTR>)*(4.5f*(-ux_<LCTR>-uz_<LCTR>) + 3.0f) - u2_<LCTR>);
		}

		<LFRONT>

		if(__tmp_0_<LCTR>) {
			DST_C_2( dst2, <LITER> ) = __tmp_1_<LCTR>;
			DST_S_2( dst2, <LITER> ) = __tmp_2_<LCTR>;
			DST_N_2( dst2, <LITER> ) = __tmp_3_<LCTR>;
			DST_W_2( dst2, <LITER> ) = __tmp_4_<LCTR>;
			DST_E_2( dst2, <LITER> ) = __tmp_5_<LCTR>;
			DST_B_2( dst2, <LITER> ) = __tmp_6_<LCTR>;
			DST_T_2( dst2, <LITER> ) = __tmp_7_<LCTR>;
			DST_SW_2( dst2, <LITER> ) = __tmp_8_<LCTR>;
			DST_SE_2( dst2, <LITER> ) = __tmp_9_<LCTR>;
			DST_NW_2( dst2, <LITER> ) = __tmp_10_<LCTR>;
			DST_NE_2( dst2, <LITER> ) = __tmp_11_<LCTR>;
			DST_SB_2( dst2, <LITER> ) = __tmp_12_<LCTR>;
			DST_ST_2( dst2, <LITER> ) = __tmp_13_<LCTR>;
			DST_NB_2( dst2, <LITER> ) = __tmp_14_<LCTR>;
			DST_NT_2( dst2, <LITER> ) = __tmp_15_<LCTR>;
			DST_WB_2( dst2, <LITER> ) = __tmp_16_<LCTR>;
			DST_WT_2( dst2, <LITER> ) = __tmp_17_<LCTR>;
			DST_EB_2( dst2, <LITER> ) = __tmp_18_<LCTR>;
			DST_ET_2( dst2, <LITER> ) = __tmp_19_<LCTR>;
		}
		else {
			DST_C_2( dst2, <LITER> ) = __tmp_20_<LCTR>;

			DST_N_2( dst2, <LITER> ) = __tmp_21_<LCTR>;
			DST_S_2( dst2, <LITER> ) = __tmp_22_<LCTR>;
			DST_E_2( dst2, <LITER> ) = __tmp_23_<LCTR>;
			DST_W_2( dst2, <LITER> ) = __tmp_24_<LCTR>;
			DST_T_2( dst2, <LITER> ) = __tmp_25_<LCTR>;
			DST_B_2( dst2, <LITER> ) = __tmp_26_<LCTR>;

			DST_NE_2( dst2, <LITER> ) = __tmp_27_<LCTR>;
			DST_NW_2( dst2, <LITER> ) = __tmp_28_<LCTR>;
			DST_SE_2( dst2, <LITER> ) = __tmp_29_<LCTR>;
			DST_SW_2( dst2, <LITER> ) = __tmp_30_<LCTR>;
			DST_NT_2( dst2, <LITER> ) = __tmp_31_<LCTR>;
			DST_NB_2( dst2, <LITER> ) = __tmp_32_<LCTR>;
			DST_ST_2( dst2, <LITER> ) = __tmp_33_<LCTR>;
			DST_SB_2( dst2, <LITER> ) = __tmp_34_<LCTR>;
			DST_ET_2( dst2, <LITER> ) = __tmp_35_<LCTR>;
			DST_EB_2( dst2, <LITER> ) = __tmp_36_<LCTR>;
			DST_WT_2( dst2, <LITER> ) = __tmp_37_<LCTR>;
			DST_WB_2( dst2, <LITER> ) = __tmp_38_<LCTR>;
		}

		<LEND>
	SWEEP_END

<FOOTER>
}

