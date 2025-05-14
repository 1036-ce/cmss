#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmss_config.h"

void get_slave_cvgmap(const char *fname, char cvgmap[CVG_MAP_SIZE]) {
	FILE *fp = fopen(fname, "r");
	fread(cvgmap, sizeof(char), CVG_MAP_SIZE, fp);
}

void get_master_cvgmap(const char *fname, char cvgmap[CVG_MAP_SIZE]) {
	FILE *fp = fopen(fname, "r");
	fread(cvgmap, sizeof(char), CVG_MAP_SIZE, fp);
}

void merge_cvgmap(char **cvgmaps, const int cnt, char result[CVG_MAP_SIZE]) {
	for (int i = 0; i < CVG_MAP_SIZE; ++i) {
		char val = 0xff;
		for (int j = 0; j < cnt; ++j) {
			val = val & cvgmaps[j][i];
		}
		result[i] = val;
	}
}

void cvgmap_dump_readable(const char *input) {
	char cvgmap[CVG_MAP_SIZE];
	get_slave_cvgmap(input, cvgmap);

	FILE *fp = fopen("tmp", "w");
	for (int i = 0; i < CVG_MAP_SIZE; ++i) {
		unsigned char val = cvgmap[i];
		fprintf(fp, "%d: val: 0x%02X\n", i, val);
	}
}

void cvgmap_dump(const char *input, const char* output) {
	FILE *fp = fopen(output, "w");
  fwrite(input, sizeof(char), CVG_MAP_SIZE, fp);
}

void diff_cvgmap(const char lhs[CVG_MAP_SIZE], const char rhs[CVG_MAP_SIZE],
                 const char *output) {
	FILE *fp = fopen(output, "w");
	for (int i = 0; i < CVG_MAP_SIZE; ++i) {
		unsigned char left  = lhs[i];
		unsigned char right = rhs[i];

		if (left != right) {
			fprintf(fp, "%d: (left: 0x%02X, right: 0x%02X)\n", i, left, right);
		}
	}
}

void validate_cvgmap(int slave_cnt) {

	char **slave_cvgmaps;
	char master_cvgmap[CVG_MAP_SIZE];
	char total_slave_cvgmap[CVG_MAP_SIZE];

	slave_cvgmaps = (char **)malloc(slave_cnt * sizeof(char *));
	for (int i = 0; i < slave_cnt; ++i) {
		slave_cvgmaps[i] = (char *)malloc(CVG_MAP_SIZE * sizeof(char));
	}

	char buf[1024];
	for (int i = 0; i < slave_cnt; ++i) {
		sprintf(buf, "fuzzer%d/fuzz_bitmap", i);
    printf("read bitmap from %s\n", buf);
		get_slave_cvgmap(buf, slave_cvgmaps[i]);
	}

	get_master_cvgmap("cvg_map", master_cvgmap);

	merge_cvgmap(slave_cvgmaps, slave_cnt, total_slave_cvgmap);

	diff_cvgmap(total_slave_cvgmap, master_cvgmap, "cvg_map_diff");
}

void save_merge_cvgmap(int slave_cnt) {
	char **slave_cvgmaps;
	char master_cvgmap[CVG_MAP_SIZE];
	char total_slave_cvgmap[CVG_MAP_SIZE];

	slave_cvgmaps = (char **)malloc(slave_cnt * sizeof(char *));
	for (int i = 0; i < slave_cnt; ++i) {
		slave_cvgmaps[i] = (char *)malloc(CVG_MAP_SIZE * sizeof(char));
	}

	char buf[1024];
	for (int i = 0; i < slave_cnt; ++i) {
		sprintf(buf, "fuzzer%d/fuzz_bitmap", i);
    printf("read bitmap from %s\n", buf);
		get_slave_cvgmap(buf, slave_cvgmaps[i]);
	}

	get_master_cvgmap("cvg_map", master_cvgmap);

	merge_cvgmap(slave_cvgmaps, slave_cnt, total_slave_cvgmap);

  cvgmap_dump(total_slave_cvgmap, "total_cvgmap");
}

int main(int argc, char **argv) {
	char opt;

	while ((opt = getopt(argc, argv, "+s:d:m:")) > 0) {
		switch (opt) {
		case 's': {
			int slave_cnt = atoi(optarg);
			validate_cvgmap(slave_cnt);
			break;
		}
		case 'd': {
			cvgmap_dump_readable(optarg);
			break;
		}
		case 'm': {
			int slave_cnt = atoi(optarg);
			save_merge_cvgmap(slave_cnt);
			break;
		}
		default: {
			fprintf(stderr, "error arguments\n");
			break;
		}
		}
	}

  printf("done\n");
	return 0;
}
