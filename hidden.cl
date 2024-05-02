
__kernel void hidden(__global float* input_tile_buf,
                     __global float* weights_tile_buf,
                     const int input_tile_size,
                     const int output_neurons_tile_size,
                     __global float* output_tile_buf) {
        // Get the global ID
        int global_id = get_global_id(0);
        
        // Calculate the row and column indices
        int row = global_id / output_neurons_tile_size;
        int col = global_id % output_neurons_tile_size;

        // Initialize the output value
        float output_value = 0.0f;
        
        // Calculate the dot product of the arrays
        for (int i = 0; i < input_tile_size; i++) {
            output_value += input_tile_buf[row * input_tile_size + i] * weights_tile_buf[col * input_tile_size + i];
        }
        
        // Store the output value
        output_tile_buf[global_id] = output_value;
    }
