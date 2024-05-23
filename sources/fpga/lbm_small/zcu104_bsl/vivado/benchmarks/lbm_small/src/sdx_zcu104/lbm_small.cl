#include "config.h"
#include "lbm_1d_array.h"

#define DFL1 (1.0/ 3.0)
#define DFL2 (1.0/18.0)
#define DFL3 (1.0/36.0)

#define CL_MAGIC_CAST(v) ((<ARRQ_src> unsigned int *) ((<ARRQ_src> void *) (&(v))))
#define CL_FLAG_VAR(v) unsigned int* const _aux_ = CL_MAGIC_CAST(v)

#define CL_TEST_FLAG_SWEEP(g,f)     ((*CL_MAGIC_CAST(LOCAL(g, FLAGS))) & (f))
#define CL_SET_FLAG_SWEEP(g,f)      {CL_FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_) |=  (f);}
#define CL_CLEAR_FLAG_SWEEP(g,f)    {CL_FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_) &= ~(f);}
#define CL_CLEAR_ALL_FLAGS_SWEEP(g) {CL_FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_)  =    0;}

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

	float ux, uy, uz, u2, rho;

	<LOOP_0_1> SWEEP_START( 0, 0, 0, 0, 0, SIZE_Z )
		if( CL_TEST_FLAG_SWEEP( src2, OBSTACLE )) {
			DST_C ( dst2 ) = SRC_C ( src2 );
			DST_S ( dst2 ) = SRC_N ( src2 );
			DST_N ( dst2 ) = SRC_S ( src2 );
			DST_W ( dst2 ) = SRC_E ( src2 );
			DST_E ( dst2 ) = SRC_W ( src2 );
			DST_B ( dst2 ) = SRC_T ( src2 );
			DST_T ( dst2 ) = SRC_B ( src2 );
			DST_SW( dst2 ) = SRC_NE( src2 );
			DST_SE( dst2 ) = SRC_NW( src2 );
			DST_NW( dst2 ) = SRC_SE( src2 );
			DST_NE( dst2 ) = SRC_SW( src2 );
			DST_SB( dst2 ) = SRC_NT( src2 );
			DST_ST( dst2 ) = SRC_NB( src2 );
			DST_NB( dst2 ) = SRC_ST( src2 );
			DST_NT( dst2 ) = SRC_SB( src2 );
			DST_WB( dst2 ) = SRC_ET( src2 );
			DST_WT( dst2 ) = SRC_EB( src2 );
			DST_EB( dst2 ) = SRC_WT( src2 );
			DST_ET( dst2 ) = SRC_WB( src2 );
			continue;
		}

		rho = + SRC_C ( src2 ) + SRC_N ( src2 )
		      + SRC_S ( src2 ) + SRC_E ( src2 )
		      + SRC_W ( src2 ) + SRC_T ( src2 )
		      + SRC_B ( src2 ) + SRC_NE( src2 )
		      + SRC_NW( src2 ) + SRC_SE( src2 )
		      + SRC_SW( src2 ) + SRC_NT( src2 )
		      + SRC_NB( src2 ) + SRC_ST( src2 )
		      + SRC_SB( src2 ) + SRC_ET( src2 )
		      + SRC_EB( src2 ) + SRC_WT( src2 )
		      + SRC_WB( src2 );

		ux = + SRC_E ( src2 ) - SRC_W ( src2 )
		     + SRC_NE( src2 ) - SRC_NW( src2 )
		     + SRC_SE( src2 ) - SRC_SW( src2 )
		     + SRC_ET( src2 ) + SRC_EB( src2 )
		     - SRC_WT( src2 ) - SRC_WB( src2 );
		uy = + SRC_N ( src2 ) - SRC_S ( src2 )
		     + SRC_NE( src2 ) + SRC_NW( src2 )
		     - SRC_SE( src2 ) - SRC_SW( src2 )
		     + SRC_NT( src2 ) + SRC_NB( src2 )
		     - SRC_ST( src2 ) - SRC_SB( src2 );
		uz = + SRC_T ( src2 ) - SRC_B ( src2 )
		     + SRC_NT( src2 ) - SRC_NB( src2 )
		     + SRC_ST( src2 ) - SRC_SB( src2 )
		     + SRC_ET( src2 ) - SRC_EB( src2 )
		     + SRC_WT( src2 ) - SRC_WB( src2 );

		ux /= rho;
		uy /= rho;
		uz /= rho;

		if( CL_TEST_FLAG_SWEEP( src2, ACCEL )) {
			ux = 0.005f;
			uy = 0.002f;
			uz = 0.000f;
		}

		u2 = 1.5f * (ux*ux + uy*uy + uz*uz);
		DST_C ( dst2 ) = (1.0f-OMEGA)*SRC_C ( src2 ) + DFL1*OMEGA*rho*(1.0f                                 - u2);

		DST_N ( dst2 ) = (1.0f-OMEGA)*SRC_N ( src2 ) + DFL2*OMEGA*rho*(1.0f +       uy*(4.5f*uy       + 3.0f) - u2);
		DST_S ( dst2 ) = (1.0f-OMEGA)*SRC_S ( src2 ) + DFL2*OMEGA*rho*(1.0f +       uy*(4.5f*uy       - 3.0f) - u2);
		DST_E ( dst2 ) = (1.0f-OMEGA)*SRC_E ( src2 ) + DFL2*OMEGA*rho*(1.0f +       ux*(4.5f*ux       + 3.0f) - u2);
		DST_W ( dst2 ) = (1.0f-OMEGA)*SRC_W ( src2 ) + DFL2*OMEGA*rho*(1.0f +       ux*(4.5f*ux       - 3.0f) - u2);
		DST_T ( dst2 ) = (1.0f-OMEGA)*SRC_T ( src2 ) + DFL2*OMEGA*rho*(1.0f +       uz*(4.5f*uz       + 3.0f) - u2);
		DST_B ( dst2 ) = (1.0f-OMEGA)*SRC_B ( src2 ) + DFL2*OMEGA*rho*(1.0f +       uz*(4.5f*uz       - 3.0f) - u2);

		DST_NE( dst2 ) = (1.0f-OMEGA)*SRC_NE( src2 ) + DFL3*OMEGA*rho*(1.0f + (+ux+uy)*(4.5f*(+ux+uy) + 3.0f) - u2);
		DST_NW( dst2 ) = (1.0f-OMEGA)*SRC_NW( src2 ) + DFL3*OMEGA*rho*(1.0f + (-ux+uy)*(4.5f*(-ux+uy) + 3.0f) - u2);
		DST_SE( dst2 ) = (1.0f-OMEGA)*SRC_SE( src2 ) + DFL3*OMEGA*rho*(1.0f + (+ux-uy)*(4.5f*(+ux-uy) + 3.0f) - u2);
		DST_SW( dst2 ) = (1.0f-OMEGA)*SRC_SW( src2 ) + DFL3*OMEGA*rho*(1.0f + (-ux-uy)*(4.5f*(-ux-uy) + 3.0f) - u2);
		DST_NT( dst2 ) = (1.0f-OMEGA)*SRC_NT( src2 ) + DFL3*OMEGA*rho*(1.0f + (+uy+uz)*(4.5f*(+uy+uz) + 3.0f) - u2);
		DST_NB( dst2 ) = (1.0f-OMEGA)*SRC_NB( src2 ) + DFL3*OMEGA*rho*(1.0f + (+uy-uz)*(4.5f*(+uy-uz) + 3.0f) - u2);
		DST_ST( dst2 ) = (1.0f-OMEGA)*SRC_ST( src2 ) + DFL3*OMEGA*rho*(1.0f + (-uy+uz)*(4.5f*(-uy+uz) + 3.0f) - u2);
		DST_SB( dst2 ) = (1.0f-OMEGA)*SRC_SB( src2 ) + DFL3*OMEGA*rho*(1.0f + (-uy-uz)*(4.5f*(-uy-uz) + 3.0f) - u2);
		DST_ET( dst2 ) = (1.0f-OMEGA)*SRC_ET( src2 ) + DFL3*OMEGA*rho*(1.0f + (+ux+uz)*(4.5f*(+ux+uz) + 3.0f) - u2);
		DST_EB( dst2 ) = (1.0f-OMEGA)*SRC_EB( src2 ) + DFL3*OMEGA*rho*(1.0f + (+ux-uz)*(4.5f*(+ux-uz) + 3.0f) - u2);
		DST_WT( dst2 ) = (1.0f-OMEGA)*SRC_WT( src2 ) + DFL3*OMEGA*rho*(1.0f + (-ux+uz)*(4.5f*(-ux+uz) + 3.0f) - u2);
		DST_WB( dst2 ) = (1.0f-OMEGA)*SRC_WB( src2 ) + DFL3*OMEGA*rho*(1.0f + (-ux-uz)*(4.5f*(-ux-uz) + 3.0f) - u2);
	SWEEP_END

<FOOTER>
}

