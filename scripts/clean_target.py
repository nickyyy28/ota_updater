import os
import sys
import time
import glob
import shutil


def get_c_files(folder_path):
    c_files = []
    search_path = os.path.join(folder_path, '**/*.c')
    c_files = glob.glob(search_path, recursive=True)
    return c_files


def read_all(src_path):
    fd = open(src_path, 'r', encoding='utf-8')
    txt = ''
    for line in fd.readlines():
        txt += line
    fd.close()
    return txt


if __name__ == '__main__':
    if len(sys.argv) < 3:
        sys.stderr.write(f"param error, usage:{sys.argv[0]} <MACRO_NAME> <OUTPUT_DIR>")
        sys.exit(-1)

    work_dir = os.getcwd()
    output_dir = os.path.join(work_dir, str(sys.argv[2]))

    sources = get_c_files(os.path.join(work_dir, ".."))

    for src in sources:
        try:
            code_text = read_all(src)
        except Exception as e:
            print(f"check {src} encoding is utf-8?")
            continue
        if code_text.__contains__(str(sys.argv[1])):
            filename_without_ext = '.'.join(os.path.basename(src).split('.')[0:-1])
            if os.path.exists(os.path.join(output_dir, filename_without_ext + ".o")):
                print(f"remove {os.path.join(output_dir, filename_without_ext + '.o')}")
                os.remove(os.path.join(output_dir, filename_without_ext + ".o"))
            if os.path.exists(os.path.join(output_dir, filename_without_ext + ".d")):
                print(f"remove {os.path.join(output_dir, filename_without_ext + '.d')}")
                os.remove(os.path.join(output_dir, filename_without_ext + ".d"))
            if os.path.exists(os.path.join(output_dir, filename_without_ext + ".crf")):
                print(f"remove {os.path.join(output_dir, filename_without_ext + '.crf')}")
                os.remove(os.path.join(output_dir, filename_without_ext + ".crf"))
