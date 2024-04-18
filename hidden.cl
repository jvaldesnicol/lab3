// Copyright (C) 2013-2018 Altera Corporation, San Jose, California, USA. All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify, merge,
// publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to
// whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// This agreement shall be governed in all respects by the laws of the State of California and
// by the laws of the United States of America.

// ACL kernel for matrix multiplication
__kernel void hidden(__global const float *A,
                     __global const float *B,
                     __global float *C,
                     const int M,
                     const int N,
                     const int K,
                     __global const float *input_tile_buf,
                     __global const float *weights_tile_buf,
                     const int input_tile_size,
                     const int output_neurons_tile_size,
                     __global float *output_tile_buf)
{
    // Get the global row and column indices
    int row = get_global_id(0);
    int col = get_global_id(1);

    // Perform matrix multiplication
    float sum = 0.0f;
    for (int k = 0; k < K; k++) {
        sum += A[row * K + k] * B[k * N + col];
    }

    // Store the result in the output matrix
    C[row * N + col] = sum;

    // Additional code for fitting the program
    // hidden(queue, global_size, local_size, input_tile_buf, weights_tile_buf, np.int32(input_tile_size), np.int32(output_neurons_tile_size), output_tile_buf)
    // ...
}

