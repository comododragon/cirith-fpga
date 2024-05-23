#define IMG_WIDTH 996
#define IMG_HEIGHT 1040
#define HISTO_WIDTH 256
#define HISTO_HEIGHT 4096
#define NUM_ITERATIONS 200

__attribute__((reqd_work_group_size(1,1,1)))
__kernel void histo(
	__global unsigned int * restrict img, unsigned int imgWidth, unsigned int imgHeight,
	__global unsigned char * restrict hist, unsigned int histoWidth, unsigned int histoHeight
) {
<HEADER>

	for(unsigned iter = 0; iter < NUM_ITERATIONS; iter++) {
		__attribute__((xcl_pipeline_loop)) for(unsigned int i = 0; i < (HISTO_WIDTH * HISTO_HEIGHT); i++)
			<ARR_hist>[i] = 0;

		<LOOP_0_1> for(unsigned int i = 0; i < (IMG_WIDTH * IMG_HEIGHT); i++) {
			const unsigned int value = <ARR_img>[i];
			if(<ARR_hist>[value] < 255)
				++<ARR_hist>[value];
		}
	}

<FOOTER>
}
