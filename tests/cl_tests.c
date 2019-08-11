/* Runs the bignum tests from OpenCL C.
 *
 * The OpenCL compiler is invoked with -I src -I tests, so
 * this file has to be compiled from the directory above (..).
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CL/cl.h>

#include "tests_info.c.tmp" // kernel_names, etc.

int test_status = 0;

void eval_test_result(const char *name, int ret) {
    if (ret == 1)
        printf("[ OK ] %s\n", name);
    else if (ret == 0){
        printf("[FAIL] %s\n", name);
        test_status = 1;
    }
    else if(ret == -1){
        printf("[TODO] %s\n", name);
        test_status = -1;
    }
    else {
        printf("[????] %s\n", name);
        test_status = -2;
    }
}

const char *testsource = R"(
    // We need to include the actual source code.
    #include "bignum.c"

    // tests and test kernels
    #include "tests.c"
    #include "tests_wrappers.cl.tmp"
)";

void exit_with_error(int errorcode, char *msg) {
    fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "errorcode: %d\n", errorcode);
    exit(errorcode);
}

int main() {
    printf("Testing bignum from OpenCL C.\n");
    /*
     *  Setup OpenCL.
     */

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1,
        &device_id, &ret_num_devices);

    if (ret != CL_SUCCESS)
        exit_with_error(ret, "clGetDeviceIDs() failed.");

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    if (ret != CL_SUCCESS)
        exit_with_error(ret, "clCreateContext() failed.");

    size_t source_length = strlen(testsource);
    cl_program program = clCreateProgramWithSource(
        context, 1, (const char**) &testsource, &source_length, &ret);
    if (ret != CL_SUCCESS)
        exit_with_error(ret, "clCreateProgramWithSource() failed");

    /*
     *  Build program and check for errors.
     */
    ret = clBuildProgram(program, 1, &device_id,
        "-I \"src/\" -I \"tests\"", NULL, NULL);

    // Get build info, no matter if the compile was successful or not:
    size_t length;
    cl_int errorcode2;

    int info_ret = clGetProgramBuildInfo(program, device_id,
        CL_PROGRAM_BUILD_LOG, 0, NULL, &length);
    if (info_ret != CL_SUCCESS)
        exit_with_error(info_ret, "clGetProgramBuildInfo() (fetch length) failed.");

    char *log = malloc(sizeof(char) * length);
    if (log == NULL)
        exit_with_error(1, "Couldn't allocate memory for build log.");

    info_ret  = clGetProgramBuildInfo(program, device_id,
        CL_PROGRAM_BUILD_LOG, length, log, NULL);
    if (info_ret != CL_SUCCESS)
        exit_with_error(info_ret, "clGetProgramBuildInfo() (fetch log) failed.");

    // Print log and free memory.
    printf("Build log:\n%s\n", log);
    free(log);

    if (ret != CL_SUCCESS)
        exit_with_error(ret, "clBuildProgram() failed.");

    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &ret);
    if (ret != CL_SUCCESS)
        exit_with_error(ret, "clCreateCommandQueue failed.");

    // Create status flag and write it to the device.
    int status = 1;
    cl_mem status_mem = clCreateBuffer(context, CL_MEM_READ_WRITE,
            sizeof(&status), NULL, &ret);
    if (ret != CL_SUCCESS)
        exit_with_error(ret, "Could not create device memory buffer for status flag.");

    ret = clEnqueueWriteBuffer(queue, status_mem, CL_TRUE, 0,
        sizeof(status), &status, 0, NULL, NULL);
    if (ret != CL_SUCCESS)
        exit_with_error(ret, "Could not write status flag to buffer.");

    // Create a compute kernel for each test function wrapper,
    // set the kernel argument, execute it and read the status flag
    // from the device.
    char *testname;
    cl_kernel kernel;

    // test_count is included from test_info.c
    for (int i=0; i<test_count; i++) {
        testname = kernel_names[i];
        kernel = clCreateKernel(program, testname, &ret);
        if (ret != CL_SUCCESS)
            exit_with_error(ret, "clCreateKernel for failed.");

        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &status_mem);
        ret = clEnqueueTask(queue, kernel, 0, NULL, NULL);
        ret = clEnqueueReadBuffer(queue, status_mem, CL_TRUE, 0,
                sizeof(status), &status, 0, NULL, NULL);
        ret = clReleaseKernel(kernel);

        eval_test_result(testname, status);
    }

    return test_status; // set by eval_test_result()
}
