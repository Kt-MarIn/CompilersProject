import subprocess

def runcmd(command):
    ret = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if ret.returncode == 0:
        print("success to run ", command)
    else:
        print("error to run ", command)

array = [f'{i:02}' for i in range(1, 13)]
print("Test for 12 samples")
for i in range(12):
    runcmd(f"bin/bplc test/test_1_r{array[i]}.bpl")
print("Test for my own samples")
runcmd(f"bin/bplc test_ex/test_cyb_r01.bpl")
