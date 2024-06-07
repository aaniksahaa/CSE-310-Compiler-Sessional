import subprocess

paths = ["sample without error and warning"]

def read_txt(filename):
    with open(filename) as f:
        data = f.read()
    return data

def write_txt(filename, text):
    with open(filename, 'w', encoding='utf-8') as f:
        f.write(text)

def run_shell_script(filepath):
    subprocess.run(['bash', filepath])

def count_mismatches(str1, str2):
    if len(str1) != len(str2):
        return -1
    mismatch_count = sum(c1 != c2 for c1, c2 in zip(str1, str2))
    return mismatch_count

for path in paths:
    print("\nRunning case: " + path + "\n")
    dir = "../sample-io/" + path

    input = read_txt(dir + "/input.txt")
    log = read_txt(dir + "/log.txt")
    token = read_txt(dir + "/token.txt")

    write_txt("./input.txt",input)

    run_shell_script("./command.sh")

    prefix = "2005001_"

    my_log = read_txt("./" + prefix + "log.txt")
    my_token = read_txt("./" + prefix + "token.txt")

    log_mismatches = count_mismatches(log, my_log)
    if(log_mismatches == -1):
        print("Log files not of equal length")
    token_mismatches = count_mismatches(token, my_token)
    if(log_mismatches == -1):
        print("Token files not of equal length")

    if(log_mismatches == 0 and token_mismatches == 0):
        print("Case " + path + ": CORRECT")
    else:
        print("Case " + path + ": output incorrect")
        print(str(log_mismatches) + " mismatches in log and " + str(token_mismatches) + " misatches in token")

print()





