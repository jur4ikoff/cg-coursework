#pragma once

#define EXP_COUNT 100
#define IMG_SIZE 200

#define SAMPLE_EXP "./../research/sample_exp.csv"
#define DEPTH_EXP "./../research/depth_exp.csv"

void run_sample_count_exp(size_t start, size_t end, size_t step);
void run_depth_exp(size_t start, size_t end, size_t step);