import sys
import os
import time
import subprocess

os.chdir("..")

print("Part 1:")
print("Compiling lfsr.v for usage in ODIN.")
process1 = subprocess.Popen(["./odin_II", "-V", "./MY_TESTS/lfsr.v", "-o", "./MY_TESTS/lfsr.blif"], stdout=subprocess.PIPE)
process1.wait()
output, error = process1.communicate()

if "Successful High-level synthesis by Odin" not in output:
    print("Failed to synthesize lfsr.v")
    sys.exit(0)
else:
    print("Successfully synthesized the lfsr module.")

print("\n\n")


print("Part 2")
print("Compiling verilog test modules that use the lfsr operator.")

test_modules_for_sim = ["testSingle.v", "testMultiple.v"]

for single_test_module in test_modules_for_sim:
    print("Compiling {} for later simulation.".format(single_test_module))
    blifName = (single_test_module.replace(".v", "") + ".blif")
    process1 = subprocess.Popen(["./odin_II", "-V", "./MY_TESTS/{}".format(single_test_module),
                                 "-o", "./MY_TESTS/{}".format(blifName) ], stdout=subprocess.PIPE)
    process1.wait()
    output, error = process1.communicate()

    if "Successful High-level synthesis by Odin" not in output:
        print("Failed to synthesize {}".format(single_test_module))
        sys.exit(0)
    else:
        print("Successfully synthesized {}".format(single_test_module))
print("\n\n")

print("Part 3")
print("Simulating files against expected output.")

test_modules_for_sim = ["testSingle.v", "testMultiple.v"]

# Generate our input vector file
test_cycles = 10
with open("./MY_TESTS/input_vectors.txt", "w") as f:
    f.write("rst \n")
    # Handle reset
    f.write("0\n")
    for i in range(0, test_cycles):
        f.write("1 \n")

for x in range(0, len(test_modules_for_sim)):
    current_test_module = test_modules_for_sim[x]
    blifName = (current_test_module.replace(".v", "") + ".blif")
    if current_test_module is "testSingle.v":
        # Generate expected output
        with open("./MY_TESTS/expected_output.txt", "w") as f:
            f.write("totalOutput\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \r\n")

            # Expected values
            current_num = 33434
            f.write("0X{:08x} \r\n".format(current_num))
            for i in range(0, test_cycles):
                # Write current
                f.write("0X{:08x} \r\n".format(current_num))

                # Calculate next
                next_num = ((current_num << 1) | (
                    ( ((current_num & (1 << 7)) > 0) ^
                     ( ((current_num & (1 << 16)) > 0) ^
                      ( ((current_num & (1 << 19)) > 0) ^
                       ( ((current_num & (1 << 25)) > 0) ^
                         ((current_num & (1 << 29)) > 0) )))) )) & 0xFFFFFFFF;

                # Write next
                f.write("0X{:08x} \r\n".format(next_num))

                current_num = next_num

        # Run the simulation
        process1 = subprocess.Popen(["./odin_II", "-b", "./MY_TESTS/{}".format(blifName), "-t", "./MY_TESTS/input_vectors.txt",
                                     "-R", "-T", "./MY_TESTS/expected_output.txt"], stdout=subprocess.PIPE)
        process1.wait()
        output, error = process1.communicate()
        if "Vector file \"./MY_TESTS/expected_output.txt\" matches output" not in output:
            print("Failed to simulate of {}".format(current_test_module))
            sys.exit(0)
        else:
           print("Successful simulation of {}".format(current_test_module))

    elif current_test_module is "testMultiple.v":
        # Generate expected output
        with open("./MY_TESTS/expected_output.txt", "w") as f:
            f.write("totalOutput1 totalOutput2 totalOutput3\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\r\n")

            # Expected values
            current_num1 = 3
            current_num2 = 389374879
            current_num3 = 983783
            f.write("0X{:08x} 0X{:08x} 0X{:08x}\r\n".format(current_num1, current_num2, current_num1 * current_num3))
            for i in range(0, test_cycles):
                # Write current
                f.write("0X{:08x} 0X{:08x} 0X{:08x}\r\n".format(current_num1, current_num2, (current_num1 * current_num3) & 0xFFFFFFFF ))

                # Calculate next
                next_num1 = ((current_num1 << 1) | (
                    ( ((current_num1 & (1 << 7)) > 0) ^
                      ( ((current_num1 & (1 << 16)) > 0) ^
                        ( ((current_num1 & (1 << 19)) > 0) ^
                          ( ((current_num1 & (1 << 25)) > 0) ^
                            ((current_num1 & (1 << 29)) > 0) )))) )) & 0xFFFFFFFF

                next_num2 = ((current_num2 << 1) | (
                    ( ((current_num2 & (1 << 7)) > 0) ^
                      ( ((current_num2 & (1 << 16)) > 0) ^
                        ( ((current_num2 & (1 << 19)) > 0) ^
                          ( ((current_num2 & (1 << 25)) > 0) ^
                            ((current_num2 & (1 << 29)) > 0) )))) )) & 0xFFFFFFFF

                next_num3 = (((current_num3 << 1) | (
                    ( ((current_num3 & (1 << 7)) > 0) ^
                      ( ((current_num3 & (1 << 16)) > 0) ^
                        ( ((current_num3 & (1 << 19)) > 0) ^
                          ( ((current_num3 & (1 << 25)) > 0) ^
                            ((current_num3 & (1 << 29)) > 0) )))) ))) & 0xFFFFFFFF

                # Write next
                f.write("0X{:08x} 0X{:08x} 0X{:08x}\r\n".format(next_num1, next_num2, (next_num1 *  next_num3) & 0xFFFFFFFF))

                current_num1 = next_num1
                current_num2 = next_num2
                current_num3 = next_num3

        # Run the simulation
        process1 = subprocess.Popen(["./odin_II", "-b", "./MY_TESTS/{}".format(blifName), "-t", "./MY_TESTS/input_vectors.txt",
                                     "-R", "-T", "./MY_TESTS/expected_output.txt"], stdout=subprocess.PIPE)
        process1.wait()
        output, error = process1.communicate()
        if "Vector file \"./MY_TESTS/expected_output.txt\" matches output" not in output:
            print("Failed to simulate of {}".format(current_test_module))
            sys.exit(0)
        else:
            print("Successful simulation of {}".format(current_test_module))


print("Everything worked as expected.")