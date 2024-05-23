/* $Id: lbm_1d_array.h,v 1.1 2008/03/04 17:30:03 stratton Exp $ */

#ifndef _LBM_MACROS_H_
#define _LBM_MACROS_H_

typedef enum {C = 0,
    N, S, E, W, T, B,
    NE, NW, SE, SW,
    NT, NB, ST, SB,
    ET, EB, WT, WB,
    FLAGS, N_CELL_ENTRIES} CELL_ENTRIES;
#define SIZE   (120)
#define SIZE_X (1*SIZE)
#define SIZE_Y (1*SIZE)
#define SIZE_Z (30)
/*############################################################################*/

typedef float LBM_Grid[SIZE_Z*SIZE_Y*SIZE_X*N_CELL_ENTRIES];
typedef LBM_Grid* LBM_GridPtr;

/*############################################################################*/

#define CALC_INDEX(x,y,z,e) ((e)+N_CELL_ENTRIES*((x)+ \
						 (y)*SIZE_X+(z)*SIZE_X*SIZE_Y))

#define SWEEP_VAR int i;

#define SWEEP_START(x1,y1,z1,x2,y2,z2) \
  for( i = CALC_INDEX(x1, y1, z1, 0); \
       i < CALC_INDEX(x2, y2, z2, 0); \
       i += N_CELL_ENTRIES ) {
#define SWEEP_START_2(x1,y1,z1,x2,y2,z2,incr) \
  for( i = CALC_INDEX(x1, y1, z1, 0); \
       i < CALC_INDEX(x2, y2, z2, 0); \
       i += incr ) {

#define SWEEP_END }

#define SWEEP_X  ((i / N_CELL_ENTRIES) % SIZE_X)
#define SWEEP_Y (((i / N_CELL_ENTRIES) / SIZE_X) % SIZE_Y)
#define SWEEP_Z  ((i / N_CELL_ENTRIES) / (SIZE_X*SIZE_Y))

#define GRID_ENTRY(g,x,y,z,e)          ((g)[CALC_INDEX( x,  y,  z, e)])
#define GRID_ENTRY_SWEEP(g,dx,dy,dz,e) ((g)[CALC_INDEX(dx, dy, dz, e)+(i)])
#define GRID_ENTRY_SWEEP_2(g,dx,dy,dz,e,o) ((g)[CALC_INDEX(dx, dy, dz, e)+(i)+(o)])

#define LOCAL(g,e)       (GRID_ENTRY_SWEEP( g,  0,  0,  0, e ))
#define NEIGHBOR_C(g,e)  (GRID_ENTRY_SWEEP( g,  0,  0,  0, e ))
#define NEIGHBOR_N(g,e)  (GRID_ENTRY_SWEEP( g,  0, +1,  0, e ))
#define NEIGHBOR_S(g,e)  (GRID_ENTRY_SWEEP( g,  0, -1,  0, e ))
#define NEIGHBOR_E(g,e)  (GRID_ENTRY_SWEEP( g, +1,  0,  0, e ))
#define NEIGHBOR_W(g,e)  (GRID_ENTRY_SWEEP( g, -1,  0,  0, e ))
#define NEIGHBOR_T(g,e)  (GRID_ENTRY_SWEEP( g,  0,  0, +1, e ))
#define NEIGHBOR_B(g,e)  (GRID_ENTRY_SWEEP( g,  0,  0, -1, e ))
#define NEIGHBOR_NE(g,e) (GRID_ENTRY_SWEEP( g, +1, +1,  0, e ))
#define NEIGHBOR_NW(g,e) (GRID_ENTRY_SWEEP( g, -1, +1,  0, e ))
#define NEIGHBOR_SE(g,e) (GRID_ENTRY_SWEEP( g, +1, -1,  0, e ))
#define NEIGHBOR_SW(g,e) (GRID_ENTRY_SWEEP( g, -1, -1,  0, e ))
#define NEIGHBOR_NT(g,e) (GRID_ENTRY_SWEEP( g,  0, +1, +1, e ))
#define NEIGHBOR_NB(g,e) (GRID_ENTRY_SWEEP( g,  0, +1, -1, e ))
#define NEIGHBOR_ST(g,e) (GRID_ENTRY_SWEEP( g,  0, -1, +1, e ))
#define NEIGHBOR_SB(g,e) (GRID_ENTRY_SWEEP( g,  0, -1, -1, e ))
#define NEIGHBOR_ET(g,e) (GRID_ENTRY_SWEEP( g, +1,  0, +1, e ))
#define NEIGHBOR_EB(g,e) (GRID_ENTRY_SWEEP( g, +1,  0, -1, e ))
#define NEIGHBOR_WT(g,e) (GRID_ENTRY_SWEEP( g, -1,  0, +1, e ))
#define NEIGHBOR_WB(g,e) (GRID_ENTRY_SWEEP( g, -1,  0, -1, e ))
#define LOCAL_2(g,e,o)       (GRID_ENTRY_SWEEP_2( g,  0,  0,  0, e, o ))
#define NEIGHBOR_C_2(g,e,o)  (GRID_ENTRY_SWEEP_2( g,  0,  0,  0, e, o ))
#define NEIGHBOR_N_2(g,e,o)  (GRID_ENTRY_SWEEP_2( g,  0, +1,  0, e, o ))
#define NEIGHBOR_S_2(g,e,o)  (GRID_ENTRY_SWEEP_2( g,  0, -1,  0, e, o ))
#define NEIGHBOR_E_2(g,e,o)  (GRID_ENTRY_SWEEP_2( g, +1,  0,  0, e, o ))
#define NEIGHBOR_W_2(g,e,o)  (GRID_ENTRY_SWEEP_2( g, -1,  0,  0, e, o ))
#define NEIGHBOR_T_2(g,e,o)  (GRID_ENTRY_SWEEP_2( g,  0,  0, +1, e, o ))
#define NEIGHBOR_B_2(g,e,o)  (GRID_ENTRY_SWEEP_2( g,  0,  0, -1, e, o ))
#define NEIGHBOR_NE_2(g,e,o) (GRID_ENTRY_SWEEP_2( g, +1, +1,  0, e, o ))
#define NEIGHBOR_NW_2(g,e,o) (GRID_ENTRY_SWEEP_2( g, -1, +1,  0, e, o ))
#define NEIGHBOR_SE_2(g,e,o) (GRID_ENTRY_SWEEP_2( g, +1, -1,  0, e, o ))
#define NEIGHBOR_SW_2(g,e,o) (GRID_ENTRY_SWEEP_2( g, -1, -1,  0, e, o ))
#define NEIGHBOR_NT_2(g,e,o) (GRID_ENTRY_SWEEP_2( g,  0, +1, +1, e, o ))
#define NEIGHBOR_NB_2(g,e,o) (GRID_ENTRY_SWEEP_2( g,  0, +1, -1, e, o ))
#define NEIGHBOR_ST_2(g,e,o) (GRID_ENTRY_SWEEP_2( g,  0, -1, +1, e, o ))
#define NEIGHBOR_SB_2(g,e,o) (GRID_ENTRY_SWEEP_2( g,  0, -1, -1, e, o ))
#define NEIGHBOR_ET_2(g,e,o) (GRID_ENTRY_SWEEP_2( g, +1,  0, +1, e, o ))
#define NEIGHBOR_EB_2(g,e,o) (GRID_ENTRY_SWEEP_2( g, +1,  0, -1, e, o ))
#define NEIGHBOR_WT_2(g,e,o) (GRID_ENTRY_SWEEP_2( g, -1,  0, +1, e, o ))
#define NEIGHBOR_WB_2(g,e,o) (GRID_ENTRY_SWEEP_2( g, -1,  0, -1, e, o ))


#define COLLIDE_STREAM
#ifdef COLLIDE_STREAM

#define SRC_C(g)  (LOCAL( g, C  ))
#define SRC_N(g)  (LOCAL( g, N  ))
#define SRC_S(g)  (LOCAL( g, S  ))
#define SRC_E(g)  (LOCAL( g, E  ))
#define SRC_W(g)  (LOCAL( g, W  ))
#define SRC_T(g)  (LOCAL( g, T  ))
#define SRC_B(g)  (LOCAL( g, B  ))
#define SRC_NE(g) (LOCAL( g, NE ))
#define SRC_NW(g) (LOCAL( g, NW ))
#define SRC_SE(g) (LOCAL( g, SE ))
#define SRC_SW(g) (LOCAL( g, SW ))
#define SRC_NT(g) (LOCAL( g, NT ))
#define SRC_NB(g) (LOCAL( g, NB ))
#define SRC_ST(g) (LOCAL( g, ST ))
#define SRC_SB(g) (LOCAL( g, SB ))
#define SRC_ET(g) (LOCAL( g, ET ))
#define SRC_EB(g) (LOCAL( g, EB ))
#define SRC_WT(g) (LOCAL( g, WT ))
#define SRC_WB(g) (LOCAL( g, WB ))

#define DST_C(g)  (NEIGHBOR_C ( g, C  ))
#define DST_N(g)  (NEIGHBOR_N ( g, N  ))
#define DST_S(g)  (NEIGHBOR_S ( g, S  ))
#define DST_E(g)  (NEIGHBOR_E ( g, E  ))
#define DST_W(g)  (NEIGHBOR_W ( g, W  ))
#define DST_T(g)  (NEIGHBOR_T ( g, T  ))
#define DST_B(g)  (NEIGHBOR_B ( g, B  ))
#define DST_NE(g) (NEIGHBOR_NE( g, NE ))
#define DST_NW(g) (NEIGHBOR_NW( g, NW ))
#define DST_SE(g) (NEIGHBOR_SE( g, SE ))
#define DST_SW(g) (NEIGHBOR_SW( g, SW ))
#define DST_NT(g) (NEIGHBOR_NT( g, NT ))
#define DST_NB(g) (NEIGHBOR_NB( g, NB ))
#define DST_ST(g) (NEIGHBOR_ST( g, ST ))
#define DST_SB(g) (NEIGHBOR_SB( g, SB ))
#define DST_ET(g) (NEIGHBOR_ET( g, ET ))
#define DST_EB(g) (NEIGHBOR_EB( g, EB ))
#define DST_WT(g) (NEIGHBOR_WT( g, WT ))
#define DST_WB(g) (NEIGHBOR_WB( g, WB ))

#define SRC_C_2(g,o)  (LOCAL_2( g, C , o ))
#define SRC_N_2(g,o)  (LOCAL_2( g, N , o ))
#define SRC_S_2(g,o)  (LOCAL_2( g, S , o ))
#define SRC_E_2(g,o)  (LOCAL_2( g, E , o ))
#define SRC_W_2(g,o)  (LOCAL_2( g, W , o ))
#define SRC_T_2(g,o)  (LOCAL_2( g, T , o ))
#define SRC_B_2(g,o)  (LOCAL_2( g, B , o ))
#define SRC_NE_2(g,o) (LOCAL_2( g, NE, o ))
#define SRC_NW_2(g,o) (LOCAL_2( g, NW, o ))
#define SRC_SE_2(g,o) (LOCAL_2( g, SE, o ))
#define SRC_SW_2(g,o) (LOCAL_2( g, SW, o ))
#define SRC_NT_2(g,o) (LOCAL_2( g, NT, o ))
#define SRC_NB_2(g,o) (LOCAL_2( g, NB, o ))
#define SRC_ST_2(g,o) (LOCAL_2( g, ST, o ))
#define SRC_SB_2(g,o) (LOCAL_2( g, SB, o ))
#define SRC_ET_2(g,o) (LOCAL_2( g, ET, o ))
#define SRC_EB_2(g,o) (LOCAL_2( g, EB, o ))
#define SRC_WT_2(g,o) (LOCAL_2( g, WT, o ))
#define SRC_WB_2(g,o) (LOCAL_2( g, WB, o ))

#define DST_C_2(g,o)  (NEIGHBOR_C_2 ( g, C , o ))
#define DST_N_2(g,o)  (NEIGHBOR_N_2 ( g, N , o ))
#define DST_S_2(g,o)  (NEIGHBOR_S_2 ( g, S , o ))
#define DST_E_2(g,o)  (NEIGHBOR_E_2 ( g, E , o ))
#define DST_W_2(g,o)  (NEIGHBOR_W_2 ( g, W , o ))
#define DST_T_2(g,o)  (NEIGHBOR_T_2 ( g, T , o ))
#define DST_B_2(g,o)  (NEIGHBOR_B_2 ( g, B , o ))
#define DST_NE_2(g,o) (NEIGHBOR_NE_2( g, NE, o ))
#define DST_NW_2(g,o) (NEIGHBOR_NW_2( g, NW, o ))
#define DST_SE_2(g,o) (NEIGHBOR_SE_2( g, SE, o ))
#define DST_SW_2(g,o) (NEIGHBOR_SW_2( g, SW, o ))
#define DST_NT_2(g,o) (NEIGHBOR_NT_2( g, NT, o ))
#define DST_NB_2(g,o) (NEIGHBOR_NB_2( g, NB, o ))
#define DST_ST_2(g,o) (NEIGHBOR_ST_2( g, ST, o ))
#define DST_SB_2(g,o) (NEIGHBOR_SB_2( g, SB, o ))
#define DST_ET_2(g,o) (NEIGHBOR_ET_2( g, ET, o ))
#define DST_EB_2(g,o) (NEIGHBOR_EB_2( g, EB, o ))
#define DST_WT_2(g,o) (NEIGHBOR_WT_2( g, WT, o ))
#define DST_WB_2(g,o) (NEIGHBOR_WB_2( g, WB, o ))

#else /* COLLIDE_STREAM */

#define SRC_C(g)  (NEIGHBOR_C ( g, C  ))
#define SRC_N(g)  (NEIGHBOR_S ( g, N  ))
#define SRC_S(g)  (NEIGHBOR_N ( g, S  ))
#define SRC_E(g)  (NEIGHBOR_W ( g, E  ))
#define SRC_W(g)  (NEIGHBOR_E ( g, W  ))
#define SRC_T(g)  (NEIGHBOR_B ( g, T  ))
#define SRC_B(g)  (NEIGHBOR_T ( g, B  ))
#define SRC_NE(g) (NEIGHBOR_SW( g, NE ))
#define SRC_NW(g) (NEIGHBOR_SE( g, NW ))
#define SRC_SE(g) (NEIGHBOR_NW( g, SE ))
#define SRC_SW(g) (NEIGHBOR_NE( g, SW ))
#define SRC_NT(g) (NEIGHBOR_SB( g, NT ))
#define SRC_NB(g) (NEIGHBOR_ST( g, NB ))
#define SRC_ST(g) (NEIGHBOR_NB( g, ST ))
#define SRC_SB(g) (NEIGHBOR_NT( g, SB ))
#define SRC_ET(g) (NEIGHBOR_WB( g, ET ))
#define SRC_EB(g) (NEIGHBOR_WT( g, EB ))
#define SRC_WT(g) (NEIGHBOR_EB( g, WT ))
#define SRC_WB(g) (NEIGHBOR_ET( g, WB ))

#define DST_C(g)  (LOCAL( g, C  ))
#define DST_N(g)  (LOCAL( g, N  ))
#define DST_S(g)  (LOCAL( g, S  ))
#define DST_E(g)  (LOCAL( g, E  ))
#define DST_W(g)  (LOCAL( g, W  ))
#define DST_T(g)  (LOCAL( g, T  ))
#define DST_B(g)  (LOCAL( g, B  ))
#define DST_NE(g) (LOCAL( g, NE ))
#define DST_NW(g) (LOCAL( g, NW ))
#define DST_SE(g) (LOCAL( g, SE ))
#define DST_SW(g) (LOCAL( g, SW ))
#define DST_NT(g) (LOCAL( g, NT ))
#define DST_NB(g) (LOCAL( g, NB ))
#define DST_ST(g) (LOCAL( g, ST ))
#define DST_SB(g) (LOCAL( g, SB ))
#define DST_ET(g) (LOCAL( g, ET ))
#define DST_EB(g) (LOCAL( g, EB ))
#define DST_WT(g) (LOCAL( g, WT ))
#define DST_WB(g) (LOCAL( g, WB ))

#define SRC_C_2(g,o)  (NEIGHBOR_C_2 ( g, C , o ))
#define SRC_N_2(g,o)  (NEIGHBOR_S_2 ( g, N , o ))
#define SRC_S_2(g,o)  (NEIGHBOR_N_2 ( g, S , o ))
#define SRC_E_2(g,o)  (NEIGHBOR_W_2 ( g, E , o ))
#define SRC_W_2(g,o)  (NEIGHBOR_E_2 ( g, W , o ))
#define SRC_T_2(g,o)  (NEIGHBOR_B_2 ( g, T , o ))
#define SRC_B_2(g,o)  (NEIGHBOR_T_2 ( g, B , o ))
#define SRC_NE_2(g,o) (NEIGHBOR_SW_2( g, NE, o ))
#define SRC_NW_2(g,o) (NEIGHBOR_SE_2( g, NW, o ))
#define SRC_SE_2(g,o) (NEIGHBOR_NW_2( g, SE, o ))
#define SRC_SW_2(g,o) (NEIGHBOR_NE_2( g, SW, o ))
#define SRC_NT_2(g,o) (NEIGHBOR_SB_2( g, NT, o ))
#define SRC_NB_2(g,o) (NEIGHBOR_ST_2( g, NB, o ))
#define SRC_ST_2(g,o) (NEIGHBOR_NB_2( g, ST, o ))
#define SRC_SB_2(g,o) (NEIGHBOR_NT_2( g, SB, o ))
#define SRC_ET_2(g,o) (NEIGHBOR_WB_2( g, ET, o ))
#define SRC_EB_2(g,o) (NEIGHBOR_WT_2( g, EB, o ))
#define SRC_WT_2(g,o) (NEIGHBOR_EB_2( g, WT, o ))
#define SRC_WB_2(g,o) (NEIGHBOR_ET_2( g, WB, o ))

#define DST_C_2(g,o)  (LOCAL_2( g, C , o ))
#define DST_N_2(g,o)  (LOCAL_2( g, N , o ))
#define DST_S_2(g,o)  (LOCAL_2( g, S , o ))
#define DST_E_2(g,o)  (LOCAL_2( g, E , o ))
#define DST_W_2(g,o)  (LOCAL_2( g, W , o ))
#define DST_T_2(g,o)  (LOCAL_2( g, T , o ))
#define DST_B_2(g,o)  (LOCAL_2( g, B , o ))
#define DST_NE_2(g,o) (LOCAL_2( g, NE, o ))
#define DST_NW_2(g,o) (LOCAL_2( g, NW, o ))
#define DST_SE_2(g,o) (LOCAL_2( g, SE, o ))
#define DST_SW_2(g,o) (LOCAL_2( g, SW, o ))
#define DST_NT_2(g,o) (LOCAL_2( g, NT, o ))
#define DST_NB_2(g,o) (LOCAL_2( g, NB, o ))
#define DST_ST_2(g,o) (LOCAL_2( g, ST, o ))
#define DST_SB_2(g,o) (LOCAL_2( g, SB, o ))
#define DST_ET_2(g,o) (LOCAL_2( g, ET, o ))
#define DST_EB_2(g,o) (LOCAL_2( g, EB, o ))
#define DST_WT_2(g,o) (LOCAL_2( g, WT, o ))
#define DST_WB_2(g,o) (LOCAL_2( g, WB, o ))

#endif /* COLLIDE_STREAM */

#define MAGIC_CAST(v) ((unsigned int*) ((void*) (&(v))))
#define FLAG_VAR(v) unsigned int* const _aux_ = MAGIC_CAST(v)

#define TEST_FLAG_SWEEP(g,f)     ((*MAGIC_CAST(LOCAL(g, FLAGS))) & (f))
#define SET_FLAG_SWEEP(g,f)      {FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_) |=  (f);}
#define CLEAR_FLAG_SWEEP(g,f)    {FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_) &= ~(f);}
#define CLEAR_ALL_FLAGS_SWEEP(g) {FLAG_VAR(LOCAL(g, FLAGS)); (*_aux_)  =    0;}

#define TEST_FLAG(g,x,y,z,f)     ((*MAGIC_CAST(GRID_ENTRY(g, x, y, z, FLAGS))) & (f))
#define SET_FLAG(g,x,y,z,f)      {FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_) |=  (f);}
#define CLEAR_FLAG(g,x,y,z,f)    {FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_) &= ~(f);}
#define CLEAR_ALL_FLAGS(g,x,y,z) {FLAG_VAR(GRID_ENTRY(g, x, y, z, FLAGS)); (*_aux_)  =    0;}

/*############################################################################*/

#endif /* _LBM_MACROS_H_ */
