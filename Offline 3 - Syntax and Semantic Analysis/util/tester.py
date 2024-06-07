import subprocess


paths = ["noerror","sserror","errorrecover","bigtest"]
prefix = "2005001_"
shell_script_name = "run.sh"


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
    dir = "../sample-io/" + path + "/"

    input_filename = path + ".txt"
    log_filename = path + "_log.txt"
    parsetree_fiename = path + "_parsetree.txt"
    error_filename = path + "_error.txt"

    input = read_txt(dir + input_filename)
    log = read_txt(dir + log_filename)
    parsetree = read_txt(dir + parsetree_fiename)
    error = read_txt(dir + error_filename)

    write_txt("./input.c",input)

    run_shell_script("./" + shell_script_name)

    my_log = read_txt("./" + prefix + "log.txt")
    my_parsetree = read_txt("./" + prefix + "parsetree.txt")
    my_error = read_txt("./" + prefix + "error.txt")

    log_mismatches = count_mismatches(log, my_log)
    if(log_mismatches == -1):
        print("\nIncorrect: Log files not of equal length\n")

    parsetree_mismatches = count_mismatches(parsetree, my_parsetree)
    if(parsetree_mismatches == -1):
        print("\nIncorrect: Parsetree files not of equal length\n")

    error_mismatches = count_mismatches(error, my_error)
    if(error_mismatches == -1):
        print("\nIncorrect: Error files not of equal length\n")

    print()

    # if(log_mismatches == -1 or parsetree_mismatches == -1 or error_mismatches == -1):
    #     break

    if(log_mismatches == 0 and parsetree_mismatches == 0 and error_mismatches == 0):
        print("Case " + path + ": CORRECT")
    else:
        print("Case " + path + ": output incorrect")
        print("Log mismatches = " + str(log_mismatches) + "\nParsetree mismatches = " + str(parsetree_mismatches) + "\nError mismatches = " + str(error_mismatches))
        break 

print()





