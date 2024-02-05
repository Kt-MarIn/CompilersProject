import subprocess

def runcmd(command):
    ret = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if ret.returncode == 0:
        print("success to run ", command)
    else:
        print("error to run ", command)

array = [f'{i:02}' for i in range(1, 16)]
print("Test for 16 samples")
runcmd(f"bin/bplc test/test_2_o01.bpl")
for i in range(15):
    runcmd(f"bin/bplc test/test_2_r{array[i]}.bpl")
